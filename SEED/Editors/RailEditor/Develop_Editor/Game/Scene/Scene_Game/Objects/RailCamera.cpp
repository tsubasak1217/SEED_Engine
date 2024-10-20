#include "RailCamera.h"
#include "SEED.h"

RailCamera::RailCamera(){
    debugModel_ = std::make_unique<Model>("suzanne");
}

RailCamera::~RailCamera(){}

void RailCamera::Update(){

#ifdef _DEBUG

    ImGui::Begin("RailCamera");

    if(isRailCameraActive_){
    
        ImGui::Checkbox("isRailCameraActive", &isRailCameraActive_);

    } else{

        if(ImGui::Checkbox("isRailCameraActive", &isRailCameraActive_)){

            SEED::SetCamera("railCamera");
            rail_t_ = 0.0f;
            if(pRailInfo_->controlPoints_.size() > 0){
                transform_.translate_ = *pRailInfo_->controlPoints_[0];
            }

        } else if(ImGui::Checkbox("isDebugCameraActive", &isDebugCameraActive_)){
            SEED::SetCamera("debug");
        }

        if(isDebugCameraActive_ == false){
            SEED::SetCamera("main");
        }
    }

    ImGui::SliderFloat("t", &rail_t_, 0.0f, 1.0f);
    ImGui::Checkbox("isMove", &isMove_);

    ImGui::End();

#endif // _DEBUG


    if(isMove_){

        // 媒介変数を加算
        rail_t_ += addValue_;
        rail_t_ = std::clamp(rail_t_, 0.0f, 1.0f);

        // 注視点
        targetPoint_ = MyMath::CatmullRomPosition(pRailInfo_->controlPoints_, rail_t_);

        // tからインデックスを求める
        int size = int(pRailInfo_->controlPoints_.size() - 1);
        float t2 = std::fmod(rail_t_ * size, 1.0f);
        int idx = int(rail_t_ * size);
        int nextIdx = std::clamp(idx + 1, 0, (int)pRailInfo_->controlPoints_.size() - 1);

        // z軸の回転を求める
        float theta1 = std::acos(MyMath::Dot({ 0.0f,1.0f,0.0f }, MyMath::Normalize(pRailInfo_->twistModels_[idx]->translate_ - *pRailInfo_->controlPoints_[idx])));
        float theta2 = std::acos(MyMath::Dot({ 0.0f,1.0f,0.0f }, MyMath::Normalize(pRailInfo_->twistModels_[nextIdx]->translate_ - *pRailInfo_->controlPoints_[nextIdx])));
        float currentIdxRotateZ = theta1 + (theta2 - theta1) * t2;

        // ベクトルから角度を求める
        Vector3 dif = MyMath::Normalize(targetPoint_ - transform_.translate_);
        transform_.rotate_ = {
        std::atan2(-dif.y, MyMath::Length(dif)),// 縦方向の回転角(X軸回り)
        std::atan2(dif.x, dif.z),// 横方向の回転角(Y軸回り)
        -currentIdxRotateZ
        };

        transform_.translate_ = targetPoint_;

        // カメラ確認用モデルの更新
        debugModel_->rotate_ = transform_.rotate_;
        debugModel_->translate_ = transform_.translate_;
        debugModel_->UpdateMatrix();
    }

    UpdateMatrix();
}

void RailCamera::Draw(){
    debugModel_->Draw();
}
