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
// コンストラクタ
////////////////////////////////////////////////////////////
Collider_Sphere::Collider_Sphere() : Collider(){
    colliderType_ = ColliderType::Sphere;
}

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
// 衝突判定
////////////////////////////////////////////////////////////
void Collider_Sphere::CheckCollision(Collider* collider){

    // すでに衝突している場合は処理を行わない
    if(collisionList_.find(collider->GetColliderID()) != collisionList_.end()){ return; }

    switch(collider->GetColliderType()){
    case ColliderType::Sphere:
    {
        Collider_Sphere* sphere = dynamic_cast<Collider_Sphere*>(collider);
        if(Collision::Sphere::Sphere(body_, sphere->GetSphere())){
            OnCollision(collider);
            collider->OnCollision(this);
        }
        break;
    }
    case ColliderType::AABB:
    {
        Collider_AABB* aabb = dynamic_cast<Collider_AABB*>(collider);
        if(Collision::Sphere::AABB(body_, aabb->GetAABB())){
            OnCollision(collider);
            collider->OnCollision(this);
        }
        break;
    }
    case ColliderType::OBB:
    {
        Collider_OBB* obb = dynamic_cast<Collider_OBB*>(collider);
        if(Collision::Sphere::OBB(body_, obb->GetOBB())){
            OnCollision(collider);
            collider->OnCollision(this);
        }
        break;
    }
    case ColliderType::Line:
    {
        Collider_Line* line = dynamic_cast<Collider_Line*>(collider);
        if(Collision::Sphere::Line(body_, line->GetLine())){
            OnCollision(collider);
            collider->OnCollision(this);
        }
        break;
    }
    }

}

////////////////////////////////////////////////////////////
// 八分木用のAABB更新
////////////////////////////////////////////////////////////
void Collider_Sphere::UpdateBox(){
    coverAABB_.center = body_.center;
    coverAABB_.halfSize = Vector3(body_.radius, body_.radius, body_.radius);
}
