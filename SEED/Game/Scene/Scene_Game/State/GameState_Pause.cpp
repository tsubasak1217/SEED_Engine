#include "GameState_Pause.h"
#include <SEED/Source/SEED.h>
#include <SEED/Source/Manager/AudioManager/AudioManager.h>
#include <SEED/Source/Basic/Scene/Scene_Base.h>
#include <Game/Scene/Scene_Game/Scene_Game.h>
#include <Game/Scene/Scene_Game/State/GameState_Play.h>
#include <Game/Scene/Input/Device/MenuBarGamePadInput.h>
#include <Game/Scene/Input/Device/MenuBarKeyInput.h>

//////////////////////////////////////////////////////////////////////////////////
//
// コンストラクタ・デストラクタ
// 
//////////////////////////////////////////////////////////////////////////////////
GameState_Pause::GameState_Pause(Scene_Base* pScene) {
    // シーンの設定
    pScene_ = pScene;
    Initialize();
}

GameState_Pause::~GameState_Pause() {
    // メインカメラをデフォルトに戻す
    SEED::SetMainCamera("default");
}

//////////////////////////////////////////////////////////////////////////////////
//
// 初期化
//
//////////////////////////////////////////////////////////////////////////////////
void GameState_Pause::Initialize() {

    // 入力クラスを初期化
    inputMapper_ = std::make_unique<InputMapper<PauseMenuInputAction>>();
    inputMapper_->AddDevice(std::make_unique<MenuBarKeyInput>());     // キー操作
    inputMapper_->AddDevice(std::make_unique<MenuBarGamePadInput>()); // パッド操作

    // menuItwm
    pauseItems_.clear();
    pauseItems_.resize(3);
    nlohmann::json data = MyFunc::GetJson("Resources/Jsons/Pause/PauseMenu.json");
    for (int i = 0; i < kMenuCount; i++) {

        // 基本初期化
        pauseItems_[i].backSprite = Sprite("UI/menuItem.png");
        pauseItems_[i].backSprite.size = { 200.0f, 50.0f };
        pauseItems_[i].backSprite.layer = 20;
        pauseItems_[i].backSprite.anchorPoint = { 0.5f, 0.5f };
        pauseItems_[i].backSprite.isApplyViewMat = false;

        // テキスト
        pauseItems_[i].text = TextBox2D("text");
        pauseItems_[i].text.layer = 21;
        pauseItems_[i].text.isApplyViewMat = false;

        if (!data.empty()) {
            if (data.size() > i) {
                pauseItems_[i].FromJson(data[i]);
            }
        }
    }

    // 色の初期化
    hexagonColors_.push_back(MyMath::FloatColor(255, 118, 11, 255, false));
    hexagonColors_.push_back(MyMath::FloatColor(10, 10, 10, 255, false));
    hexagonColors_.push_back(MyMath::FloatColor(255, 13, 86, 255, false));
    hexagonSize_ = 46.0f;

    // Spriteなどを一度更新しておく(ちらつき防止)
    Update();
}


//////////////////////////////////////////////////////////////////////////////////
//
// 終了処理
//
//////////////////////////////////////////////////////////////////////////////////
void GameState_Pause::Finalize() {
}

