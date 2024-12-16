#include "Scene_Clear.h"
#include "InputManager.h"
#include "Scene_Title.h"
#include "SEED.h"

Scene_Clear::Scene_Clear(){}

Scene_Clear::~Scene_Clear(){}

void Scene_Clear::Initialize(){}

void Scene_Clear::Finalize(){}

void Scene_Clear::Update(){

    if(Input::IsTriggerKey(DIK_SPACE)){
        nextScene_ = new Scene_Title();
    }
}

void Scene_Clear::Draw(){

    Quad2D screenRect = MakeEqualQuad2D(2000.0f, {1.0f,1.0f,0.0f,1.0f});
    SEED::DrawQuad2D(screenRect);
}
