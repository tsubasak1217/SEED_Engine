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
