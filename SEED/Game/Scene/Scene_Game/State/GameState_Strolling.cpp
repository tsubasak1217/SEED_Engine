#include "GameState_Strolling.h"
#include <SEED/Source/Basic/Scene/Scene_Base.h>

GameState_Strolling::GameState_Strolling(){

}

GameState_Strolling::GameState_Strolling(Scene_Base* pScene){
    pScene_ = pScene;
    pScene_->GetHierarchy()->LoadFromJson("Resources/Jsons/Scenes/scene.json");
    SEED::SetMainCamera("debug");

    text_ = TextBox2D(
        "WASD,QEで移動\nマウス右クリックしながら動かすとカメラ動きます\n\n画面右上のタブから[ポストエフェクト]>[新しいポストエフェクトを追加]で\n任意のポストエフェクトを追加&調整できます。\n同じ物でも重ね掛け可能です。"
    );
    text_.SetFont("DefaultAssets/M_PLUS_Rounded_1c/MPLUSRounded1c-Regular.ttf");
    text_.anchorPos = { 0.0f,0.0f };
    text_.transform.translate = { 20.0f, 20.0f };
    text_.size = { 800.0f, 720.0f };
    text_.textBoxVisible = false;
    text_.fontSize = 40.0f;
    text_.align = TextAlign::LEFT;
    text_.useOutline = true;
    text_.outlineWidth = 3.0f;
    text_.lineSpacing = 1.0f;
}

void GameState_Strolling::Initialize(){

}

void GameState_Strolling::Finalize(){
}

void GameState_Strolling::Update(){
}

void GameState_Strolling::Draw(){
    text_.Draw();
}

void GameState_Strolling::BeginFrame(){
}

void GameState_Strolling::EndFrame(){
}

void GameState_Strolling::HandOverColliders(){
}

void GameState_Strolling::ManageState(){
}
