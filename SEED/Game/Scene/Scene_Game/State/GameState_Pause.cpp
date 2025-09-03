#include "GameState_Pause.h"
#include <SEED/Source/SEED.h>
#include <SEED/Source/Basic/Scene/Scene_Base.h>
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

    for(int i = 0; i < 3; i++){
        menuPos_[i] = { 640.0f,200.0f + i * 100.0f };
    }
    menuSize_ = { 400.0f,50.0f };

    MenuText_[0] = TextBox2D("Return to the Game");
    MenuText_[1] = TextBox2D("Restart");
    MenuText_[2] = TextBox2D("Back in the Title");
    
    for (int i = 0; i < 3; i++) {
        MenuBack_[i] = Sprite("DefaultAssets/white1x1.png");
        MenuBack_[i].size = menuSize_;
        MenuBack_[i].color = { 1.0f,1.0f,1.0f,1.0f };
        MenuBack_[i].leftTop = menuPos_[i];
        MenuBack_[i].anchorPoint = { 0.5f,0.5f };
        MenuBack_[i].drawLocation = DrawLocation::Front;
        MenuBack_[i].layer = 1;

        MenuText_[i].transform.translate = menuPos_[i];
        MenuText_[i].SetFont("");
        MenuText_[i].size = menuSize_;
        MenuText_[i].color = { 0.0f,0.0f,0.0f,1.0f };
        MenuText_[i].fontSize = 32;
        MenuText_[i].textBoxVisible = false; // テキストボックスの枠を非表示
    }
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
    //上移動
    if(inputMapper_->GetVector(PauseMenuInputAction::MoveY) < 0.0f) {
        currentMenu_--;
        if (currentMenu_ < 0) {
            currentMenu_ = 2;
        }
        AudioManager::PlayAudio("SE/turnoverPaper.mp3", false, 0.3f, 1.0f);
    }
    //下移動
    if (inputMapper_->GetVector(PauseMenuInputAction::MoveY) > 0.0f) {
         currentMenu_++;
         if (currentMenu_ > 2) {
             currentMenu_ = 0;
         }
         AudioManager::PlayAudio("SE/turnoverPaper.mp3", false, 0.3f, 1.0f);
    }

    // 選択中のメニューの色を変える
    for (size_t i = 0; i < 3; i++) {
        if (i == currentMenu_) {
            MenuBack_[i].color = { 1.0f,0.5f,0.5f,1.0f };
        }
        else {
            MenuBack_[i].color = { 1.0f,1.0f,1.0f,1.0f };
        }
    }

    
}

//////////////////////////////////////////////////////////////////////////////////
//
// 描画
//
//////////////////////////////////////////////////////////////////////////////////
void GameState_Pause::Draw() {
    for(int i = 0; i < 3; i++){
        MenuBack_[i].Draw();
        MenuText_[i].Draw();
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

    //State_Playに戻る
    if(inputMapper_->IsTriggered(PauseMenuInputAction::Pause)){
        changeStateRequest_ = true;
        return;
    }

    if (changeStateRequest_) {
        pScene_->ChangeState(new GameState_Play(pScene_));
        changeStateRequest_ = false;
        return;
    }

    // 決定
    if (inputMapper_->IsTriggered(PauseMenuInputAction::Enter)) {
        AudioManager::PlayAudio("SE/iceSound.mp3", false, 0.3f, 1.0f);
        switch (currentMenu_) {
        case 0:// 続ける
            changeStateRequest_ = true;
            break;
        case 1:// やり直す
            pScene_->ChangeScene("Game");
            break;
        case 2:// タイトルへ戻る
            pScene_->ChangeScene("Title");
            AudioManager::EndAllAudio();
            break;
        default:
            break;
        }
    }

   
}
