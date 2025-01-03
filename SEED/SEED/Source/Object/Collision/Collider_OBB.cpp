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
            OnCollision(collider);
            collider->OnCollision(this);
        }
        break;
    }
    case ColliderType::AABB:
    {
        Collider_AABB* aabb = dynamic_cast<Collider_AABB*>(collider);
        if(Collision::OBB::AABB(body_, aabb->GetAABB())){
            OnCollision(collider);
            collider->OnCollision(this);
        }
        break;
    }
    case ColliderType::OBB:
    {
        Collider_OBB* obb = dynamic_cast<Collider_OBB*>(collider);
        if(Collision::OBB::OBB(body_, obb->GetOBB())){
            OnCollision(collider);
            collider->OnCollision(this);
        }
        break;
    }
    case ColliderType::Line:
    {
        Collider_Line* line = dynamic_cast<Collider_Line*>(collider);
        if(Collision::OBB::Line(body_, line->GetLine())){
            OnCollision(collider);
            collider->OnCollision(this);
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
