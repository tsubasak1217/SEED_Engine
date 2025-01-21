#include "Collider_Sphere.h"
#include "Base/BaseObject.h"
#include "Collision/Collider_Sphere.h"
#include "Collision/Collider_AABB.h"
#include "Collision/Collider_OBB.h"
#include "Collision/Collider_Line.h"
#include "Collision/Collider_Capsule.h"
#include "Collision/Collider_Plane.h"
#include "CollisionManaer/Collision.h"
#include <SEED.h>

////////////////////////////////////////////////////////////
// コンストラクタ・デストラクタ
////////////////////////////////////////////////////////////
Collider_Sphere::Collider_Sphere() : Collider(){
    colliderType_ = ColliderType::Sphere;
}

Collider_Sphere::~Collider_Sphere(){}

////////////////////////////////////////////////////////////
// 行列の更新
////////////////////////////////////////////////////////////
void Collider_Sphere::UpdateMatrix(){

    // 行列の更新
    Collider::UpdateMatrix();
    // 本体の更新
    body_.center = local_.center * worldMat_ + offset_;
    // 八分木用のAABB更新
    UpdateBox();

}

////////////////////////////////////////////////////////////
// 描画
////////////////////////////////////////////////////////////
void Collider_Sphere::Draw(){
    SEED::DrawSphere(body_.center, body_.radius, 6, color_);
}


////////////////////////////////////////////////////////////
// フレーム開始時処理
////////////////////////////////////////////////////////////
void Collider_Sphere::BeginFrame(){
    // 前回のAABBを保存
    preBody_ = body_;

    Collider::BeginFrame();
}

////////////////////////////////////////////////////////////
// 衝突判定
////////////////////////////////////////////////////////////
void Collider_Sphere::CheckCollision(Collider* collider){

    // すでに衝突している場合は処理を行わない
    if(collisionList_.find(collider->GetColliderID()) != collisionList_.end()){ return; }

    CollisionData collisionData;

    switch(collider->GetColliderType()){
    case ColliderType::Sphere:
    {
        Collider_Sphere* sphere = dynamic_cast<Collider_Sphere*>(collider);
        collisionData = Collision::Sphere::Sphere(this, sphere);

        // 衝突した場合
        if(collisionData.isCollide){
            OnCollision(collider, collider->GetObjectType());
            collider->OnCollision(this, objectType_);

            // 押し戻しを行う
            PushBack(this, collider, collisionData);
        }
        break;
    }
    case ColliderType::AABB:
    {
        Collider_AABB* aabb = dynamic_cast<Collider_AABB*>(collider);
        if(IsMoved()){
            collisionData = Collision::Sphere::AABB(this, aabb);
        } else{
            collisionData = Collision::AABB::Sphere(aabb, this);
        }

        if(collisionData.isCollide){
            OnCollision(collider, collider->GetObjectType());
            collider->OnCollision(this, objectType_);

            // 押し戻しを行う
            PushBack(this, collider, collisionData);
        }
        break;
    }
    case ColliderType::OBB:
    {
        Collider_OBB* obb = dynamic_cast<Collider_OBB*>(collider);
        if(IsMoved()){
            collisionData = Collision::Sphere::OBB(this, obb);
        } else{
            collisionData = Collision::OBB::Sphere(obb, this);
        }

        if(collisionData.isCollide){
            OnCollision(collider, collider->GetObjectType());
            collider->OnCollision(this, objectType_);

            // 押し戻しを行う
            PushBack(this, collider, collisionData);
        }
        break;
    }
    case ColliderType::Line:
    {
        Collider_Line* line = dynamic_cast<Collider_Line*>(collider);
        if(Collision::Sphere::Line(body_, line->GetLine())){
            OnCollision(collider, collider->GetObjectType());
            collider->OnCollision(this, objectType_);
        }
        break;
    }
    }
    if(collisionData.isCollide){
        collidedPosition_ = collider->GetWoarldTranslate();
    }
}


//////////////////////////////////////////////////////////////////
// トンネリングを考慮しないtrue or falseの衝突判定
//////////////////////////////////////////////////////////////////
bool Collider_Sphere::CheckCollision(const AABB& aabb){
    return Collision::Sphere::AABB(body_, aabb);
}

bool Collider_Sphere::CheckCollision(const OBB& obb){
    return Collision::Sphere::OBB(body_, obb);
}

bool Collider_Sphere::CheckCollision(const Line& line){
    return Collision::Sphere::Line(body_, line);
}

bool Collider_Sphere::CheckCollision(const Sphere& sphere){
    return Collision::Sphere::Sphere(body_, sphere);
}

////////////////////////////////////////////////////////////
// 八分木用のAABB更新
////////////////////////////////////////////////////////////
void Collider_Sphere::UpdateBox(){

    AABB aabb[2] = {
        AABB(body_.center,{body_.radius,body_.radius,body_.radius}),
        AABB(preBody_.center,{preBody_.radius,preBody_.radius,preBody_.radius})
    };

    coverAABB_ = MaxAABB(aabb[0], aabb[1]);
}


////////////////////////////////////////////////////////////
// ImGuiでのパラメーター編集
////////////////////////////////////////////////////////////
void Collider_Sphere::Edit(){
#ifdef _DEBUG

    std::string colliderID = "##" + std::to_string(colliderID_);// コライダーID
    color_ = { 1.0f,1.0f,0.0f,1.0f };// 編集中のコライダーの色(黄色)

    // 全般情報
    Collider::Edit();

    // 中心座標
    ImGui::Text("------ Center ------");
    ImGui::Indent();
    ImGui::DragFloat3(std::string("Center" + colliderID).c_str(), &local_.center.x, 0.1f);
    ImGui::Unindent();

    ImGui::Dummy(ImVec2(0.0f, 10.0f));

    // 半径
    ImGui::Text("------ Radius ------");
    ImGui::Indent();
    ImGui::DragFloat(std::string("Radius" + colliderID).c_str(), &body_.radius, 0.05f, 0.0f);
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

////////////////////////////////////////////////////////////
// コライダーの情報をjson形式でまとめる
////////////////////////////////////////////////////////////
nlohmann::json Collider_Sphere::GetJsonData(){
    nlohmann::json json = Collider::GetJsonData();

    // コライダーの種類
    json["colliderType"] = "Sphere";

    // 全般の情報
    json.merge_patch(Collider::GetJsonData());

    // 球の情報
    json["center"] = local_.center;
    json["radius"] = body_.radius;

    return json;
}

////////////////////////////////////////////////////////////
// jsonデータから読み込み
////////////////////////////////////////////////////////////
void Collider_Sphere::LoadFromJson(const nlohmann::json& jsonData){
    // 全般情報の読み込み
    Collider::LoadFromJson(jsonData);

    // 球の情報
    local_.center = jsonData["center"];
    body_.radius = jsonData["radius"];

    // 行列の更新
    UpdateMatrix();
}

////////////////////////////////////////////////////////////
// 移動したかどうか
////////////////////////////////////////////////////////////
bool Collider_Sphere::IsMoved(){
    Sphere sphere[2] = {
        GetSphere(),
        GetPreSphere()
    };

    if(sphere[0].center != sphere[1].center or sphere[0].radius != sphere[1].radius){
        return true;
    }

    return false;
}
