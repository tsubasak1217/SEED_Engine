#include "Scene_Title.h"
#include "Scene_Game.h"
#include "Scene_Clear.h"
#include "InputManager.h"

Scene_Title::Scene_Title()
{
}

Scene_Title::~Scene_Title()
{
}

void Scene_Title::Initialize()
{
}

void Scene_Title::Finalize()
{
}

void Scene_Title::Update()
{
    if(Input::IsTriggerKey(DIK_SPACE)){
        nextScene_ = new Scene_Game();
    }

}

void Scene_Title::Draw()
{
    Quad2D screenRect = MakeEqualQuad2D(2000.0f, { 0.0f,1.0f,1.0f,1.0f });
    SEED::DrawQuad2D(screenRect);
}
