#include "Scene_Game.h"
#include <GameState_Play.h>
#include <GameState_Enter.h>
#include <SEED.h>

Scene_Game::Scene_Game(SceneManager* pSceneManager){
    pSceneManager_ = pSceneManager;
    ChangeState(new GameState_Play(this));
    Initialize();
};

Scene_Game::~Scene_Game(){}

void Scene_Game::Initialize(){

    ////////////////////////////////////////////////////
    //  モデル生成
    ////////////////////////////////////////////////////


    ////////////////////////////////////////////////////
    //  ライトの方向初期化
    ////////////////////////////////////////////////////

    SEED::GetDirectionalLight()->direction_ = { 0.0f,-1.0f,0.0f };


    ////////////////////////////////////////////////////
    //  カメラ初期化
    ////////////////////////////////////////////////////

    SEED::GetCamera()->transform_.translate_ = { 0.0f,3.0f,0.0f };
    SEED::GetCamera()->Update();

    ////////////////////////////////////////////////////
    //  天球の作成
    ////////////////////////////////////////////////////


    ////////////////////////////////////////////////////
    //  解像度の初期設定
    ////////////////////////////////////////////////////

    SEED::ChangeResolutionRate(resolutionRate_);
}

void Scene_Game::Finalize(){}

void Scene_Game::Update(){
    /*======================= 前フレームの値保存 ======================*/

    preRate_ = resolutionRate_;

    /*========================== ImGui =============================*/

#ifdef _DEBUG

    ImGui::Begin("Control Point");

    if(ImGui::Button("Add Control Point")){

        // カメラの正面にコントロールポイントを配置
        Vector3 emitPos = SEED::GetCamera()->transform_.translate_ + SEED::GetCamera()->normal_ * 5.0f;

        controlModels_.emplace_back(std::make_unique<Model>("sphere"));
        controlPoints_.emplace_back(&controlModels_.back()->translate_);
        controlModels_.back()->color_ = { 1.0f,1.0f,0.0f,1.0f };
        controlModels_.back()->scale_ = { 0.5f,0.5f,0.5f };
        controlModels_.back()->translate_ = emitPos;

        twistModels_.emplace_back(std::make_unique<Model>("sphere"));
        twistModels_.back()->color_ = { 0.0f,1.0f,0.0f,1.0f };
        twistModels_.back()->scale_ = { 0.5f,0.5f,0.5f };
        twistModels_.back()->translate_ = emitPos;
        twistModels_.back()->translate_.y += 0.5f;
    }

    for(int i = 0; i < controlModels_.size(); i++){

        // 動かす前の座標保存
        Vector3 before = controlModels_[i]->translate_;

        ImGui::DragFloat3(
            std::string(std::string("controlPoint##") + std::to_string(i)).c_str(),
            &controlModels_[i]->translate_.x, 0.05f
        );

        // 動いた差分
        Vector3 dif = controlModels_[i]->translate_ - before;
        // アップベクトルの座標も同時に動かす
        twistModels_[i]->translate_ += dif;

        ImGui::DragFloat3(
            std::string(std::string("upPoint(twist)##") + std::to_string(i)).c_str(),
            &twistModels_[i]->translate_.x, 0.05f
        );

        ImGui::Separator();
        ImGui::Dummy({ 0,5 });

        controlModels_[i]->UpdateMatrix();
        twistModels_[i]->UpdateMatrix();
    }

    for(int i = 0; i < controlPoints_.size(); i++){
        SEED::DrawLine(
            *controlPoints_[i],
            twistModels_[i]->translate_,
            { 0.0f,0.0f,1.0f,1.0f }
        );;
    }


    ImGui::End();

#endif

    /*========================= 解像度の更新 ==========================*/

    // 前フレームと値が違う場合のみ更新
    if(resolutionRate_ != preRate_){
        SEED::ChangeResolutionRate(resolutionRate_);
    }

    /*========================= 各状態の更新 ==========================*/

    currentState_->Update();

}

void Scene_Game::Draw(){
    // 描画
    for(auto& controlPoint : controlModels_){
        controlPoint->Draw();
    }

    for(auto& twistModel : twistModels_){
        twistModel->Draw();
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