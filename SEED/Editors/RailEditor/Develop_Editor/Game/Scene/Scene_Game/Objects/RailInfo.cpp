#include "RailInfo.h"

RailInfo::~RailInfo(){}

void RailInfo::Update(){

#ifdef _DEBUG

    ImGui::Begin("Control Point");

    if(ImGui::Button("Add Control Point") or InputManager::IsTriggerPadButton(PAD_BUTTON::A)){

        // カメラの正面にコントロールポイントを配置
        Vector3 emitPos = SEED::GetCamera()->transform_.translate_ + SEED::GetCamera()->normal_ * 5.0f;

        controlModels_.emplace_back(std::make_unique<Model>("sphere"));
        controlModels_.back()->color_ = { 1.0f,1.0f,0.0f,1.0f };
        controlModels_.back()->scale_ = { 0.5f,0.5f,0.5f };
        controlModels_.back()->translate_ = emitPos;

        controlPoints_.emplace_back(&controlModels_.back()->translate_);
        controlAngles_.emplace_back(&controlModels_.back()->rotate_);

        twistModels_.emplace_back(std::make_unique<Model>("suzanne2"));
        twistModels_.back()->color_ = { 0.0f,1.0f,0.0f,1.0f };
        twistModels_.back()->scale_ = { 0.5f,0.5f,0.5f };
        twistModels_.back()->translate_ = emitPos;
    }

    for(int i = 0; i < controlModels_.size(); i++){

        // 動かす前の座標保存
        Vector3 before = controlModels_[i]->translate_;

        ImGui::DragFloat3(
            std::string(std::string("controlPoint##") + std::to_string(i)).c_str(),
            &controlModels_[i]->translate_.x, 0.01f
        );

        // 動いた差分
        Vector3 dif = controlModels_[i]->translate_ - before;
        // アップベクトルの座標も同時に動かす
        twistModels_[i]->translate_ += dif;

        ImGui::DragFloat3(
            std::string(std::string("rotate(twist)##") + std::to_string(i)).c_str(),
            &twistModels_[i]->rotate_.x, 0.01f
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

}

void RailInfo::Draw(){

    // 描画
    for(auto& controlPoint : controlModels_){
        controlPoint->Draw();
    }

    for(auto& twistModel : twistModels_){
        twistModel->Draw();
    }

    // レール間の線の描画
    int totalSubdivision = ((int)controlPoints_.size() - 1) * kSubdivision_;
    for(int i = 0; i < totalSubdivision - 1; i++){
        SEED::DrawLine(
            MyMath::CatmullRomPosition(controlPoints_, float(i) / float(totalSubdivision)),
            MyMath::CatmullRomPosition(controlPoints_, float(i + 1) / float(totalSubdivision)),
            { 1.0f,0.0f,0.0f,1.0f }
        );
    }

}
