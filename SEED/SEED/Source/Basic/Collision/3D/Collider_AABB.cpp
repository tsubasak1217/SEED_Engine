#include "Collider_AABB.h"
#include <SEED/Source/Basic/Object/GameObject.h>
#include <SEED/Source/Basic/Collision/3D/Collider_Sphere.h>
#include <SEED/Source/Basic/Collision/3D/Collider_OBB.h>
#include <SEED/Source/Basic/Collision/3D/Collider_Line.h>
#include <SEED/Source/Basic/Collision/3D/Collider_Capsule.h>
#include <SEED/Source/Basic/Collision/3D/Collider_Plane.h>
#include <SEED/Source/Manager/CollisionManager/Collision.h>
#include <SEED/Source/SEED.h>

//////////////////////////////////////////////////////////////////
// コンストラクタ・デストラクタ
//////////////////////////////////////////////////////////////////
Collider_AABB::Collider_AABB() : Collider(){
    colliderType_ = ColliderType::AABB;
}

Collider_AABB::~Collider_AABB(){
}


//////////////////////////////////////////////////////////////////
// 行列の更新
//////////////////////////////////////////////////////////////////
void Collider_AABB::UpdateMatrix(){
    // 行列の更新
    Collider::UpdateMatrix();

    // 本体の更新
    body_.center = local_.center * worldMat_ + offset_;

    // 八分木用のAABB更新
    UpdateBox();
}


//////////////////////////////////////////////////////////////////
// 描画
//////////////////////////////////////////////////////////////////
void Collider_AABB::Draw(){
    SEED::DrawAABB(body_, color_);
}


//////////////////////////////////////////////////////////////////
// フレーム開始時処理
//////////////////////////////////////////////////////////////////
void Collider_AABB::BeginFrame(){

    // 前回のAABBを保存
    preBody_ = body_;

    Collider::BeginFrame();
}


//////////////////////////////////////////////////////////////////
// 衝突判定
//////////////////////////////////////////////////////////////////
void Collider_AABB::CheckCollision(Collider* collider){

    // すでに衝突している場合は処理を行わない
    if(collisionList_.find(collider->GetColliderID()) != collisionList_.end()){ return; }

    CollisionData collisionData;
    
    switch(collider->GetColliderType()){
    case ColliderType::Sphere:
    {
        Collider_Sphere* sphere = dynamic_cast<Collider_Sphere*>(collider);
        if(IsMoved()){
            collisionData = Collision::AABB::Sphere(this, sphere);
        } else{
            collisionData = Collision::Sphere::AABB(sphere,this);
        }

        if(collisionData.isCollide){
            // 押し戻しを行う
            PushBack(this, collider, collisionData);

            // 衝突時の処理
            OnCollision(collider, collider->GetObjectType());
            collider->OnCollision(this, objectType_);
        }

        break;
    }
    case ColliderType::AABB:
    {
        Collider_AABB* aabb = dynamic_cast<Collider_AABB*>(collider);
        if(Collision::AABB::AABB(body_, aabb->GetAABB())){
            OnCollision(collider,collider->GetObjectType());
            collider->OnCollision(this,objectType_);
        }
        break;
    }
    case ColliderType::OBB:
    {
        Collider_OBB* obb = dynamic_cast<Collider_OBB*>(collider);
        if(Collision::AABB::OBB(body_, obb->GetOBB())){
            OnCollision(collider,collider->GetObjectType());
            collider->OnCollision(this,objectType_);
        }
        break;
    }
    case ColliderType::Line:
    {
        Collider_Line* line = dynamic_cast<Collider_Line*>(collider);
        if(Collision::AABB::Line(body_, line->GetLine())){
            OnCollision(collider,collider->GetObjectType());
            collider->OnCollision(this,objectType_);
        }
        break;
    }
    case ColliderType::Capsule:
    {
        //Collider_Capsule* capsule = dynamic_cast<Collider_Capsule*>(collider);
        //if(Collision::AABB::Capsule(body_, capsule->GetCapsule())){
        //    OnCollision(collider,collider->GetObjectType());
        //    collider->OnCollision(this,objectType_);
        //}
        break;
    }
    case ColliderType::Plane:
    {
        //Collider_Plane* plane = dynamic_cast<Collider_Plane*>(collider);
        //if(Collision::AABB::Plane(body_, plane->GetPlane())){
        //    OnCollision(collider,collider->GetObjectType());
        //    collider->OnCollision(this,objectType_);
        //}
        break;
    }
    }
    if(collisionData.isCollide){
        collidedPosition_ = collider->GetWoarldTranslate();
        collisionList_.insert(collider->GetColliderID());
    }
}


