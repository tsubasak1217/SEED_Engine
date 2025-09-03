#include "Collider_Quad2D.h"
#include <SEED/Source/Basic/Object/GameObject2D.h>
#include <SEED/Source/Basic/Collision/2D/Collider_Circle.h>

#include <SEED/Source/Manager/CollisionManager/Collision.h>
#include <SEED/Source/SEED.h>

////////////////////////////////////////////////////////////////
// コンストラクタ・デストラクタ
////////////////////////////////////////////////////////////////
Collider_Quad2D::Collider_Quad2D() : Collider2D(){
    colliderType_ = ColliderType2D::Quad;
}

Collider_Quad2D::~Collider_Quad2D(){
}

////////////////////////////////////////////////////////////////
// 行列の更新
////////////////////////////////////////////////////////////////
void Collider_Quad2D::UpdateMatrix(){
    // 行列の更新
    Collider2D::UpdateMatrix();

    // 本体の更新
    body_.translate = local_.translate * worldMat_ + offset_;
    body_.rotate = ExtractRotation(worldMat_);

    // 八分木用のAABB更新
    UpdateBox();
}

////////////////////////////////////////////////////////////////
// 描画
////////////////////////////////////////////////////////////////
void Collider_Quad2D::Draw(){
    SEED::DrawQuad2D(body_);
}

////////////////////////////////////////////////////////////////
// フレーム開始時処理
////////////////////////////////////////////////////////////////
void Collider_Quad2D::BeginFrame(){
    // 前回のAABBを保存
    preBody_ = body_;

    Collider2D::BeginFrame();
}

////////////////////////////////////////////////////////////////
// 衝突判定
////////////////////////////////////////////////////////////////
void Collider_Quad2D::CheckCollision(Collider2D* collider){

    // すでに衝突している場合は処理を行わない
    if(collisionList_.find(collider->GetColliderID()) != collisionList_.end()){ return; }
    CollisionData2D collisionData;

    switch(collider->GetColliderType()){
    case ColliderType2D::Circle:
    {
        //Collider_Sphere* sphere = dynamic_cast<Collider_Sphere*>(collider);
        //if(IsMoved()){
        //    collisionData = Collision::OBB::Sphere(this, sphere);
        //} else{
        //    collisionData = Collision::Sphere::OBB(sphere, this);
        //}

        //if(collisionData.isCollide){
        //    // 押し戻しを行う
        //    PushBack(this, collider, collisionData);

        //    // 衝突時の処理
        //    OnCollision(collider,collider->GetObjectType());
        //    collider->OnCollision(this,objectType_);
        //}
        break;
    }
    case ColliderType2D::Quad:
    {
        //Collider_AABB* aabb = dynamic_cast<Collider_AABB*>(collider);
        //if(Collision::OBB::AABB(body_, aabb->GetAABB())){
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
bool Collider_Quad2D::CheckCollision(const Circle& circle){
    circle;
    return false;//Collision::Quad2D::Circle(body_, circle);
}

bool Collider_Quad2D::CheckCollision(const Quad2D& quad){
    quad;
    return false;// Collision::Quad2D::Quad2D(body_, quad);
}

////////////////////////////////////////////////////////////////
// 八分木用のAABB更新
////////////////////////////////////////////////////////////////
void Collider_Quad2D::UpdateBox(){
    
    AABB2D aabb[2];

    //float maxLen[2] = {
    //    (std::max)({body_.halfSize.x, body_.halfSize.y}),
    //    (std::max)({preBody_.halfSize.x, preBody_.halfSize.y})
    //};
    //
    //aabb[0].center = body_.center;
    //aabb[0].halfSize = { maxLen[0],maxLen[0],maxLen[0] };

    //aabb[1].center = preBody_.center;
    //aabb[1].halfSize = { maxLen[1],maxLen[1],maxLen[1] };

    coverAABB_ = MaxAABB(aabb[0], aabb[1]);
}

////////////////////////////////////////////////////////////////
// 前回のコライダーを破棄
////////////////////////////////////////////////////////////////
void Collider_Quad2D::DiscardPreCollider(){
    UpdateMatrix();
}

////////////////////////////////////////////////////////////////
// ImGuiでの編集
////////////////////////////////////////////////////////////////
void Collider_Quad2D::Edit(){
#ifdef _DEBUG

    std::string colliderID = "##" + std::to_string(colliderID_);// コライダーID
    color_ = { 1.0f,1.0f,0.0f,1.0f };// 編集中のコライダーの色(黄色)

    // 全般情報
    Collider2D::Edit();

    // 中心座標
    ImGui::Text("------ Center ------");
    ImGui::Indent();
    ImGui::DragFloat2(std::string("Center" + colliderID).c_str(), &local_.translate.x, 0.1f);
    ImGui::Unindent();

    ImGui::Dummy(ImVec2(0.0f, 10.0f));

    // 回転
    ImGui::Text("------ Rotate ------");
    ImGui::Indent();
    ImGui::DragFloat(std::string("Rotate" + colliderID).c_str(), &rotate_, 0.1f);
    ImGui::Unindent();

    ImGui::Dummy(ImVec2(0.0f, 10.0f));

    // オフセット
    ImGui::Text("------ Offset ------");
    ImGui::Indent();
    ImGui::DragFloat3(std::string("Offset" + colliderID).c_str(), &offset_.x, 0.1f);
    ImGui::Unindent();

#endif // _DEBUG
}


////////////////////////////////////////////////////////////////
// コライダーの情報をjson形式でまとめる
////////////////////////////////////////////////////////////////
nlohmann::json Collider_Quad2D::GetJsonData(){
    nlohmann::json json;

    // 全般の情報
    json.merge_patch(Collider2D::GetJsonData());

    // コライダーの種類
    json["colliderType"] = "Quad2D";

    // ローカル座標
    json["local"]["center"] = local_.translate;

    // オフセット
    json["offset"] = offset_;

    return json;
}

////////////////////////////////////////////////////////////////
// jsonデータからコライダーの情報を読み込む
////////////////////////////////////////////////////////////////
void Collider_Quad2D::LoadFromJson(const nlohmann::json& jsonData){
    // 全般の情報
    Collider2D::LoadFromJson(jsonData);

    // ローカル座標
    local_.translate = jsonData["local"]["center"];

    // オフセット
    offset_ = jsonData["offset"];

    // 行列の更新
    UpdateMatrix();
}


////////////////////////////////////////////////////////////////
// 移動したかどうか
////////////////////////////////////////////////////////////////
bool Collider_Quad2D::IsMoved(){
    //OBB obb[2] = {
    //    GetOBB(),
    //    GetPreOBB()
    //};

    //if(obb[0].center != obb[1].center or obb[0].halfSize != obb[1].halfSize or obb[0].rotate != obb[1].rotate){
    //    return true;
    //}

    return false;
}
