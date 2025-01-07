#include "Collider_OBB.h"
#include "Base/BaseObject.h"
#include "Collision/Collider_Sphere.h"
#include "Collision/Collider_AABB.h"
#include "Collision/Collider_Line.h"
#include "Collision/Collider_Capsule.h"
#include "Collision/Collider_Plane.h"
#include "CollisionManaer/Collision.h"
#include <SEED.h>

////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
Collider_OBB::Collider_OBB() : Collider(){
    colliderType_ = ColliderType::OBB;
}

////////////////////////////////////////////////////////////////
// 行列の更新
////////////////////////////////////////////////////////////////
void Collider_OBB::UpdateMatrix(){
    // 行列の更新
    Collider::UpdateMatrix();

    // 本体の更新
    body_.center = local_.center * worldMat_ + offset_;
    body_.rotate = ExtractRotation(worldMat_);

    // 八分木用のAABB更新
    UpdateBox();
}

////////////////////////////////////////////////////////////////
// 描画
////////////////////////////////////////////////////////////////
void Collider_OBB::Draw(){
    SEED::DrawOBB(body_, color_);
}

////////////////////////////////////////////////////////////////
// 衝突判定
////////////////////////////////////////////////////////////////
void Collider_OBB::CheckCollision(Collider* collider){

    // すでに衝突している場合は処理を行わない
    if(collisionList_.find(collider->GetColliderID()) != collisionList_.end()){ return; }

    switch(collider->GetColliderType()){
    case ColliderType::Sphere:
    {
        Collider_Sphere* sphere = dynamic_cast<Collider_Sphere*>(collider);
        if(Collision::OBB::Sphere(body_, sphere->GetSphere())){
            OnCollision(collider,collider->GetObjectType());
            collider->OnCollision(this,objectType_);
        }
        break;
    }
    case ColliderType::AABB:
    {
        Collider_AABB* aabb = dynamic_cast<Collider_AABB*>(collider);
        if(Collision::OBB::AABB(body_, aabb->GetAABB())){
            OnCollision(collider,collider->GetObjectType());
            collider->OnCollision(this,objectType_);
        }
        break;
    }
    case ColliderType::OBB:
    {
        Collider_OBB* obb = dynamic_cast<Collider_OBB*>(collider);
        if(Collision::OBB::OBB(body_, obb->GetOBB())){
            OnCollision(collider,collider->GetObjectType());
            collider->OnCollision(this,objectType_);
        }
        break;
    }
    case ColliderType::Line:
    {
        Collider_Line* line = dynamic_cast<Collider_Line*>(collider);
        if(Collision::OBB::Line(body_, line->GetLine())){
            OnCollision(collider,collider->GetObjectType());
            collider->OnCollision(this,objectType_);
        }
        break;
    }
    }

}

////////////////////////////////////////////////////////////////
// 八分木用のAABB更新
////////////////////////////////////////////////////////////////
void Collider_OBB::UpdateBox(){
    coverAABB_.center = body_.center;
    float maxLen = (std::max)({ body_.halfSize.x, body_.halfSize.y, body_.halfSize.z });
    coverAABB_.halfSize = Vector3(maxLen, maxLen, maxLen);
}

////////////////////////////////////////////////////////////////
// ImGuiでの編集
////////////////////////////////////////////////////////////////
void Collider_OBB::Edit(){
#ifdef _DEBUG

    std::string colliderID = "##" + std::to_string(colliderID_);// コライダーID
    color_ = { 1.0f,1.0f,0.0f,1.0f };// 編集中のコライダーの色(黄色)

    // 中心座標
    ImGui::Text("------ Center ------");
    ImGui::Indent();
    ImGui::DragFloat3(std::string("Center" + colliderID).c_str(), &local_.center.x, 0.1f);
    ImGui::Unindent();

    ImGui::Dummy(ImVec2(0.0f, 10.0f));

    // 半径
    ImGui::Text("------ HalfSie ------");
    ImGui::Indent();
    ImGui::DragFloat3(std::string("HalfSie" + colliderID).c_str(), &body_.halfSize.x, 0.025f);
    ImGui::Unindent();

    ImGui::Dummy(ImVec2(0.0f, 10.0f));

    // 回転
    ImGui::Text("------ Rotate ------");
    ImGui::Indent();
    ImGui::DragFloat3(std::string("Rotate" + colliderID).c_str(), &rotate_.x, 0.1f);
    ImGui::Unindent();

    ImGui::Dummy(ImVec2(0.0f, 10.0f));

    // オフセット
    ImGui::Text("------ Offset ------");
    ImGui::Indent();
    ImGui::DragFloat3(std::string("Offset" + colliderID).c_str(), &offset_.x, 0.1f);
    ImGui::Unindent();

    // アニメーションフラグ
    ImGui::Text("------ Animation ------");
    ImGui::Checkbox("Animation", &isAnimation_);

#endif // _DEBUG
}


////////////////////////////////////////////////////////////////
// コライダーの情報をjson形式でまとめる
////////////////////////////////////////////////////////////////
nlohmann::json Collider_OBB::GetJsonData(){
    nlohmann::json json;

    // 全般の情報
    json.merge_patch(Collider::GetJsonData());

    // コライダーの種類
    json["colliderType"] = "OBB";

    // ローカル座標
    json["local"]["center"] = local_.center;
    json["local"]["halfSize"] = body_.halfSize;

    // オフセット
    json["offset"] = offset_;

    return json;
}

////////////////////////////////////////////////////////////////
// jsonデータからコライダーの情報を読み込む
////////////////////////////////////////////////////////////////
void Collider_OBB::LoadFromJson(const nlohmann::json& jsonData){
    // 全般の情報
    Collider::LoadFromJson(jsonData);

    // ローカル座標
    local_.center = jsonData["local"]["center"];
    body_.halfSize = jsonData["local"]["halfSize"];

    // オフセット
    offset_ = jsonData["offset"];

    // 行列の更新
    UpdateMatrix();
}
