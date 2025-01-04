#include "Collider_Capsule.h"
#include "Base/BaseObject.h"
#include <SEED.h>

//////////////////////////////////////////////////////////////////////////
// コンストラクタ
//////////////////////////////////////////////////////////////////////////
Collider_Capsule::Collider_Capsule() : Collider(){
    colliderType_ = ColliderType::Capsule;
}

//////////////////////////////////////////////////////////////////////////
// 行列の更新
//////////////////////////////////////////////////////////////////////////
void Collider_Capsule::UpdateMatrix(){
    // 行列の更新
    Collider::UpdateMatrix();

    // 本体の更新
    body_.origin = local_.origin * worldMat_ + offset_;
    body_.end = local_.end * worldMat_ + offset_;

    // 八分木用のAABB更新
    UpdateBox();
}

//////////////////////////////////////////////////////////////////////////
// 描画
//////////////////////////////////////////////////////////////////////////
void Collider_Capsule::Draw(){
    SEED::DrawCapsule(body_.origin,body_.end,body_.radius,6,color_);
}

//////////////////////////////////////////////////////////////////////////
// 八分木用のAABB更新
//////////////////////////////////////////////////////////////////////////
void Collider_Capsule::UpdateBox(){
    Vector3 min;
    Vector3 max;

    min.x = (std::min)({ body_.origin.x, body_.end.x }) - body_.radius;
    min.y = (std::min)({ body_.origin.y, body_.end.y }) - body_.radius;
    min.z = (std::min)({ body_.origin.z, body_.end.z }) - body_.radius;

    max.x = (std::max)({ body_.origin.x, body_.end.x }) + body_.radius;
    max.y = (std::max)({ body_.origin.y, body_.end.y }) + body_.radius;
    max.z = (std::max)({ body_.origin.z, body_.end.z }) + body_.radius;

    coverAABB_.center = (min + max) * 0.5f;
    coverAABB_.halfSize = (max - min) * 0.5f;
}


//////////////////////////////////////////////////////////////////////////
// ImGuiでの編集
//////////////////////////////////////////////////////////////////////////
void Collider_Capsule::Edit(const std::string& headerName){

#ifdef _DEBUG

    if(ImGui::CollapsingHeader(headerName.c_str())){
        ImGui::Indent();

        color_ = { 1.0f,1.0f,0.0f,1.0f };// 編集中のコライダーの色(黄色)

        // 中心座標
        ImGui::Text("------ Origin ------");
        ImGui::Indent();
        ImGui::DragFloat3("x:y:z", &local_.origin.x, 0.05f);
        ImGui::Unindent();

        ImGui::Dummy(ImVec2(0.0f, 10.0f));

        // 半径
        ImGui::Text("-------- End -------");
        ImGui::Indent();
        ImGui::DragFloat3("x:y:z", &local_.end.x, 0.05f);
        ImGui::Unindent();

        ImGui::Dummy(ImVec2(0.0f, 10.0f));

        // 半径
        ImGui::Text("------ Radius ------");
        ImGui::Indent();
        ImGui::DragFloat("radius", &body_.radius, 0.05f, 0.0f);
        ImGui::Unindent();

        ImGui::Dummy(ImVec2(0.0f, 10.0f));

        // オフセット
        ImGui::Text("------ Offset ------");
        ImGui::Indent();
        ImGui::DragFloat3("x:y:z", &offset_.x, 0.1f);
        ImGui::Unindent();

        ImGui::Unindent();
    }

#endif // _DEBUG

}

