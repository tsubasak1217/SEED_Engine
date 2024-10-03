#include "Scene_Game.h"
#include <GameState_Play.h>
#include <GameState_Enter.h>
#include <SEED.h>
#include <Environment.h>

#include "ShapeMath.h"

Scene_Game::Scene_Game(SceneManager* pSceneManager){
    pSceneManager_ = pSceneManager;
    ChangeState(new GameState_Play(this));
    Initialize();
};

Scene_Game::~Scene_Game(){}

void Scene_Game::Initialize(){
}

void Scene_Game::Finalize(){}

void Scene_Game::Update(){
    /*======================= 前フレームの値保存 ======================*/


    /*========================== ImGui =============================*/

#ifdef _DEBUG

    ImGui::Begin("Control Point");
    
    if(ImGui::Button("Add Control Point")){
        controlModels_.emplace_back(std::make_unique<Model>("sphere"));
        controlPoints_.emplace_back(&controlModels_.back()->translate_);
    }

    for(int i = 0; i < controlModels_.size(); i++){
        ImGui::DragFloat3(
            std::string(std::string("controlPoint##") + std::to_string(i)).c_str(),
            &controlModels_[i]->translate_.x, 0.05f
        );

        controlModels_[i]->UpdateMatrix();
    }

    ImGui::End();

#endif

    /*========================= 各状態のの更新 ==========================*/

    currentState_->Update();

}



void Scene_Game::Draw(){

    // 描画
    for(auto& controlPoint : controlModels_){
        controlPoint->Draw();
    }

    if(controlPoints_.size() >= 4){
        int totalSubdivision = ((int)controlPoints_.size() - 1) * kSubdivision_;
        for(int i = 0; i < totalSubdivision - 1; i++){
            SEED::DrawLine(
                MyMath::CatmullRomPosition(controlPoints_, float(i) / float(totalSubdivision)),
                MyMath::CatmullRomPosition(controlPoints_, float(i + 1) / float(totalSubdivision)),
                { 1.0f,0.0f,0.0f,1.0f }
            );
        }
    }

    SEED::DrawGrid(1.0f, 100);
    currentState_->Draw();
}

void Scene_Game::MoveCamera(){

}
