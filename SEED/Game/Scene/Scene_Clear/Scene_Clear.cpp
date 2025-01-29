#include "Scene_Clear.h"
#include "InputManager/InputManager.h"
#include "Scene_Title.h"
#include "Sprite.h"
#include "SEED.h"

Scene_Clear::Scene_Clear(SceneManager* pSceneManager){
    pSceneManager_ = pSceneManager;
}

Scene_Clear::~Scene_Clear(){}

void Scene_Clear::Initialize(){}

void Scene_Clear::Finalize(){}

void Scene_Clear::Update(){
    if(currentState_){
        currentState_->Update();
    }
}

void Scene_Clear::Draw(){
    if(currentState_){
        currentState_->Draw();
    }

    Sprite sprite("win.png");
    sprite.size = { 1280.0f,720.0f };
    SEED::DrawSprite(sprite);
}

void Scene_Clear::EndFrame(){
    if(Input::IsTriggerPadButton(PAD_BUTTON::B | PAD_BUTTON::A)){
        ChangeScene(new Scene_Title(pSceneManager_));
    }
}
