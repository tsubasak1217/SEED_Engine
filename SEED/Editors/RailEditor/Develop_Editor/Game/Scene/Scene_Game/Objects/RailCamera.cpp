#include "RailCamera.h"
#include "SEED.h"

RailCamera::~RailCamera(){}

void RailCamera::Update(){

#ifdef _DEBUG

    ImGui::Begin("RailCamera");

    if(isRailCameraActive_){
    
        ImGui::Checkbox("isRailCameraActive", &isRailCameraActive_);

    } else{

        if(ImGui::Checkbox("isRailCameraActive", &isRailCameraActive_)){
            SEED::SetCamera("railCamera");
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

        // 注視点
        targetPoint_ = MyMath::CatmullRomPosition(pRailInfo_->controlPoints_, rail_t_);

        // ベクトルから角度を求める
        Vector3 dif = MyMath::Normalize(targetPoint_ - transform_.translate_);
        transform_.rotate_ = {
        std::atan2(-dif.y, MyMath::Length(dif)),// 縦方向の回転角(X軸回り)
        std::atan2(dif.x, dif.z),// 横方向の回転角(Y軸回り)
        0.0f
        };

        transform_.translate_ = targetPoint_;
    }

    UpdateMatrix();
}
