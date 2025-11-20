#include "GameState_Pause.h"
#include <SEED/Source/Basic/Scene/Scene_Base.h>

///////////////////////////////////////////////////////////////////////////////
// コンストラクタ・デストラクタ
///////////////////////////////////////////////////////////////////////////////
GameState_Pause::GameState_Pause(){
}

GameState_Pause::GameState_Pause(Scene_Base* pScene){
    pScene_ = pScene;
}

GameState_Pause::~GameState_Pause(){
}

///////////////////////////////////////////////////////////////////////////////
// 初期化・終了処理
///////////////////////////////////////////////////////////////////////////////
void GameState_Pause::Initialize(){
    // ポーズUIの読み込み
    pauseItemsParent_ = pScene_->GetHierarchy()->LoadObject2D("Pause/PauseItems.prefab");

    // メニュー項目
    menus_[Resume] = pauseItemsParent_->GetChild("resume");
    menus_[Retry] = pauseItemsParent_->GetChild("retry");
    menus_[ToSelect] = pauseItemsParent_->GetChild("toSelect");

    for(int32_t i = 0; i < menus_.size(); i++){
        if(selectedIndex_ != i){
            menus_[i]->masterColor_ = Color(0.4f, 0.4f, 0.4f, 1.0f);
            menus_[i]->GetComponent<ColorControlComponent>()->SetIsActive(false);
            menus_[i]->GetChild("backScrollObject")->GetComponent<Routine2DComponent>()->RevercePlay();
        }
    }

    // ポーズ背景
    backSpriteObj_ = pauseItemsParent_->GetChild("backSprite");

    // タイマー
    pauseTimer_.Initialize(0.8f);
    for(int32_t i = 0; i < scalingTimers_.size(); i++){
        // 選択中の項目だけ処理を変える
        if(i != selectedIndex_){
            // 通常初期化
            scalingTimers_[i].Initialize(0.3f);
        } else{
            // デフォルトでスケーリングを完了させておく
            scalingTimers_[i].Initialize(0.3f, 0.3f);
        }
    }
}

void GameState_Pause::Finalize(){
}

///////////////////////////////////////////////////////////////////////////////
// 更新処理
///////////////////////////////////////////////////////////////////////////////
void GameState_Pause::Update(){

    // メニュー項目選択
    SelectMenuItem();

    // メニュー項目スケーリング用のタイマー更新
    for(int32_t i = 0; i < scalingTimers_.size(); i++){

        // 選択中かどうかで更新処理を変える
        if(i != selectedIndex_){
            scalingTimers_[i].Update(-1.0f);
        } else{
            scalingTimers_[i].Update(1.0f);
        }

        // スケーリング反映
        float scale = 0.9f + 0.1f * scalingTimers_[i].GetEase(Easing::OutBack);
        menus_[i]->aditionalTransform_.scale = Vector2(scale, scale);
    }

    // ポーズタイマーの更新
    if(isExit_){
        // ポーズ解除中
        pauseTimer_.Update(-1.0f);
    } else{
        // ポーズ中
        pauseTimer_.Update(1.0f);
    }
}


///////////////////////////////////////////////////////////////////////////////
// 描画処理
///////////////////////////////////////////////////////////////////////////////
void GameState_Pause::Draw(){
}

///////////////////////////////////////////////////////////////////////////////
// フレーム開始時処理
///////////////////////////////////////////////////////////////////////////////
void GameState_Pause::BeginFrame(){
}


///////////////////////////////////////////////////////////////////////////////
// フレーム終了時処理
///////////////////////////////////////////////////////////////////////////////
void GameState_Pause::EndFrame(){
}


///////////////////////////////////////////////////////////////////////////////
// コライダーを渡す処理
///////////////////////////////////////////////////////////////////////////////
void GameState_Pause::HandOverColliders(){
}


//////////////////////////////////////////////////////////////////////////////
// ステート管理処理
//////////////////////////////////////////////////////////////////////////////
void GameState_Pause::ManageState(){
    // ポーズ中
    if(!isExit_){
        if(pauseTimer_.GetProgress() > 0.0f){
            if(Input::IsTriggerKey(DIK_ESCAPE)){
                isExit_ = true;
                // 透明にしていく
                backSpriteObj_->GetComponent<ColorControlComponent>()->RevercePlay();

                // メニュー項目も逆再生で画面外へ
                for(int32_t i = 0; i < menus_.size(); i++){
                    menus_[i]->GetComponent<Routine2DComponent>()->RevercePlay();
                }

                // 音声再生
                AudioManager::PlayAudio(AudioDictionary::Get("Return"), false, 0.5f);
            }
        }

        // スペースキーで決定
        if(Input::IsTriggerKey(DIK_SPACE)){

            // 選択項目に応じた処理
            switch(selectedIndex_){
            case Resume:// ポーズ解除
                // 背景を透明にしていく
                backSpriteObj_->GetComponent<ColorControlComponent>()->RevercePlay();
                // メニュー項目も逆再生で画面外へ
                for(int32_t i = 0; i < menus_.size(); i++){
                    menus_[i]->GetComponent<Routine2DComponent>()->RevercePlay();
                }
                isExit_ = true;
                break;

            case Retry:// リトライ
                pScene_->EraseFromHierarchy(pauseItemsParent_);
                RythmGameManager::GetInstance()->Retry();
                pScene_->EndEvent();
                break;

            case ToSelect:// タイトルに戻る
                pScene_->ChangeScene("Game");// gameSceneのselectState
                return;

            default:
                break;
            }

        }


    } else{// ポーズ終了中

        // タイマーが0になったら
        if(pauseTimer_.GetProgress() == 0.0f){
            // 再びゲームに戻る
            RythmGameManager::GetInstance()->Resume();
            pScene_->EraseFromHierarchy(pauseItemsParent_);
            pScene_->EndEvent();
        }
    }
}


//////////////////////////////////////////////////////////////////////////////
// メニュー項目選択
//////////////////////////////////////////////////////////////////////////////
void GameState_Pause::SelectMenuItem(){

    // 上下の入力取得
    int inputDir = Input::IsTriggerKey(DIK_S) - Input::IsTriggerKey(DIK_W);

    // 選択インデックス更新
    int prevIndex = selectedIndex_;
    selectedIndex_ += inputDir;
    selectedIndex_ = MyFunc::Spiral(selectedIndex_, 0, PauseMenuItemCount - 1);

    if(inputDir != 0){

        // 逆方向に再生する
        menus_[prevIndex]->GetChild("backScrollObject")->GetComponent<Routine2DComponent>()->RevercePlay();
        menus_[selectedIndex_]->GetChild("backScrollObject")->GetComponent<Routine2DComponent>()->RevercePlay();

        // 選択中の項目だけ色を変える
        for(int32_t i = 0; i < menus_.size(); i++){

            if(i != selectedIndex_){
                menus_[i]->GetComponent<ColorControlComponent>()->SetIsActive(false);
                menus_[i]->masterColor_ = Color(0.4f, 0.4f, 0.4f, 1.0f);
            } else{
                menus_[i]->GetComponent<ColorControlComponent>()->SetIsActive(true);
            }
        }

        // 音声再生
        AudioManager::PlayAudio(AudioDictionary::Get("ItemSelect"), false, 0.5f);
    }
}