//////////////////////////////////////////////////////////////////////////////////
//
// 更新
//
//////////////////////////////////////////////////////////////////////////////////
void GameState_Pause::Update() {

    // メニューの選択
    // 
    if (!isExitScene_) {
        //上移動
        const float kSEVolume = 0.3f;
        if (inputMapper_->GetVector(PauseMenuInputAction::MoveY) < 0.0f) {

            currentMenu_ = MyFunc::Spiral(currentMenu_ - 1, 0, kMenuCount - 1);
            AudioManager::PlayAudio(AudioDictionary::Get("ポーズ_選択"), false, kSEVolume);
        }
        //下移動
        if (inputMapper_->GetVector(PauseMenuInputAction::MoveY) > 0.0f) {

            currentMenu_ = MyFunc::Spiral(currentMenu_ + 1, 0, kMenuCount - 1);
            AudioManager::PlayAudio(AudioDictionary::Get("ポーズ_選択"), false, kSEVolume);
        }
    }

    // メニュータイマーの更新
    float t = menuTimer_.GetProgress();
    float ease;
    if (isExit_) {
        menuTimer_.Update(-1.0f);
        ease = EaseInQuint(t);
    } else {
        menuTimer_.Update(1.0f);
        ease = EaseOutQuint(t);
    }

    // 選択中のメニューの色を変える
    for (size_t i = 0; i < 3; i++) {
        // 時間の更新
        pauseItems_[i].selectTime.Update(i == currentMenu_ ? 1.0f : -1.0f);

        // パラメーターの更新
        pauseItems_[i].backSprite.transform.translate = MyMath::Lerp(pauseItems_[i].start.translate, pauseItems_[i].end.translate, ease);
        pauseItems_[i].text.transform.translate = MyMath::Lerp(pauseItems_[i].start.translate, pauseItems_[i].end.translate, ease);
        pauseItems_[i].backSprite.size = pauseItems_[i].backSpriteSize;

        float t2 = pauseItems_[i].selectTime.GetProgress();
        float ease2 = EaseOutBack(t2);
        pauseItems_[i].backSprite.transform.scale = 1.0f + 0.2f * ease2;
        pauseItems_[i].text.transform.scale = pauseItems_[i].backSprite.transform.scale;
        pauseItems_[i].backSprite.color = pauseItems_[i].backColor;
        pauseItems_[i].backSprite.color.w = 0.4f + 0.6f * t2;
        pauseItems_[i].text.color.w = pauseItems_[i].backSprite.color.w;
    }

    // 終了遷移のタイマー更新
    if (isExitScene_) {
        sceneChangeTimer_.Update();
    }

#ifdef _DEBUG
    ImFunc::CustomBegin("Pause", MoveOnly_TitleBar);
    {

        std::string label;

        for (int i = 0; i < 3; i++) {
            label = "Menu" + std::to_string(i);
            if (ImGui::CollapsingHeader(label.c_str())) {
                pauseItems_[i].Edit();
            }
        }

        ImGui::Separator();

        // jsonに保存
        if (ImGui::Button("Save Json")) {
            MenuItemsToJson();
        }

        ImGui::End();
    }
#endif // _DEBUG


}

//////////////////////////////////////////////////////////////////////////////////
//
// 描画
//
//////////////////////////////////////////////////////////////////////////////////
void GameState_Pause::Draw() {
    for (int i = 0; i < 3; i++) {
        pauseItems_[i].backSprite.Draw();
        pauseItems_[i].text.Draw();
    }
}

//////////////////////////////////////////////////////////////////////////////////
//
// フレーム開始処理
//
//////////////////////////////////////////////////////////////////////////////////
void GameState_Pause::BeginFrame() {
}

//////////////////////////////////////////////////////////////////////////////////
//
// フレーム終了処理
//
//////////////////////////////////////////////////////////////////////////////////
void GameState_Pause::EndFrame() {

    // 歩き音声が再生中なら止める
    if (AudioManager::IsPlayingAudio(AudioDictionary::Get("プレイヤー_足音"))) {

        AudioManager::EndAudio(AudioManager::GetAudioHandle(AudioDictionary::Get("プレイヤー_足音")));
    }
}

//////////////////////////////////////////////////////////////////////////////////
//
// コリジョン情報の受け渡し
//
//////////////////////////////////////////////////////////////////////////////////
void GameState_Pause::HandOverColliders() {
}

