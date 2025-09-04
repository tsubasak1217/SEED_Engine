#include "ClearState_MenuSelect.h"
#include <SEED/Source/SEED.h>
#include <Game/Scene/Scene_Clear/Scene_Clear.h>
#include <Game/Scene/Scene_Game/Scene_Game.h>
#include <Game/Scene/Scene_Game/State/GameState_Play.h>
#include <Game/Scene/Input/Device/MenuBarGamePadInput.h>
#include <Game/Scene/Input/Device/MenuBarKeyInput.h>
#include <Game/Objects/Stage/GameStage.h>

//////////////////////////////////////////////////////////////////////////////
//
// コンストラクタ・デストラクタ
//
//////////////////////////////////////////////////////////////////////////////

ClearState_MenuSelect::ClearState_MenuSelect(Scene_Base* pScene) {
    pScene_ = pScene;
    Initialize();
}

ClearState_MenuSelect::~ClearState_MenuSelect() {

}

//////////////////////////////////////////////////////////////////////////////
//
// 初期化
//
//////////////////////////////////////////////////////////////////////////////
void ClearState_MenuSelect::Initialize() {
    // 入力クラスを初期化
    inputMapper_ = std::make_unique<InputMapper<PauseMenuInputAction>>();
    inputMapper_->AddDevice(std::make_unique<MenuBarKeyInput>());     // キー操作
    inputMapper_->AddDevice(std::make_unique<MenuBarGamePadInput>()); // パッド操作
    for (int i = 0; i < 2; i++) {
        menuPos_[i] = { 640.0f,300.0f + i * 100.0f };
    }
    menuSize_ = { 400.0f,50.0f };
    MenuText_[0] = TextBox2D("ステージ" + std::to_string(pScene_->currentStageIndex_ + 1) + "へ");
    MenuText_[1] = TextBox2D("ステージ選択へ戻る");
    for (int i = 0; i < 2; i++) {
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
    ClearText_ = TextBox2D("Stage Clear!");
    ClearText_.transform.translate = { 640.0f,150.0f };
    ClearText_.SetFont("");
    ClearText_.size = { 400.0f,100.0f };
    ClearText_.color = { 1.0f,1.0f,0.0f,1.0f };
    ClearText_.fontSize = 48;
    ClearText_.textBoxVisible = false;
}

//////////////////////////////////////////////////////////////////////////////
//
// 終了処理
//
//////////////////////////////////////////////////////////////////////////////
void ClearState_MenuSelect::Finalize() {}

//////////////////////////////////////////////////////////////////////////////
//
// 更新
//
//////////////////////////////////////////////////////////////////////////////
void ClearState_MenuSelect::Update() {

    // メニューの選択
    //上移動
    if (inputMapper_->GetVector(PauseMenuInputAction::MoveY) < 0.0f) {
        currentMenu_--;
        if (currentMenu_ < 0) {
            currentMenu_ = 1;
        }
        AudioManager::PlayAudio("SE/turnoverPaper.mp3", false, 1.0f, 0.0f);
    }
    //下移動
    if (inputMapper_->GetVector(PauseMenuInputAction::MoveY) > 0.0f) {
        currentMenu_++;
        if (currentMenu_ > 1) {
            currentMenu_ = 0;
        }
        AudioManager::PlayAudio("SE/turnoverPaper.mp3", false, 1.0f, 0.0f);
    }


    if (pScene_->currentStageIndex_ == pScene_->maxStageCount_ - 1) {
        //現在のステージが最大ステージ数なら次のステージに進むメニューを選択できないようにする
        if (currentMenu_ == 0) {
            currentMenu_ = 1;
        }
    }

    // 選択中のメニューの色を変える
    for (size_t i = 0; i < 2; i++) {
        if (i == currentMenu_) {
            MenuBack_[i].color = { 1.0f,0.5f,0.5f,1.0f };
        } else {
            MenuBack_[i].color = { 1.0f,1.0f,1.0f,1.0f };
        }
    }

}

//////////////////////////////////////////////////////////////////////////////
//
// 描画
//
//////////////////////////////////////////////////////////////////////////////
void ClearState_MenuSelect::Draw() {


    if (pScene_->currentStageIndex_ != pScene_->maxStageCount_ - 1) {
        //現在のステージが最大ステージ数でないなら次のステージに進むメニューを表示
        MenuBack_[0].Draw();
        MenuText_[0].Draw();
    }

    //セレクト画面に戻るメニューは常に表示
    MenuBack_[1].Draw();
    MenuText_[1].Draw();

    //クリアテキストの描画
    ClearText_.Draw();
}

//////////////////////////////////////////////////////////////////////////////
//
// フレーム開始処理
//
//////////////////////////////////////////////////////////////////////////////
void ClearState_MenuSelect::BeginFrame() {}

//////////////////////////////////////////////////////////////////////////////
//
// フレーム終了処理
//
//////////////////////////////////////////////////////////////////////////////
void ClearState_MenuSelect::EndFrame() {}

//////////////////////////////////////////////////////////////////////////////
//
// コライダーの引き継ぎ
//
//////////////////////////////////////////////////////////////////////////////
void ClearState_MenuSelect::HandOverColliders() {}

//////////////////////////////////////////////////////////////////////////////
//
// 状態管理
//
//////////////////////////////////////////////////////////////////////////////
void ClearState_MenuSelect::ManageState() {


    // 決定
    if (inputMapper_->IsTriggered(PauseMenuInputAction::Enter)) {
        AudioManager::PlayAudio("SE/iceSound.mp3", false, 0.3f, 1.0f);
        changeStateRequest_ = true;
    }
    // 状態遷移要求があったら状態を変更する
    if (changeStateRequest_) {
        changeStateRequest_ = false;
        switch (currentMenu_) {
        case 0: // next Stage...次のステージに進む

            pScene_->ChangeScene("Game");
            break;
        case 1: // Back in the SelectMenu...セレクト画面に戻る
            pScene_->ChangeScene("Select");
            AudioManager::EndAllAudio();
            break;
        default:
            break;
        }
    }
}
