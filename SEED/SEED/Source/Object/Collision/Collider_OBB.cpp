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
// コンストラクタ・デストラクタ
////////////////////////////////////////////////////////////////
Collider_OBB::Collider_OBB() : Collider(){
    colliderType_ = ColliderType::OBB;
}

Collider_OBB::~Collider_OBB(){
}

////////////////////////////////////////////////////////////////
// 行列の更新
////////////////////////////////////////////////////////////////
void Collider_OBB::UpdateMatrix(){
    // 行列の更新
    Collider::UpdateMatrix();

    // 本体の更新
    body_.center = local_.center * worldMat_ + offset_;
    body_.halfSize = local_.halfSize * GetWorldScale();
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
// フレーム開始時処理
////////////////////////////////////////////////////////////////
void Collider_OBB::BeginFrame(){
    // 前回のAABBを保存
    preBody_ = body_;

    Collider::BeginFrame();
}

////////////////////////////////////////////////////////////////
// 衝突判定
////////////////////////////////////////////////////////////////
void Collider_OBB::CheckCollision(Collider* collider){

    // すでに衝突している場合は処理を行わない
    if(collisionList_.find(collider->GetColliderID()) != collisionList_.end()){ return; }
    CollisionData collisionData;

    switch(collider->GetColliderType()){
    case ColliderType::Sphere:
    {
        Collider_Sphere* sphere = dynamic_cast<Collider_Sphere*>(collider);
        if(IsMoved()){
            collisionData = Collision::OBB::Sphere(this, sphere);
        } else{
            collisionData = Collision::Sphere::OBB(sphere, this);
        }

        if(collisionData.isCollide){
            OnCollision(collider,collider->GetObjectType());
            collider->OnCollision(this,objectType_);

            // 押し戻しを行う
            PushBack(this, collider, collisionData);
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
    if(collisionData.isCollide){
        collidedPosition_ = collider->GetWoarldTranslate();
        collisionList_.insert(collider->GetColliderID());
    }
}


//////////////////////////////////////////////////////////////////
// トンネリングを考慮しないtrue or falseの衝突判定
//////////////////////////////////////////////////////////////////
bool Collider_OBB::CheckCollision(const AABB& aabb){
    return Collision::OBB::AABB(body_, aabb);
}

bool Collider_OBB::CheckCollision(const OBB& obb){
    return Collision::OBB::OBB(body_, obb);
}

bool Collider_OBB::CheckCollision(const Line& line){
    return Collision::OBB::Line(body_, line);
}

bool Collider_OBB::CheckCollision(const Sphere& sphere){
    return Collision::OBB::Sphere(body_, sphere);
}

////////////////////////////////////////////////////////////////
// 八分木用のAABB更新
////////////////////////////////////////////////////////////////
void Collider_OBB::UpdateBox(){
    
    AABB aabb[2];

    float maxLen[2] = {
        (std::max)({body_.halfSize.x, body_.halfSize.y, body_.halfSize.z}),
        (std::max)({preBody_.halfSize.x, preBody_.halfSize.y, preBody_.halfSize.z})
    };
    
    aabb[0].center = body_.center;
    aabb[0].halfSize = { maxLen[0],maxLen[0],maxLen[0] };

    aabb[1].center = preBody_.center;
    aabb[1].halfSize = { maxLen[1],maxLen[1],maxLen[1] };

    coverAABB_ = MaxAABB(aabb[0], aabb[1]);
}

////////////////////////////////////////////////////////////////
// ImGuiでの編集
////////////////////////////////////////////////////////////////
void Collider_OBB::Edit(){
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
    ImGui::DragFloat3(std::string("HalfSie" + colliderID).c_str(), &local_.halfSize.x, 0.025f);
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
    local_.halfSize = body_.halfSize;

    // オフセット
    offset_ = jsonData["offset"];

    // 行列の更新
    UpdateMatrix();
}


////////////////////////////////////////////////////////////////
// 移動したかどうか
////////////////////////////////////////////////////////////////
bool Collider_OBB::IsMoved(){
    OBB obb[2] = {
        GetOBB(),
        GetPreOBB()
    };

    if(obb[0].center != obb[1].center or obb[0].halfSize != obb[1].halfSize or obb[0].rotate != obb[1].rotate){
        return true;
    }

    return false;
}
