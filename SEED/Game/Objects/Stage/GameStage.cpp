#include "GameStage.h"

//============================================================================
//	include
//============================================================================
#include <SEED/Lib/JsonAdapter/JsonAdapter.h>
#include <SEED/Lib/MagicEnumAdapter/EnumAdapter.h>
#include <SEED/Source/Manager/InputManager/InputManager.h>
#include <SEED/Source/Manager/AudioManager/AudioManager.h>
#include <Environment/Environment.h>
#include <Game/GameSystem.h>
#include <Game/Components/StageObjectComponent.h>
#include <Game/Scene/Scene_Game/Scene_Game.h>
#include <Game/Objects/Stage/Objects/Player/Entity/Player.h>
#include <Game/Objects/Stage/Objects/Warp/Warp.h>
#include <Game/Objects/Stage/Objects/Laser/LaserLauncher.h>
#include <Game/Objects/Stage/Methods/GameStageBuilder.h>
#include <Game/Objects/Stage/Methods/GameStageHelper.h>

// imgui
#include <SEED/Source/Manager/ImGuiManager/ImGuiManager.h>

//============================================================================
//	GameStage classMethods
//============================================================================

void GameStage::Initialize(int currentStageIndex) {

    // 境界線
    borderLine_ = std::make_unique<BorderLine>();
    borderLine_->Initialize();
    // ワープ管理
    warpController_ = std::make_unique<GameStageWarpController>();
    warpController_->Initialize();
    // レーザー管理
    laserController_ = std::make_unique<GameStageLaserController>();
    laserController_->Initialize();
    laserController_->SetWarpController(warpController_.get());

    // json適応
    ApplyJson();

    // 最初のステージを構築する
    maxStageCount_ = GameStageHelper::GetCSVFileCount(); // 最大ステージ数をCSVファイル数から取得
    currentStageIndex_ = currentStageIndex;              // 最初のステージインデックス
    isRemoveHologram_ = false;
    BuildStage();

    // ステージのサイズを計算
    CalculateCurrentStageRange();
    // カメラ調整の初期化
    cameraAdjuster_.SetStageRange(currentStageRange_.value());
    cameraAdjuster_.Update();

    // UI画像
    removeUI_ = Sprite("UI/buttons.png");
    removeUI_.size = { 231.0f,129.0f };
    removeUI_.clipSize = { 231.0f,129.0f };
    removeUI_.anchorPoint = { 0.5f,0.5f };
    removeUI_.layer = 20;
    removeUI_.isApplyViewMat = true;
    removeUI_.transform.scale = Vector2(0.25f);
    if (Input::GetRecentInputDevice() == InputDevice::GAMEPAD) {
        removeUI_.clipLT.y = 129.0f * 3.0f;
    } else {
        removeUI_.clipLT.y = 129.0f * 1.0f;
    }

    // テキストボックス
    removeUI_TextBox = TextBox2D("回収");
    removeUI_TextBox.SetFont("DefaultAssets/Digital/851Gkktt_005.ttf");
    removeUI_TextBox.fontSize = 20.0f;
    removeUI_TextBox.size = { 211.0f,20.0f };
    removeUI_TextBox.glyphSpacing = 0.4f;
    removeUI_TextBox.layer = 20;
    removeUI_TextBox.textBoxVisible = false;
    removeUI_TextBox.isApplyViewMat = true;
    removeUI_TextBox.useOutline = true;
    removeUI_TextBox.outlineWidth = 4.0f;
}

void GameStage::Reset() {

    if (!requestInitialize_) {
        return;
    }

    // リクエストがあれば初期化する
    BuildStage();
    // カメラ調整の初期化
    cameraAdjuster_.SetStageRange(currentStageRange_.value());
    cameraAdjuster_.Update();
    requestInitialize_ = false;
}

///////////////////////////////////////////////////////////////////////////////
//
// ステージ構築処理
//
///////////////////////////////////////////////////////////////////////////////

void GameStage::BuildStage() {

    // 全てのオブジェクトを破棄
    for (GameObject2D* object : objects_) {
        delete object;
    }
    objects_.clear();
    for (GameObject2D* object : hologramObjects_) {
        delete object;
    }
    hologramObjects_.clear();

    // 現在のステージ番号でステージの構築
    std::string fileName = "stage_" + std::to_string(currentStageIndex_) + ".csv";

    // ステージの構築
    GameStageBuilder stageBuilder{};
    objects_ = stageBuilder.CreateFromCSVFile(fileName, stageObjectMapTileSize_);
    // コライダーの登録
    stageBuilder.CreateColliders(objects_, stageObjectMapTileSize_);
    // ステージのサイズを計算
    CalculateCurrentStageRange();

    // リストからプレイヤーのポインタを渡す
    GetListsPlayerPtr();
    // リストから必要なポインタを渡す
    warpController_->SetPlayer(player_);
    SetListsWarpPtr(StageObjectCommonState::None);
    SetListsLaserLaunchersPtr(StageObjectCommonState::None);

    // 状態をプレイ中に遷移させる
    currentState_ = State::Play;

    // 最初の座標を渡しておく
    onBlockPlayerRecordData_.push_back({ false,player_->GetOwner()->GetWorldTranslate() });
    deadMomentLaserCollisions_.clear();
}

