#include "GameState_Pause.h"
#include <SEED/Source/Basic/Scene/Scene_Base.h>
#include <Game/Scene/Scene_Game/State/GameState_Play.h>
#include <SEED/Source/SEED.h>

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
    // 全ての音声を停止
    AudioManager::EndAllAudio();
}

//////////////////////////////////////////////////////////////////////////////////
//
// 初期化
//
//////////////////////////////////////////////////////////////////////////////////
void GameState_Pause::Initialize() {

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
    if(Input::IsTriggerStick(LR::LEFT,DIRECTION4::UP) || Input::IsTriggerPadButton(PAD_BUTTON::UP)) {
        currentMenu_--;
        if (currentMenu_ < 0) {
            currentMenu_ = 2;
        }
    }
    //下移動
    if (Input::IsTriggerStick(LR::LEFT,DIRECTION4::DOWN) || Input::IsTriggerPadButton(PAD_BUTTON::DOWN)) {
         currentMenu_++;
         if (currentMenu_ > 2) {
             currentMenu_ = 0;
         }
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
    // 決定
    if (Input::IsTriggerPadButton(PAD_BUTTON::A)) {
        switch (currentMenu_) {
        case 0:// 続ける
            pScene_->ChangeState(new GameState_Play(pScene_));
            break;
        case 1:// やり直す
            pScene_->ChangeScene("Game");
            break;
        case 2:// タイトルへ戻る
            pScene_->ChangeScene("Title");
            break;
        default:
            break;
        }
    }

    //State_Playに戻る
    if(Input::IsTriggerPadButton(PAD_BUTTON::START)){
        pScene_->ChangeState(new GameState_Play(pScene_));
        return;
    }
}