//////////////////////////////////////////////////////////////////
// トンネリングを考慮しないtrue or falseの衝突判定
//////////////////////////////////////////////////////////////////
bool Collider_AABB::CheckCollision(const AABB& aabb){
    return Collision::AABB::AABB(body_, aabb);
}

bool Collider_AABB::CheckCollision(const OBB& obb){
    return Collision::AABB::OBB(body_, obb);
}

bool Collider_AABB::CheckCollision(const Line& line){
    return Collision::AABB::Line(body_, line);
}

bool Collider_AABB::CheckCollision(const Sphere& sphere){
    return Collision::AABB::Sphere(body_, sphere);
}


//////////////////////////////////////////////////////////////////
// 八分木用のAABB更新
//////////////////////////////////////////////////////////////////
void Collider_AABB::UpdateBox(){

    Vector3 min = {
        (std::min)(body_.center.x - body_.halfSize.x, preBody_.center.x - preBody_.halfSize.x),
        (std::min)(body_.center.y - body_.halfSize.y, preBody_.center.y - preBody_.halfSize.y),
        (std::min)(body_.center.z - body_.halfSize.z, preBody_.center.z - preBody_.halfSize.z)
    };

    Vector3 max = {
        (std::max)(body_.center.x + body_.halfSize.x, preBody_.center.x + preBody_.halfSize.x),
        (std::max)(body_.center.y + body_.halfSize.y, preBody_.center.y + preBody_.halfSize.y),
        (std::max)(body_.center.z + body_.halfSize.z, preBody_.center.z + preBody_.halfSize.z)
    };

    coverAABB_.halfSize = (max - min) * 0.5f;
    coverAABB_.center = min + coverAABB_.halfSize;
}


//////////////////////////////////////////////////////////////////
// 前回のコライダーを破棄
//////////////////////////////////////////////////////////////////
void Collider_AABB::DiscardPreCollider(){
    UpdateMatrix();
}

//////////////////////////////////////////////////////////////////
// ImGuiでのパラメーター編集
//////////////////////////////////////////////////////////////////
void Collider_AABB::Edit(){
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
    ImGui::Text("------ HalfSie ------");
    ImGui::Indent();
    ImGui::DragFloat3(std::string("HalfSie" + colliderID).c_str(), &body_.halfSize.x, 0.025f);
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

//////////////////////////////////////////////////////////////////
// コライダーの情報をjson形式でまとめる
//////////////////////////////////////////////////////////////////
nlohmann::json Collider_AABB::GetJsonData(){
    nlohmann::json json;

    // 全般の情報
    json.merge_patch(Collider::GetJsonData());

    // コライダーの種類
    json["colliderType"] = "AABB";

    // ローカル座標
    json["local"]["center"] = local_.center;
    json["local"]["halfSize"] = body_.halfSize;

    // オフセット
    json["offset"] = offset_;

    return json;
}

//////////////////////////////////////////////////////////////////
// jsonデータから読み込み
//////////////////////////////////////////////////////////////////
void Collider_AABB::LoadFromJson(const nlohmann::json& jsonData){
    // 全般情報の読み込み
    Collider::LoadFromJson(jsonData);

    // ローカル座標
    local_.center = jsonData["local"]["center"];
    body_.halfSize = jsonData["local"]["halfSize"];

    // オフセット
    offset_ = jsonData["offset"];
}

//////////////////////////////////////////////////////////////////
// 移動したかどうか
//////////////////////////////////////////////////////////////////
bool Collider_AABB::IsMoved(){
    AABB aabb[2] = {
        GetAABB(),
        GetPreAABB()
    };

    if(aabb[0].center != aabb[1].center or aabb[0].halfSize != aabb[1].halfSize){
        return true;
    }

    return false;
}