//Objectのアクティブ・非アクティブ設定
void GameStage::SetIsActive(bool isActive) {

    // アクティブを設定する
    for (GameObject2D* object : std::views::join(std::array{ objects_, hologramObjects_ })) {

        object->SetIsActive(isActive);

        if (!isActive) {
            object->SetIsMustDraw(true);
        } else {
            object->SetIsMustDraw(false);
        }

        if (StageObjectComponent* component = object->GetComponent<StageObjectComponent>()) {

            // レーザーは別でアクティブを設定する
            if (LaserLauncher* laserLauncher = component->GetStageObject<LaserLauncher>()) {

                laserLauncher->SetIsLaserActive(isActive);
            }
            // ワープも別でアクティブを設定する
            if (Warp* warp = component->GetStageObject<Warp>()) {

                // 状態を元に戻す
                warp->ResetAnimation();
            }
        }
    }
}

void GameStage::AddDisActiveObject(GameObject2D* object) {

    disActiveObjects_.push_back(object);
}

// 非アクティブオブジェクトの再アクティブ化
void GameStage::ReActivateDisActiveObjects() {

    for (auto& object : disActiveObjects_) {

        object->SetIsActive(true);

        if (StageObjectComponent* component = object->GetComponent<StageObjectComponent>()) {
            // レーザーは別でアクティブを設定する
            if (LaserLauncher* laserLauncher = component->GetStageObject<LaserLauncher>()) {

                laserLauncher->SetIsLaserActive(true);
            }
            // ワープも別でアクティブを設定する
            if (Warp* warp = component->GetStageObject<Warp>()) {

                // 状態を元に戻す
                warp->SetNone();
            }
        }
        object = nullptr;
    }
    disActiveObjects_.clear();
}

void GameStage::RecordPlayerOnBlock(const Vector2& translate) {

    // 座標をブロックの中心位置にする
    const float tileSize = stageObjectMapTileSize_;
    Vector2 blockPos = Vector2(std::round(translate.x / tileSize) * tileSize,
        player_->GetOwner()->GetWorldTranslate().y);

    // 踏まれたブロック位置の座標をセット
    onBlockPlayerRecordData_.push_back({ borderLine_->IsActive(),blockPos });
    while (maxRecordCount_ < onBlockPlayerRecordData_.size()) {

        // 最大数を超えた場合古い座標を削除する
        onBlockPlayerRecordData_.pop_front();
    }
}

bool GameStage::IsTriggredAnyDevice() const {

    return player_->IsTriggredAnyDevice();
}

/////////////////////////////////////////////////////////////////////////
//
// 全体の更新処理
//
/////////////////////////////////////////////////////////////////////////
void GameStage::Update(bool isUpdateBorderLine) {

    switch (currentState_) {
        //============================================================================
        //	ゲームプレイ中の更新処理
        //============================================================================
    case GameStage::State::Play:

        UpdatePlay(isUpdateBorderLine);
        break;
        //============================================================================
        //	クリア時の処理
        //============================================================================
    case GameStage::State::Clear:

        UpdateClear();
        break;
        //============================================================================
        //	プレイヤーがやられた時の処理
        //============================================================================
    case GameStage::State::Dead:

        UpdateDead();
        break;
        //============================================================================
        //	リトライ時の処理
        //============================================================================
    case GameStage::State::Retry:

        break;
        //============================================================================
        //	セレクト画面に戻る時の処理
        //============================================================================
    case GameStage::State::Select:

        break;
    }
}

//////////////////////////////////////////////////////////////////////////
//
// 各状態の更新処理
//
//////////////////////////////////////////////////////////////////////////

