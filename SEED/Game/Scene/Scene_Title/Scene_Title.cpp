#include "Scene_Title.h"

///local
//lib
#include "InputManager/InputManager.h"
//external
#include "../adapter/json/JsonCoordinator.h"

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
    titleLogo_ = std::make_unique<Sprite>("checkerBoard.png");
    titleLogo_->translate = {780.f,360.f};

    howToStartUI_ = std::make_unique<Sprite>("monsterBall.png");
    howToStartUI_->translate = {640.f,680.f};

    //===================== PlayerModel =====================//
    playerModel_ = std::make_unique<Model>("dinosaur.gltf");
    playerModel_->isRotateWithQuaternion_ = false;
    playerModel_->StartAnimation("handUpRunning",true);

    // gameSceneとは分ける
    JsonCoordinator::LoadGroup("TitlePlayerModel");
    JsonCoordinator::RegisterItem("TitlePlayerModel","Rotate",playerModel_->rotate_);
    JsonCoordinator::RegisterItem("TitlePlayerModel","Translate",playerModel_->translate_);
    playerModel_->UpdateMatrix();

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

    //===================== PlayerModel =====================//
    playerModel_->Update();
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
    //===================== PlayerModel =====================//
    playerModel_->Draw();
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
    //===================== Json =====================//
#ifdef _DEBUG
    ImGui::Begin("TitlePlayerModel");
    JsonCoordinator::RenderGroupUI("TitlePlayerModel");
    if(ImGui::Button("Save")){
        JsonCoordinator::SaveGroup("TitlePlayerModel");
    }
    ImGui::End();
#endif // _DEBUG
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
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//  すべてのコライダーをコリジョンマネージャに渡す
//
/////////////////////////////////////////////////////////////////////////////////////////
void Scene_Title::HandOverColliders(){}
