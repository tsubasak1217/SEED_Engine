#include "Scene_Base.h"
#include <Game/GameSystem.h>
#include <SEED/Source/Basic/Object/GameObject.h>

////////////////////////////////////////////////////////////////////
// 
//  コンストラクタ
// 
////////////////////////////////////////////////////////////////////
Scene_Base::Scene_Base(){
    hierarchy_ = std::make_unique<Hierarchy>();
}

////////////////////////////////////////////////////////////////////
//
//  初期化処理
//
////////////////////////////////////////////////////////////////////
void Scene_Base::Initialize(){

    if(currentState_){
        currentState_->Initialize();
    }
}

////////////////////////////////////////////////////////////////////
//
//  終了処理
//
////////////////////////////////////////////////////////////////////
void Scene_Base::Finalize(){

    if(currentState_){
        currentState_->Finalize();
    }

    hierarchy_->Finalize();
}

////////////////////////////////////////////////////////////////////
//
//  更新処理
//
////////////////////////////////////////////////////////////////////
void Scene_Base::Update(){
    // ヒエラルキー内のオブジェクトの更新
    hierarchy_->Update();

    /*========== 各状態固有の更新 ===========*/

    if(currentState_){
        currentState_->Update();
    }

    if(currentEventState_){
        currentEventState_->Update();
    }

    // 編集処理
    SceneEdit();
}


////////////////////////////////////////////////////////////////////
//
//  描画処理
//
////////////////////////////////////////////////////////////////////
void Scene_Base::Draw(){
    /*=========== 各状態固有の描画 ============*/

    if(currentEventState_){
        currentEventState_->Draw();
    }

    if(currentState_){
        currentState_->Draw();
    }

    /*======== 各オブジェクトの基本描画 ========*/

    // ヒエラルキー内のオブジェクトの描画
    hierarchy_->Draw();
}


///////////////////////////////////////////////////////////////////
//
//  フレーム開始・終了時処理
//
///////////////////////////////////////////////////////////////////
void Scene_Base::BeginFrame(){

    // 現在のステートがあればフレーム開始処理を行う
    if(currentState_){
        currentState_->BeginFrame();
    }

    // ヒエラルキー内のオブジェクトの開始処理
    hierarchy_->BeginFrame();
}

void Scene_Base::EndFrame(){
    // 現在のステートがあればフレーム終了処理を行う
    if(currentState_){
        currentState_->EndFrame();
    }

    // ヒエラルキー内のオブジェクトの描画
    hierarchy_->EndFrame();

    // ステートクラス内の遷移処理を実行
    if(currentState_){
        currentState_->ManageState();
    }
}

////////////////////////////////////////////////////////////////////
// 
// コリジョンマネージャにコライダーを渡す
//
////////////////////////////////////////////////////////////////////

void Scene_Base::HandOverColliders(){
    if(currentState_){
        currentState_->HandOverColliders();
    }
}


////////////////////////////////////////////////////////////////////
//
//  シーン・ステート関連
//
////////////////////////////////////////////////////////////////////
void Scene_Base::ChangeScene(const std::string& nextSceneName){
    GameSystem::ChangeScene(nextSceneName);
}

void Scene_Base::ChangeState(State_Base* nextState){
    currentState_.reset(nextState);
}

void Scene_Base::CauseEvent(EventState_Base* nextEventState){
    currentEventState_.reset(nextEventState);
}


////////////////////////////////////////////////////////////////////
// 
//  ヒエラルキー関連
// 
////////////////////////////////////////////////////////////////////

// ヒエラルキーへの登録・削除
void Scene_Base::RegisterToHierarchy(GameObject* gameObject){
    hierarchy_->RegisterGameObject(gameObject);
}

void Scene_Base::RegisterToHierarchy(GameObject2D* gameObject){
    hierarchy_->RegisterGameObject(gameObject);
}

void Scene_Base::RemoveFromHierarchy(GameObject* gameObject){
    hierarchy_->RemoveGameObject(gameObject);
}

void Scene_Base::RemoveFromHierarchy(GameObject2D* gameObject){
    hierarchy_->RemoveGameObject(gameObject);
}

// オブジェクトの存在確認
bool Scene_Base::IsExistObject(uint32_t gameObjectandle)const{
    return hierarchy_->IsExistObject(gameObjectandle);
}

bool Scene_Base::IsExistObject2D(uint32_t gameObjectandle)const{
    return hierarchy_->IsExistObject2D(gameObjectandle);
}

void Scene_Base::SortObject2DByTranslate(ObjSortMode sortMode){
    hierarchy_->SortObject2DByTranslate(sortMode);
}