void GameStage::UpdatePlay(bool isUpdateBorderLine) {

    // ワープの更新処理
    UpdateWarp();
    // レーザーの更新処理
    UpdateLaserLauncher();

    // 境界線の更新処理(ホログラムオブジェクトの作成も行っている)
    if (isUpdateBorderLine) {
        UpdateBorderLine();
    }

    //プレイヤーが境界線を越えたか判定
    CheckPlayerCrossedBorderLine();
    //プレイヤーがカメラの範囲外に出たか判定
    CheckPlayerOutOfCamera();

    // クリア判定
    CheckClear();
    // 死亡判定
    CheckPlayerDead();

    // カメラの調整
    cameraAdjuster_.Update();

    // uiの更新
    UpdateRemoveUI();

    // 背景描画
    if (player_) {
        backDrawer_.Update(player_->GetOwner()->GetWorldTranslate());
    }

    // ホログラム専用更新処理
    UpdateHologramAppearanceUpdateAnimation();
}

void GameStage::UpdateWarp() {

    // ワープの更新処理
    warpController_->Update();
}

void GameStage::UpdateLaserLauncher() {

    // レーザーの更新処理
    laserController_->Update();
}

void GameStage::UpdateBorderLine() {

    // プレイヤーのワールド座標
    const Vector2 playerWorldTranslate = player_->GetOwner()->GetWorldTranslate();

    // プレイヤーの入力処理に応じて境界線を置いたり外したりする
    // 境界線がまだ置かれていないとき
    if (!borderLine_->IsActive() && player_->IsPutBorder()) {

        // 境界線を置いてホログラムオブジェクトを構築する
        PutBorderLine();

        //タイマーを減らす
        removeUITimer_.Update(-1.0f);

        // SE
        const float kSEVolume = 0.24f;
        AudioManager::PlayAudio(AudioDictionary::Get("ミラー_設置"), false, kSEVolume);

    } else if (borderLine_->CanTransitionDisable(player_->GetSprite().transform.translate,
        stageObjectMapTileSize_)) {

        // ワープ中は境界線を消せない && ホログラム中は回収できない
        if (!warpController_->IsWarping()) {

            if (player_->IsRemoveBorder()) {

                //プレイヤーがホログラム状態のときは回収できない
                if (player_->GetIsHologram() == true) {
                    borderLine_->SetIsShaking(true);
                } else {

                    // 境界線を非アクティブ状態にしてホログラムオブジェクトを全て破棄する
                    isRemoveHologram_ = true;

                    // SE
                    const float kSEVolume = 0.24f;
                    AudioManager::PlayAudio(AudioDictionary::Get("ミラー_回収"), false, kSEVolume);
                }
            }

            // //プレイヤーがホログラム状態のときはUIを表示しない
            if (player_->GetIsHologram() == true) {
                //タイマーを減らす
                removeUITimer_.Update(-1.0f);
            } else {
               
                removeUITimer_.Update();
            }
        } else {
            //タイマーを減らす
            removeUITimer_.Update(-1.0f);
        }
    } else {
        //タイマーを減らす
        removeUITimer_.Update(-1.0f);
    }

    // 境界線のX座標を一番占有率の高いオブジェクトの端に設定する
    float axisX = GameStageHelper::ComputeBorderAxisXFromContact(objects_,
        player_->GetSprite(), playerWorldTranslate, player_->GetMoveDirection(), stageObjectMapTileSize_);
    Vector2 placePos = playerWorldTranslate;
    placePos.x = GameStageHelper::BorderAxisXFromPlayerDirection(axisX, player_->GetMoveDirection(), stageObjectMapTileSize_);

    // 境界線の更新処理
    borderLine_->Update(placePos, playerWorldTranslate.y + player_->GetSprite().size.y, stageObjectMapTileSize_);
}

void GameStage::UpdateHologramAppearanceUpdateAnimation() {

    // ホログラム専用の更新処理
    for (const auto& object : hologramObjects_) {
        if (StageObjectComponent* component = object->GetComponent<StageObjectComponent>()) {

            component->AppearanceUpdateAnimation(blockAppearanceBaseDuration_,
                blockAppearanceSpacing_, blockAppearanceEasing_);
        }
    }
}

void GameStage::UpdateClear() {

    // カメラの調整処理
    cameraAdjuster_.Update();

    // ここでクリア状態の更新を行う(プレイヤーを喜ばせるなど)
}

void GameStage::UpdateDead() {

    // ホログラムを非アクティブ状態に
    isRemoveHologram_ = true;
    //境界線を削除
    RemoveBorderLine();
    //ステージを再構築
    BuildStage();
    //stateをPlayに戻す
    currentState_ = State::Play;
}

void GameStage::UpdateRetry() {


}

void GameStage::UpdateReturnSelect() {


}

/////////////////////////////////////////////////////////////////////////
//
// リストからプレイヤーのポインタを取得して渡す
//
/////////////////////////////////////////////////////////////////////////

