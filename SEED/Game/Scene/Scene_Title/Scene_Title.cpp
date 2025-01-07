#include "Scene_Title.h"
#include "InputManager/InputManager.h"
#include "SceneManager.h"
#include "Scene_Game.h"

Scene_Title::Scene_Title(SceneManager* pSceneManager){
    pSceneManager_ = pSceneManager;
    SEED::SetCamera("main");
}

Scene_Title::~Scene_Title(){}

void Scene_Title::Initialize(){}

void Scene_Title::Finalize(){}

void Scene_Title::Update(){
    //currentState_->Update();

    if(Input::IsTriggerPadButton(PAD_BUTTON::B)){
        pSceneManager_->ChangeScene(new Scene_Game(pSceneManager_));
    }
}

void Scene_Title::Draw(){
    //currentState_->Draw();

    Sprite sprite("title.png");
    SEED::DrawSprite(sprite);


}

void Scene_Title::EndFrame(){}
