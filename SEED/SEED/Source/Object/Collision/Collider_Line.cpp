#include "Collider_Line.h"
#include "Base/BaseObject.h"
#include <SEED.h>

//////////////////////////////////////////////////////////////////
// コンストラクタ
//////////////////////////////////////////////////////////////////
Collider_Line::Collider_Line() : Collider(){
    colliderType_ = ColliderType::Line;
}

//////////////////////////////////////////////////////////////////
// 行列の更新
//////////////////////////////////////////////////////////////////
void Collider_Line::UpdateMatrix(){
    // 行列の更新
    Collider::UpdateMatrix();

    // 本体の更新
    body_.origin_ = local_.origin_ * worldMat_ + offset_;
    body_.end_ = local_.end_ * worldMat_ + offset_;

    // 八分木用のAABB更新
    UpdateBox();
}

//////////////////////////////////////////////////////////////////
// 描画
//////////////////////////////////////////////////////////////////
void Collider_Line::Draw(){
    SEED::DrawLine(body_.origin_, body_.end_, color_);
}

//////////////////////////////////////////////////////////////////
// 八分木用のAABB更新
//////////////////////////////////////////////////////////////////
void Collider_Line::UpdateBox(){
    Vector3 min;
    Vector3 max;

    min.x = (std::min)({ body_.origin_.x, body_.end_.x });
    min.y = (std::min)({ body_.origin_.y, body_.end_.y });
    min.z = (std::min)({ body_.origin_.z, body_.end_.z });

    max.x = (std::max)({ body_.origin_.x, body_.end_.x });
    max.y = (std::max)({ body_.origin_.y, body_.end_.y });
    max.z = (std::max)({ body_.origin_.z, body_.end_.z });

    coverAABB_.center = (min + max) * 0.5f;
    coverAABB_.halfSize = (max - min) * 0.5f;
}


//////////////////////////////////////////////////////////////////
// ImGuiでの編集
//////////////////////////////////////////////////////////////////
void Collider_Line::Edit(const std::string& headerName){
#ifdef _DEBUG

    if(ImGui::CollapsingHeader(headerName.c_str())){
        ImGui::Indent();

        color_ = { 1.0f,1.0f,0.0f,1.0f };// 編集中のコライダーの色(黄色)

        // 中心座標
        ImGui::Text("------ Origin ------");
        ImGui::Indent();
        ImGui::DragFloat3("x:y:z", &local_.origin_.x, 0.05f);
        ImGui::Unindent();

        ImGui::Dummy(ImVec2(0.0f, 10.0f));

        // 半径
        ImGui::Text("-------- End -------");
        ImGui::Indent();
        ImGui::DragFloat3("x:y:z", &local_.end_.x, 0.05f);
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
