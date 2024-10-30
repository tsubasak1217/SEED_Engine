#include "RailInfo.h"
#include "ShapeMath.h"

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

    // レール用の分割数
    int railCount = totalSubdivision / 8;
    std::vector<Vector3> railPoints;
    for(int i = 0; i < railCount; i++){

        float rail_t = float(i) / float(railCount);

        int size = int(controlPoints_.size() - 1);
        float t2 = std::fmod(rail_t * size, 1.0f);
        int idx = int(rail_t * size);
        int nextIdx = std::clamp(idx + 1, 0, (int)controlPoints_.size() - 1);

        Vector3 rotate;
        rotate = MyMath::Lerp(twistModels_[idx]->rotate_, twistModels_[nextIdx]->rotate_, t2);

        Vector3 railPoint[2]{};
        railPoint[0] = (Vector3(1.0f, 0.0f, 0.0f) * RotateMatrix(rotate)) * railHalfWidth_;
        railPoint[1] = (Vector3(-1.0f, 0.0f, 0.0f) * RotateMatrix(rotate)) * railHalfWidth_;

        Vector3 catmullromPos = MyMath::CatmullRomPosition(controlPoints_, rail_t);

        //レールの描画頂点の計算
        railPoints.push_back(catmullromPos + railPoint[0]);
        railPoints.push_back(catmullromPos + railPoint[1]);
    }

    int railGH = TextureManager::LoadTexture("rail.png");
    for(int i = 0; i < ((int)railPoints.size() / 2) - 1; i++){

        Quad quad = MakeEqualQuad(1.0f);
        quad.lightingType = LIGHTINGTYPE_NONE;
        quad.localVertex[0] = railPoints[(i * 2) + 3];
        quad.localVertex[1] = railPoints[(i * 2) + 2];
        quad.localVertex[2] = railPoints[i * 2 + 1];
        quad.localVertex[3] = railPoints[i * 2];
        
        SEED::DrawQuad(quad, railGH);
    }
}