//////////////////////////////////////////////////////////////////////////////////
//
// ステートの管理
//
//////////////////////////////////////////////////////////////////////////////////
void GameState_Pause::ManageState() {

    Scene_Game* gameScene = dynamic_cast<Scene_Game*>(pScene_);
    GameStage* stage_ = gameScene->GetStage();

    if (isResetStage_) {

        // 次のシーンに進むまでのタイマーを進める
        nextStageTimer_.Update();
        if (nextStageTimer_.IsFinished()) {

            // ホログラムのBGMが流れていたら止める
            if (AudioManager::IsPlayingAudio(holoBGMHandle_)) {

                // 音声をリセットする
                AudioManager::EndAudio(holoBGMHandle_);
                noneBGMHandle_ = AudioManager::PlayAudio(AudioDictionary::Get("ゲームシーン_通常BGM"), true, kBGMVolume_);
            }
            gameScene->ChangeScene("Game");
        }
        return;
    }

    //State_Playに戻る
    if (inputMapper_->IsTriggered(PauseMenuInputAction::Pause)) {

        // カメラ範囲を元に戻す
        isExit_ = true;
        stage_->CalculateCurrentStageRange();

        // SE
        const float kSEVolume = 0.5f;
        AudioManager::PlayAudio(AudioDictionary::Get("ポーズ_ゲームに戻る"), false, kSEVolume);
        stage_->CalculateCurrentStageRange();// カメラ範囲を元に戻す
        // 画面が切り替わったらポーズ解除
        stage_->SetIsPaused(false);
        return;
    }

    if (isExit_ && menuTimer_.GetPrevProgress() <= 0.0f) {
        pScene_->ChangeState(new GameState_Play(pScene_));
        isExit_ = false;
        return;
    }

    // 決定
    if (isExitScene_ == false) {
        if (inputMapper_->IsTriggered(PauseMenuInputAction::Enter)) {

            switch (currentMenu_) {
            case 0:
            {

                // 続ける
               // SE
                const float kSEVolume = 0.5f;
                AudioManager::PlayAudio(AudioDictionary::Get("ポーズ_ゲームに戻る"), false, kSEVolume);
                isExit_ = true;
                stage_->CalculateCurrentStageRange();// カメラ範囲を元に戻す
                stage_->SetIsPaused(false);
                break;
            }
            case 1:
            {
                // やり直す
                // SE
                const float kSEVolume = 0.5f;
                AudioManager::PlayAudio(AudioDictionary::Get("ポーズ_リトライ"), false, kSEVolume);

                // 遷移処理を開始する
                isResetStage_ = true;
                NextStageTransition* transition = SceneTransitionDrawer::AddTransition<NextStageTransition>();
                transition->SetParam(stripHeight_, appearEndTimeT_, color_);
                transition->StartTransition(nextStageTimer_.GetDuration(), nextStageTime_);
                return;
            }
            case 2:// セレクトへ戻る
            {
                // シーン遷移を生成して開始
                isExitScene_ = true;
                HexagonTransition* transition = SceneTransitionDrawer::AddTransition<HexagonTransition>();
                transition->SetHexagonInfo(hexagonSize_, hexagonColors_);
                transition->StartTransition(sceneChangeTimer_.GetDuration(), nextSceneEnterTime_);

                // SE
                const float kSEVolume = 0.5f;
                AudioManager::PlayAudio(AudioDictionary::Get("ポーズ_セレクトへ"), false, kSEVolume);

                break;
            }
            default:
                break;
            }
        }
    }

    // セレクトシーンに遷移

    if (sceneChangeTimer_.IsFinished()) {
        pScene_->ChangeScene("Select");

        // 音声をリセットする
        AudioManager::EndAudio(noneBGMHandle_);
        AudioManager::EndAudio(holoBGMHandle_);
        return;
    }

}


/////////////////////////////////////////////////////////////////////
// json
/////////////////////////////////////////////////////////////////////

nlohmann::json PauseItem::ToJson() const {
    nlohmann::json data;
    data["start"] = start;
    data["end"] = end;
    data["backColor"] = backColor;
    data["backSpriteSize"] = backSpriteSize;
    data["text"] = text.GetJsonData();
    return data;
}

void PauseItem::FromJson(const nlohmann::json& data) {
    start = data.value("start", Transform2D());
    end = data.value("end", Transform2D());
    backColor = data.value("backColor", Vector4(1.0f));
    backSpriteSize = data.value("backSpriteSize", Vector2(200.0f, 50.0f));
    text.LoadFromJson(data["text"]);
}

void GameState_Pause::MenuItemsToJson() {
    nlohmann::ordered_json data = nlohmann::ordered_json::array();
    for (const PauseItem& item : pauseItems_) {
        data.push_back(item.ToJson());
    }

    // JSONファイルに保存
    MyFunc::CreateJsonFile("Resources/Jsons/Pause/PauseMenu.json", data);
}

/////////////////////////////////////////////////////////////////////
// ImGuiで編集
/////////////////////////////////////////////////////////////////////
void PauseItem::Edit() {
#ifdef _DEBUG
    ImGuiManager::RegisterGuizmoItem(&start);
    ImGuiManager::RegisterGuizmoItem(&end);
    ImGui::DragFloat2("BackSpriteSize", &backSpriteSize.x, 1.0f, 1.0f, 1000.0f);
    ImGui::ColorEdit4("BackColor", &backColor.x);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    text.Edit();
#endif // _DEBUG
}