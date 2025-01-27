#include "Scene_Title.h"

///local
//lib
#include "InputManager/InputManager.h"

//state
#include "State/TitleState_Enter.h"
#include "State/TitleState_Main.h"
#include "State/TitleState_Out.h"

////////////////////////////////////////////////////////////////////////////////////////////////
//
// コンストラクタ・デストラクタ
//
//////////////////////////////////////////////////////////////////////////////////////////////
Scene_Title::Scene_Title(){
    Initialize();
    SEED::SetCamera("main");
}

Scene_Title::~Scene_Title(){}

////////////////////////////////////////////////////////////////////////////////////////////////
//
// 初期化
//
//////////////////////////////////////////////////////////////////////////////////////////////
void Scene_Title::Initialize(){
    //===================== state =====================//
    currentState_ = std::make_unique<TitleState_Enter>(this);
    currentState_->Initialize();

    //===================== UI =====================//
    titleLogo_ = std::make_unique<UI>("titleLogo");
    titleLogo_->Initialize("Assets/checkerBoard.png");

    toNextButton_ = std::make_unique<UI>("toNextButton_");
    toNextButton_->Initialize("Assets/monsterBall.png");

    toExitButton_ = std::make_unique<UI>("toExitButton_");
    toExitButton_->Initialize("Assets/uvChecker.png");
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
// 終了処理
//
//////////////////////////////////////////////////////////////////////////////////////////////
void Scene_Title::Finalize(){
    //===================== state =====================//
    if(currentState_){
        currentState_->Finalize();
    }
    //===================== UI =====================//
    titleLogo_->Finalize();
    toNextButton_->Finalize();
    toExitButton_->Finalize();
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
// 更新
//
//////////////////////////////////////////////////////////////////////////////////////////////
void Scene_Title::Update(){
    //===================== state =====================//
    if(currentState_){
        currentState_->Update();
    }

    //===================== UI =====================//
    titleLogo_->Update();
    toNextButton_->Update();
    toExitButton_->Update();
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
// 描画
//
//////////////////////////////////////////////////////////////////////////////////////////////
void Scene_Title::Draw(){
    //===================== state =====================//
    if(currentState_){
        currentState_->Draw();
    }
    //===================== UI =====================//
    titleLogo_->Draw();
    toNextButton_->Draw();
    toExitButton_->Draw();
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
// フレーム開始処理
//
//////////////////////////////////////////////////////////////////////////////////////////////
void Scene_Title::BeginFrame(){
    Scene_Base::BeginFrame();
    //===================== state =====================//
    if(currentState_){
        currentState_->BeginFrame();
    }
    //===================== UI =====================//
    titleLogo_->BeginFrame();
    toNextButton_->BeginFrame();
    toExitButton_->BeginFrame();
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
// フレーム終了処理
//
//////////////////////////////////////////////////////////////////////////////////////////////
void Scene_Title::EndFrame(){
    //===================== state =====================//
    if(currentState_){
        currentState_->EndFrame();
    }
    //===================== UI =====================//
    titleLogo_->EndFrame();
    toNextButton_->EndFrame();
    toExitButton_->EndFrame();
}


/////////////////////////////////////////////////////////////////////////////////////////
//
//  すべてのコライダーをコリジョンマネージャに渡す
//
/////////////////////////////////////////////////////////////////////////////////////////
void Scene_Title::HandOverColliders(){}
