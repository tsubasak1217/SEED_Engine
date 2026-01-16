#include "Collider_Circle.h"
#include <SEED/Source/Basic/Object/GameObject.h>
#include <SEED/Source/Basic/Collision/2D/Collider_Quad2D.h>
#include <SEED/Source/Manager/CollisionManager/Collision.h>
#include <SEED/Source/SEED.h>

namespace SEED{
    ////////////////////////////////////////////////////////////
    // コンストラクタ・デストラクタ
    ////////////////////////////////////////////////////////////
    Collider_Circle::Collider_Circle() : Collider2D(){
        colliderType_ = ColliderType2D::Circle;
    }

    Collider_Circle::~Collider_Circle(){}

    ////////////////////////////////////////////////////////////
    // 行列の更新
    ////////////////////////////////////////////////////////////
    void Collider_Circle::UpdateMatrix(){

        // 行列の更新
        Collider2D::UpdateMatrix();
        // 本体の更新
        body_.center = local_.center * worldMat_ + offset_;

        if(ownerObject_){
            Vector2 parentScale = ownerObject_->GetWorldScale();
            float averageScale = (parentScale.x + parentScale.y) / 2.0f;
            body_.radius = local_.radius * averageScale;
        }

        // 八分木用のAABB更新
        UpdateBox();

    }

    ////////////////////////////////////////////////////////////
    // 描画
    ////////////////////////////////////////////////////////////
    void Collider_Circle::Draw(){
        //SEED::DrawSphere(body_.center, body_.radius, 6, color_);
    }


    ////////////////////////////////////////////////////////////
    // フレーム開始時処理
    ////////////////////////////////////////////////////////////
    void Collider_Circle::BeginFrame(){
        // 前回のAABBを保存
        preBody_ = body_;

        Collider2D::BeginFrame();
    }

    ////////////////////////////////////////////////////////////
    // 衝突判定
    ////////////////////////////////////////////////////////////
    void Collider_Circle::CheckCollision(Collider2D* collider){

        // すでに衝突している場合は処理を行わない
        if(collisionList_.find(collider->GetColliderID()) != collisionList_.end()){ return; }

        CollisionData2D collisionData;

        switch(collider->GetColliderType()){
        case ColliderType2D::Circle:
        {
            Collider_Circle* circle = dynamic_cast<Collider_Circle*>(collider);
            collisionData = Collision::Circle::Circle(this, circle);

            // 衝突した場合
            if(collisionData.isCollide){
                // 押し戻しを行う
                PushBack(this, collider, collisionData);

                // 衝突時の処理
                OnCollision(collider, collider->GetObjectType());
                collider->OnCollision(this, objectType_);
            }
            break;
        }
        case ColliderType2D::Quad:
        {
            //Collider_Quad2D* aabb = dynamic_cast<Collider_Quad2D*>(collider);
            //if(IsMoved()){
            //    collisionData = Collision::Sphere::AABB(this, aabb);
            //} else{
            //    collisionData = Collision::AABB::Sphere(aabb, this);
            //}

            //if(collisionData.isCollide){
            //    // 押し戻しを行う
            //    PushBack(this, collider, collisionData);

            //    // 衝突時の処理
            //    OnCollision(collider, collider->GetObjectType());
            //    collider->OnCollision(this, objectType_);
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
    bool Collider_Circle::CheckCollision(const Topology::Circle& aabb){
        aabb;
        return false;//Collision::Sphere::AABB(body_, aabb);
    }

    bool Collider_Circle::CheckCollision(const Topology::Quad2D& obb){
        obb;
        return false;// Collision::Sphere::OBB(body_, obb);
    }

    ////////////////////////////////////////////////////////////
    // 八分木用のAABB更新
    ////////////////////////////////////////////////////////////
    void Collider_Circle::UpdateBox(){

        Topology::AABB2D aabb[2] = {
            Topology::AABB2D(body_.center,{body_.radius,body_.radius}),
            Topology::AABB2D(preBody_.center,{preBody_.radius,preBody_.radius})
        };

        coverAABB_ = Methods::Shape::MaxAABB(aabb[0], aabb[1]);
    }


    ////////////////////////////////////////////////////////////
    // 前回のコライダーを破棄
    ////////////////////////////////////////////////////////////
    void Collider_Circle::DiscardPreCollider(){
        UpdateMatrix();
    }

    ////////////////////////////////////////////////////////////
    // ImGuiでのパラメーター編集
    ////////////////////////////////////////////////////////////
    void Collider_Circle::Edit(){
    #ifdef _DEBUG

        std::string colliderID = "##" + std::to_string(colliderID_);// コライダーID
        color_ = { 1.0f,1.0f,0.0f,1.0f };// 編集中のコライダーの色(黄色)

        // 全般情報
        Collider2D::Edit();

        // 中心座標
        ImGui::Text("------ Center ------");
        ImGui::Indent();
        ImGui::DragFloat2(std::string("Center" + colliderID).c_str(), &local_.center.x, 0.1f);
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
        ImGui::DragFloat2(std::string("Offset" + colliderID).c_str(), &offset_.x, 0.1f);
        ImGui::Unindent();

    #endif // _DEBUG
    }

    ////////////////////////////////////////////////////////////
    // コライダーの情報をjson形式でまとめる
    ////////////////////////////////////////////////////////////
    nlohmann::json Collider_Circle::GetJsonData(){
        nlohmann::json json = Collider2D::GetJsonData();

        // コライダーの種類
        json["colliderType"] = "Sphere";

        // 全般の情報
        json.merge_patch(Collider2D::GetJsonData());

        // 球の情報
        json["center"] = local_.center;
        json["radius"] = body_.radius;

        return json;
    }

    ////////////////////////////////////////////////////////////
    // jsonデータから読み込み
    ////////////////////////////////////////////////////////////
    void Collider_Circle::LoadFromJson(const nlohmann::json& jsonData){
        // 全般情報の読み込み
        Collider2D::LoadFromJson(jsonData);

        // 球の情報
        local_.center = jsonData["center"];
        local_.radius = jsonData["radius"];
        body_.radius = jsonData["radius"];

        // 行列の更新
        UpdateMatrix();
    }

    ////////////////////////////////////////////////////////////
    // 移動したかどうか
    ////////////////////////////////////////////////////////////
    bool Collider_Circle::IsMoved(){
        Topology::Circle circle[2] = {
            GetCircle(),
            GetPreCircle()
        };

        if(circle[0].center != circle[1].center or circle[0].radius != circle[1].radius){
            return true;
        }

        return false;
    }
}