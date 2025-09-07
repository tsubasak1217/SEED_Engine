#include "GameStage.h"

//============================================================================
//	include
//============================================================================
#include <SEED/Lib/JsonAdapter/JsonAdapter.h>
#include <SEED/Lib/MagicEnumAdapter/EnumAdapter.h>
#include <SEED/Source/Manager/InputManager/InputManager.h>
#include <Environment/Environment.h>
#include <Game/GameSystem.h>
#include <Game/Components/StageObjectComponent.h>
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

    // リストからプレイヤーのポインタを渡す
    GetListsPlayerPtr();
    // リストから必要なポインタを渡す
    warpController_->SetPlayer(player_);
    SetListsWarpPtr(StageObjectCommonState::None);
    SetListsLaserLaunchersPtr(StageObjectCommonState::None);

    // 状態をプレイ中に遷移させる
    currentState_ = State::Play;
}

//Objectのアクティブ・非アクティブ設定
void GameStage::SetIsActive(bool isActive) {

    // アクティブを設定する
    for (GameObject2D* object : std::views::join(std::array{ objects_, hologramObjects_ })) {

        object->SetIsActive(isActive);

        if(!isActive){
            object->SetIsMustDraw(true);
        } else{
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

/////////////////////////////////////////////////////////////////////////
//
// 全体の更新処理
//
/////////////////////////////////////////////////////////////////////////
void GameStage::Update() {

    switch (currentState_) {
        //============================================================================
        //	ゲームプレイ中の更新処理
        //============================================================================
    case GameStage::State::Play:

        UpdatePlay();
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

void GameStage::UpdatePlay() {

    // ワープの更新処理
    UpdateWarp();
    // レーザーの更新処理
    UpdateLaserLauncher();

    // 境界線の更新処理(ホログラムオブジェクトの作成も行っている)
    UpdateBorderLine();

    // クリア判定
    CheckClear();
    // 死亡判定
    CheckPlayerDead();

    // カメラの調整
    cameraAdjuster_.Update();
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
    } else if (borderLine_->CanTransitionDisable(player_->GetSprite().translate,
        stageObjectMapTileSize_) && player_->IsRemoveBorder()) {

        // ワープ中は境界線を消せない
        if (!warpController_->IsWarping()) {

            // 境界線を非アクティブ状態にしてホログラムオブジェクトを全て破棄する
            isRemoveHologram_ = true;
        }
    }

    // 境界線のX座標を一番占有率の高いオブジェクトの端に設定する
    float axisX = GameStageHelper::ComputeBorderAxisXFromContact(objects_,
        player_->GetSprite(), playerWorldTranslate, player_->GetMoveDirection(), stageObjectMapTileSize_);
    Vector2 placePos = playerWorldTranslate;
    placePos.x = GameStageHelper::BorderAxisXFromPlayerDirection(axisX, player_->GetMoveDirection(), stageObjectMapTileSize_);

    // 境界線の更新処理
    borderLine_->Update(placePos, playerWorldTranslate.y + player_->GetSprite().size.y, stageObjectMapTileSize_);
}

void GameStage::UpdateClear() {

    // インデックスを進める
    currentStageIndex_ = std::clamp(++currentStageIndex_, uint32_t(0), maxStageCount_);
    BuildStage();
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
}

void GameStage::CheckClear() {

    // デバッグ用
    if (isClear_) {
        currentState_ = State::Clear;
    }
}

void GameStage::CheckPlayerDead() {
    // 死亡判定
    if (player_->IsDead()) {
        currentState_ = State::Dead;
    }
}

// ステージの範囲を計算する
void GameStage::CalculateCurrentStageRange(){

    // リセット
    currentStageRange_ = std::nullopt;

    for(GameObject2D* object : std::views::join(std::array{ objects_,hologramObjects_ })){
        Vector2 pos = object->GetWorldTranslate();

        // 範囲の初期化
        if(currentStageRange_ == std::nullopt){
            currentStageRange_ = Range2D(pos, pos);
        }

        // x,yそれぞれの最小値・最大値を更新
        if(pos.x < currentStageRange_.value().min.x){
            currentStageRange_.value().min.x = pos.x;
        
        } else if(currentStageRange_.value().max.x < pos.x){
            currentStageRange_.value().max.x = pos.x;
        }

        if(pos.y < currentStageRange_.value().min.y){
            currentStageRange_.value().min.y = pos.y;
        
        } else if(currentStageRange_.value().max.y < pos.y){
            currentStageRange_.value().max.y = pos.y;
        }
    }

    // ステージ範囲に少し余裕を持たせる
    float margin = stageObjectMapTileSize_;
    if(currentStageRange_ == std::nullopt){
        currentStageRange_ = Range2D({0.0f,0.0f},{0.0f,0.0f});
    }
    currentStageRange_.value().min.x -= margin;
    currentStageRange_.value().min.y -= margin;
    currentStageRange_.value().max.x += margin;
    currentStageRange_.value().max.y += margin;

    // カメラ調整に範囲を渡す
    cameraAdjuster_.SetStageRange(currentStageRange_.value());
}

/////////////////////////////////////////////////////////////////////////
//
// 描画
//
/////////////////////////////////////////////////////////////////////////

void GameStage::Draw() {

    // 境界線の描画
    borderLine_->Draw();
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
    playerSize_ = stageObjectMapTileSize_ * 0.8f;
    borderLine_->FromJson(data["BorderLine"]);
    warpController_->FromJson(data.value("WarpController", nlohmann::json()));
}

void GameStage::SaveJson() {

    nlohmann::json data;

    data["stageObjectMapTileSize_"] = stageObjectMapTileSize_;
    borderLine_->ToJson(data["BorderLine"]);
    warpController_->ToJson(data["WarpController"]);

    JsonAdapter::Save(kJsonPath_, data);
}