void GameStage::GetListsPlayerPtr() {
    player_ = nullptr;
    for (GameObject2D* object : objects_) {
        if (StageObjectComponent* component = object->GetComponent<StageObjectComponent>()) {
            if (component->GetStageObjectType() == StageObjectType::Player) {

                player_ = component->GetStageObject<Player>();
                break;
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////
//
// ワープのポインタをリストから取得して渡す
//
//////////////////////////////////////////////////////////////////////////

void GameStage::SetListsWarpPtr(StageObjectCommonState state) {

    std::vector<Warp*> warps{};
    if (state == StageObjectCommonState::None) {
        for (GameObject2D* object : objects_) {
            if (StageObjectComponent* component = object->GetComponent<StageObjectComponent>()) {
                if (component->GetStageObjectType() == StageObjectType::Warp) {

                    // ワープのポインタを追加
                    warps.push_back(component->GetStageObject<Warp>());
                }
            }
        }
    } else if (state == StageObjectCommonState::Hologram) {
        for (GameObject2D* object : hologramObjects_) {
            if (StageObjectComponent* component = object->GetComponent<StageObjectComponent>()) {
                if (component->GetStageObjectType() == StageObjectType::Warp) {

                    // ワープのポインタを追加
                    warps.push_back(component->GetStageObject<Warp>());
                }
            }
        }
    }
    // ワープのポインタを渡す
    warpController_->SetWarps(state, warps);
}

//////////////////////////////////////////////////////////////////////////
//
// レーザー発射台のポインタをリストから取得して渡す
//
//////////////////////////////////////////////////////////////////////////

void GameStage::SetListsLaserLaunchersPtr(StageObjectCommonState state) {

    std::vector<LaserLauncher*> launcheres{};
    if (state == StageObjectCommonState::None) {
        for (GameObject2D* object : objects_) {
            if (StageObjectComponent* component = object->GetComponent<StageObjectComponent>()) {
                if (component->GetStageObjectType() == StageObjectType::LaserLauncher) {

                    // レーザー発射台のポインタを追加
                    launcheres.push_back(component->GetStageObject<LaserLauncher>());
                }
            }
        }
    } else if (state == StageObjectCommonState::Hologram) {
        for (GameObject2D* object : hologramObjects_) {
            if (StageObjectComponent* component = object->GetComponent<StageObjectComponent>()) {
                if (component->GetStageObjectType() == StageObjectType::LaserLauncher) {

                    // レーザー発射台のポインタを追加
                    launcheres.push_back(component->GetStageObject<LaserLauncher>());
                }
            }
        }
    }
    // レーザー発射台のポインタを渡す
    laserController_->SetLaserLauncheres(state, launcheres);
}

/////////////////////////////////////////////////////////////////////////
//
// 境界線設置
//
/////////////////////////////////////////////////////////////////////////

void GameStage::PutBorderLine() {

    // プレイヤーの向き
    const LR playerDirection = player_->GetMoveDirection();
    // プレイヤーのワールド座標
    const Vector2 playerWorldTranslate = player_->GetOwner()->GetWorldTranslate();

    // 境界線のX座標を一番占有率の高いオブジェクトの端に設定する
    float axisX = GameStageHelper::ComputeBorderAxisXFromContact(objects_,
        player_->GetSprite(), playerWorldTranslate, playerDirection, stageObjectMapTileSize_);
    axisX = GameStageHelper::BorderAxisXFromPlayerDirection(axisX, playerDirection, stageObjectMapTileSize_);

    // 境界線をアクティブ状態にする
    borderLine_->SetActivate();
    //向きの設定
    borderLine_->SetDirection(playerDirection);

    // ホログラムオブジェクトを生成する
    GameStageBuilder stageBuilder{};
    hologramObjects_ = stageBuilder.CreateFromBorderLine(objects_, axisX, playerWorldTranslate.y,
        static_cast<int>(playerDirection), stageObjectMapTileSize_);
    // コライダーの登録
    stageBuilder.CreateColliders(hologramObjects_, stageObjectMapTileSize_);
    // リストから必要なポインタを渡す
    warpController_->SetPlayer(player_);
    SetListsWarpPtr(StageObjectCommonState::Hologram);
    SetListsLaserLaunchersPtr(StageObjectCommonState::Hologram);

    // ステージのサイズを計算
    CalculateCurrentStageRange();
    cameraAdjuster_.Update();

    // 背景描画に境界線の情報を渡す
    backDrawer_.SetActive(true);
    Range2D range = cameraAdjuster_.GetCameraRange();
    range.max.y = borderLine_->GetSprite().transform.translate.y - backDrawer_.GetHexagonSize() / 2.0f;
    backDrawer_.SetBorder(axisX, playerDirection, range);
}

/////////////////////////////////////////////////////////////////////////
//
// 境界線削除
//
/////////////////////////////////////////////////////////////////////////

void GameStage::RemoveBorderLine() {

    if (!isRemoveHologram_) {
        return;
    }

    // 境界線を非アクティブ状態にする
    borderLine_->SetDeactivate();
    // ホログラム側のレーザーをすべて破棄する
    laserController_->ResetLauncheres(StageObjectCommonState::Hologram);
    // ホログラム側のワープにアクセスできないようにする
    warpController_->ResetWarps(StageObjectCommonState::Hologram);

    // 非アクティブオブジェクトを再アクティブ化する
    ReActivateDisActiveObjects();
     
    // 作成したホログラムオブジェクトをすべて破棄する
    for (GameObject2D* object : hologramObjects_) {
        delete object;
        object = nullptr;
    }
    hologramObjects_.clear();

    // 削除完了
    isRemoveHologram_ = false;

    // ステージのサイズを計算
    CalculateCurrentStageRange();

    // 背景描画を非アクティブにする
    backDrawer_.SetActive(false);
}

void GameStage::RequestNextStage() {

    // クリア状態以外の時には受け付けない
    if (currentState_ != State::Clear) {
        return;
    }

    // 次のステージを構築する
    // インデックスを進める
    currentStageIndex_ = std::clamp(++currentStageIndex_, uint32_t(0), maxStageCount_);
}

void GameStage::CheckClear() {

    // プレイヤーから受け取ったクリア判定でクリア状態に遷移させる
    if (isClear_) {

        currentState_ = State::Clear;
    }
}

void GameStage::CheckPlayerDead() {

    // 死亡判定
    if (player_->IsDeadFinishTrigger()) {

        // レーザーで元の位置に戻す処理
        // レーザーの衝突位置を取得する
        //SetDeadLaserCollisions();

        //Vector2 target = onBlockPlayerRecordData_.back().translate;
        //// 逆イテレータで最新の座標でチェックする
        //bool isFound = false;
        //for (auto it = onBlockPlayerRecordData_.rbegin(); it != onBlockPlayerRecordData_.rend(); ++it) {
        //    // レーザーと衝突していたら他の座標でチェックする
        //    if (IsSafeRecordPoint(*it)) {

        //        // 補間先を設定
        //        target = it->translate;
        //        isFound = true;
        //        // 境界線を置いていなかった場合
        //        if (!it->isPutBordered && !player_->GetIsHologram()) {

        //            isRemoveHologram_ = true;
        //            break;
        //        }
        //    }
        //}
        //// 元の位置にワープして戻す
        //RecordData data = onBlockPlayerRecordData_.back();
        //warpController_->DeadWarp(player_->GetOwner()->GetWorldTranslate(), target);
        //deadMomentLaserCollisions_.clear();

        // レーザーで死んだらリスタート
        isRemoveHologram_ = true;
        requestInitialize_ = true;
    }
}

// プレイヤーが境界線を越えたかどうか
void GameStage::CheckPlayerCrossedBorderLine() {
    if (borderLine_->IsActive()) {

        // プレイヤーのワールド座標
        const Vector2 prePos = player_->GetOwner()->GetPrePos();
        const Vector2 playerWorldTranslate = player_->GetOwner()->GetWorldTranslate();

        // 境界線のX座標の計算
        float axisX = GameStageHelper::ComputeBorderAxisXFromContact(objects_,
            player_->GetSprite(), playerWorldTranslate, player_->GetMoveDirection(), stageObjectMapTileSize_);
        axisX = GameStageHelper::BorderAxisXFromPlayerDirection(axisX, player_->GetMoveDirection(), stageObjectMapTileSize_);

        //向きごとの境界線を越えたかどうかの判定をする
        if (borderLine_->GetSprite().transform.translate.y > playerWorldTranslate.y) {
            if (borderLine_->GetDirection() == LR::LEFT) {
                /*--- LEFT ---*/
                // プレイヤーが境界線を左から右に越えたか
                if (playerWorldTranslate.x < borderLine_->GetSprite().transform.translate.x &&
                    prePos.x > borderLine_->GetSprite().transform.translate.x) {

                    player_->SetIsHologram(true);
                }
                if (playerWorldTranslate.x > borderLine_->GetSprite().transform.translate.x &&
                    prePos.x < borderLine_->GetSprite().transform.translate.x) {

                    player_->SetIsHologram(false);
                }
            } else {
                /*--- RIGHT ---*/
                // プレイヤーが境界線を右から左に越えたか
                if (playerWorldTranslate.x > borderLine_->GetSprite().transform.translate.x &&
                    prePos.x < borderLine_->GetSprite().transform.translate.x) {

                    player_->SetIsHologram(true);
                }
                if (playerWorldTranslate.x < borderLine_->GetSprite().transform.translate.x &&
                    prePos.x > borderLine_->GetSprite().transform.translate.x) {

                    player_->SetIsHologram(false);
                }
            }
        }
    } else {
        // 境界線が置かれていないときはホログラム状態を解除する
        player_->SetIsHologram(false);
    }
}

void GameStage::CheckPlayerOutOfCamera() {
    // プレイヤーがカメラ範囲から出たかどうか
    if (currentStageRange_ != std::nullopt && player_ != nullptr) {
        // カメラの範囲
        const Range2D cameraRange = cameraAdjuster_.GetCameraRange();
        // プレイヤーがカメラ範囲から出たか
        if (player_->IsOutOfCamera(cameraRange)) {

            // 元の位置にワープして戻す
            RecordData data = onBlockPlayerRecordData_.back();
            warpController_->DeadWarp(player_->GetOwner()->GetWorldTranslate(), data.translate);
        }
    }
}


// ステージの範囲を計算する
void GameStage::CalculateCurrentStageRange() {

    // リセット
    currentStageRange_ = std::nullopt;

    for (GameObject2D* object : std::views::join(std::array{ objects_,hologramObjects_ })) {
        Vector2 pos = object->GetWorldTranslate();

        // 範囲の初期化
        if (currentStageRange_ == std::nullopt) {
            currentStageRange_ = Range2D(pos, pos);
        }

        // x,yそれぞれの最小値・最大値を更新
        if (pos.x < currentStageRange_.value().min.x) {
            currentStageRange_.value().min.x = pos.x;

        } else if (currentStageRange_.value().max.x < pos.x) {
            currentStageRange_.value().max.x = pos.x;
        }

        if (pos.y < currentStageRange_.value().min.y) {
            currentStageRange_.value().min.y = pos.y;

        } else if (currentStageRange_.value().max.y < pos.y) {
            currentStageRange_.value().max.y = pos.y;
        }
    }

    // ステージ範囲に少し余裕を持たせる
    float margin = stageObjectMapTileSize_;
    if (currentStageRange_ == std::nullopt) {
        currentStageRange_ = Range2D({ 0.0f,0.0f }, { 0.0f,0.0f });
    }
    currentStageRange_.value().min.x -= margin;
    currentStageRange_.value().min.y -= margin;
    currentStageRange_.value().max.x += margin;
    currentStageRange_.value().max.y += margin;

    // カメラ調整に範囲を渡す
    cameraAdjuster_.SetStageRange(currentStageRange_.value());
}

// プレイヤーに近づく
void GameStage::CloseToPlayer(LR direction, float zoomRate, const Vector2& focus) {

    currentStageRange_ = std::nullopt;
    if (direction == LR::LEFT) {
        // playerを左に写すように
        Vector2 min = player_->GetOwner()->GetWorldTranslate() - Vector2(playerSize_ * focus.x, playerSize_ * focus.y);
        Vector2 max = min + Vector2(playerSize_ * zoomRate, stageObjectMapTileSize_);
        currentStageRange_ = Range2D(min, max);

    } else if (direction == LR::RIGHT) {
        // playerを右に写すように
        Vector2 max = player_->GetOwner()->GetWorldTranslate() + Vector2(playerSize_ * focus.x, playerSize_ * focus.y);
        Vector2 min = max - Vector2(playerSize_ * zoomRate, stageObjectMapTileSize_);

    } else {
        // playerを中心に写すように
        Vector2 min = player_->GetOwner()->GetWorldTranslate() - Vector2(playerSize_ * 1.1f, stageObjectMapTileSize_ * 0.5f);
        Vector2 max = min + Vector2(playerSize_ * zoomRate, stageObjectMapTileSize_);
    }

    // カメラ調整に範囲を渡す
    cameraAdjuster_.SetStageRange(currentStageRange_.value());
}

bool GameStage::IsSafeRecordPoint(const RecordData& data) const {

    // 衝突判定を取得して衝突していれば別のデータでチェックする
    AABB2D playerAABB = playerCollision_.GetAABB();
    playerAABB.center = data.translate;
    for (const auto& laserCollision : deadMomentLaserCollisions_) {

        // 衝突した場合別のデータ
        if (Collision::AABB2D::AABB2D(playerAABB, laserCollision.GetAABB())) {
            return false;
        }
    }
    return true;
}

void GameStage::SetDeadLaserCollisions() {

    // プレイヤーがレーザーの衝突で死んだときステージ内のレーザー判定をすべて取得する
    if (deadMomentLaserCollisions_.empty()) {
        for (const auto& object : std::views::join(std::array{ objects_,hologramObjects_ })) {
            if (StageObjectComponent* stage = object->GetComponent<StageObjectComponent>()) {
                if (LaserLauncher* laserLauncher = stage->GetStageObject<LaserLauncher>()) {

                    // 発射台からすべてのレーザーを取得する
                    for (const auto& laser : laserLauncher->GetLasers()) {
                        if (Collision2DComponent* collision = laser->GetComponent<Collision2DComponent>()) {

                            // 全ての衝突を取得
                            for (const auto& collider : collision->GetColliders()) {
                                if (Collider_AABB2D* aabb = static_cast<Collider_AABB2D*>(collider.get())) {

                                    deadMomentLaserCollisions_.emplace_back(*aabb);
                                }
                            }
                        }
                    }
                }
            }
        }
        if (Collision2DComponent* component = player_->GetOwner()->GetComponent<Collision2DComponent>()) {

            // プレイヤーの衝突を取得
            for (const auto& collider : component->GetColliders()) {
                if (Collider_AABB2D* aabb = static_cast<Collider_AABB2D*>(collider.get())) {

                    playerCollision_ = *aabb;
                }
            }
        }
    }
}

// 取り除くUIの更新
void GameStage::UpdateRemoveUI() {

    // 入力デバイスが変わったら画像を切り替える
    if (Input::IsChangedInputDevice()) {
        if (Input::GetRecentInputDevice() == InputDevice::GAMEPAD) {
            removeUI_.clipLT.y = 129.0f * 3.0f;
        } else {
            removeUI_.clipLT.y = 129.0f * 1.0f;
        }
    }

    if (removeUITimer_.GetPrevProgress() == 0.0f) {
        return;
    }

    float ease = removeUITimer_.GetEase(Easing::OutBack);

    // 座標。色の更新
    removeUI_.transform.translate = player_->GetOwner()->GetWorldTranslate() - Vector2(0.0f, playerSize_ * 1.5f) * ease;
    removeUI_.transform.translate.x -= removeUI_.size.x * 0.25f * removeUI_.transform.scale.x;
    removeUI_.color.w = removeUITimer_.GetProgress();

    // テキストボックスの更新
    float offset = removeUI_.size.x * removeUI_.transform.scale.x;
    removeUI_TextBox.transform.translate = removeUI_.transform.translate + Vector2(offset, 0.0f);
    removeUI_TextBox.color.w = removeUI_.color.w;

    // 揺れの更新
    //ShakeRemoveUI();
}

//void GameStage::ShakeRemoveUI() {
//
//    if (borderLine_->IsShaking() == false) return;
//
//    if (removeUITimer_.GetPrevProgress() == 0.0f) {
//        return;
//    }
//
//     shakeStartPosX_ = player_->GetOwner()->GetWorldTranslate().x - removeUI_.size.x * 0.25f * removeUI_.transform.scale.x;
//     float offset = removeUI_.size.x * removeUI_.transform.scale.x;
//
//    if (!removeUIShakeTimer_.IsFinished()) {
//        removeUIShakeTimer_.Update();
//
//        float decay = 1.0f - removeUIShakeTimer_.GetProgress();
//
//        float wave = std::sin(removeUIShakeTimer_.GetProgress() * 100.0f);
//
//        removeUI_.transform.translate.x = shakeStartPosX_ + wave * shakeAmount_ * decay;
//       
//        removeUI_TextBox.transform.translate.x = removeUI_.transform.translate.x + offset;
//    } else {
//        // 揺れ終了
//        removeUI_.transform.translate.x = shakeStartPosX_;
//        removeUI_TextBox.transform.translate.x = removeUI_.transform.translate.x + offset;
//
//        removeUIShakeTimer_.Reset();
//    }
//
//}

/////////////////////////////////////////////////////////////////////////
//
// 描画
//
/////////////////////////////////////////////////////////////////////////

void GameStage::Draw() {

    // クリアになったら表示しない
    if (currentState_ != State::Clear) {

        // 境界線の描画
        borderLine_->Draw();

        // 取り除くUIの更新・描画
        removeUI_.Draw();
        removeUI_TextBox.Draw();
    }

    // 背景描画
    backDrawer_.Draw();
}

/////////////////////////////////////////////////////////////////////////
//
// ImGui編集
//
/////////////////////////////////////////////////////////////////////////

void GameStage::Edit() {
#ifdef _DEBUG

    ImFunc::CustomBegin("GameStage", MoveOnly_TitleBar);
    {
        ImGui::PushItemWidth(192.0f);
        if (ImGui::Button("Save Json")) {

            SaveJson();
        }

        if (ImGui::BeginTabBar("GameStageTab")) {
            if (ImGui::BeginTabItem("Stage")) {

                if (ImGui::Button("ReBuildStage")) {

                    for (GameObject2D* object : objects_) {
                        delete object;
                    }
                    objects_.clear();
                    for (GameObject2D* object : hologramObjects_) {
                        delete object;
                    }
                    hologramObjects_.clear();
                    // 再構築
                    BuildStage();
                }

                ImGui::Text("currentStage / max: %d/%d", currentStageIndex_, maxStageCount_);
                ImGui::Text("currentState: %s", EnumAdapter<State>::ToString(currentState_));

                ImGui::DragFloat("stageObjectMapTileSize", &stageObjectMapTileSize_, 0.5f);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("BorderLine")) {

                borderLine_->Edit(player_->GetOwner()->GetWorldTranslate(), stageObjectMapTileSize_);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Warp")) {

                warpController_->Edit();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Laser")) {

                laserController_->Edit();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Camera")) {
                float cameraFov = SEED::GetMainCamera()->GetFov();
                Vector2 clipRange = SEED::GetMainCamera()->GetClipRange();
                ImGui::DragFloat("CameraFov", &cameraFov, 0.1f, 1.0f, 179.0f);
                ImGui::DragFloat2("CameraClipRange", &clipRange.x, 0.1f, 0.1f, 10000.0f);
                SEED::GetMainCamera()->SetFov(cameraFov);
                SEED::GetMainCamera()->SetClipRange(clipRange);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Record")) {

                const Vector2 playerWorldTranslate = player_->GetOwner()->GetWorldTranslate();
                ImGui::Text("playerPos: %.1f,%.1f", playerWorldTranslate.x, playerWorldTranslate.y);
                ImGui::DragInt("maxRecord", &maxRecordCount_, 1, 1, 16);
                uint32_t index = 0;
                for (const auto& data : onBlockPlayerRecordData_) {

                    ImGui::SeparatorText(("records: " + std::to_string(index)).c_str());
                    std::string key = "translate: %.1f,%.1f";
                    ImGui::Text(key.c_str(), data.translate.x, data.translate.y);
                    key = std::format("isPutBordered: {}", data.isPutBordered);
                    ImGui::Text(key.c_str(), data.translate.x, data.translate.y);
                    ++index;
                }
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("blockAppearance")) {

                ImGui::DragFloat("blockAppearanceBaseDuration", &blockAppearanceBaseDuration_, 0.01f);
                ImGui::DragFloat("blockAppearanceSpacing", &blockAppearanceSpacing_, 0.01f);
                EnumAdapter<Easing::Type>::Combo("blockAppearanceEasing", &blockAppearanceEasing_);
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        ImGui::PopItemWidth();
        ImGui::End();
    }
#endif // _DEBUG
}

//////////////////////////////////////////////////////////////////////////
//
// Json保存・読み込み
//
//////////////////////////////////////////////////////////////////////////

void GameStage::ApplyJson() {

    nlohmann::json data;
    if (!JsonAdapter::LoadCheck(kJsonPath_, data)) {
        return;
    }

    stageObjectMapTileSize_ = data.value("stageObjectMapTileSize_", 32.0f);
    blockAppearanceBaseDuration_ = data.value("blockAppearanceBaseDuration_", 0.26f);
    blockAppearanceSpacing_ = data.value("blockAppearanceSpacing_", 0.1f);
    //blockAppearanceEasing_ = EnumAdapter<Easing::Type>::FromString(data["blockAppearanceEasing_"]).value();
    playerSize_ = stageObjectMapTileSize_ * 0.8f;
    borderLine_->FromJson(data["BorderLine"]);
    warpController_->FromJson(data.value("WarpController", nlohmann::json()));
}

void GameStage::SaveJson() {

    nlohmann::json data;

    data["stageObjectMapTileSize_"] = stageObjectMapTileSize_;
    data["blockAppearanceBaseDuration_"] = blockAppearanceBaseDuration_;
    data["blockAppearanceSpacing_"] = blockAppearanceSpacing_;
    data["blockAppearanceEasing_"] = EnumAdapter<Easing::Type>::ToString(blockAppearanceEasing_);
    borderLine_->ToJson(data["BorderLine"]);
    warpController_->ToJson(data["WarpController"]);

    JsonAdapter::Save(kJsonPath_, data);
}
