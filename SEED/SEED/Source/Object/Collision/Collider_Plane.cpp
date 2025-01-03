#include "Collider_Plane.h"
#include "Base/BaseObject.h"
#include "Collision/Collider_Sphere.h"
#include "Collision/Collider_AABB.h"
#include "Collision/Collider_OBB.h"
#include "Collision/Collider_Line.h"
#include "Collision/Collider_Capsule.h"
#include "Collision/Collider_Plane.h"
#include "CollisionManaer/Collision.h"
#include "SEED.h"

//////////////////////////////////////////////////////////////////
// コンストラクタ
//////////////////////////////////////////////////////////////////
Collider_Plane::Collider_Plane() : Collider(){
    colliderType_ = ColliderType::Plane;
}

//////////////////////////////////////////////////////////////////
// 行列の更新
//////////////////////////////////////////////////////////////////
void Collider_Plane::UpdateMatrix(){
    
    // ワールド行列の更新
    Collider::UpdateMatrix();

    // ローカル座標をワールド座標に変換
    for(int i = 0; i < 4; i++){
        body_.localVertex[i] = local_.localVertex[i] * worldMat_ + offset_;
    }

    // 八分木用のAABB更新
    UpdateBox();
}

//////////////////////////////////////////////////////////////////
// 描画
//////////////////////////////////////////////////////////////////
void Collider_Plane::Draw(){
    SEED::DrawLine(body_.localVertex[0], body_.localVertex[1], color_);
    SEED::DrawLine(body_.localVertex[1], body_.localVertex[2], color_);
    SEED::DrawLine(body_.localVertex[2], body_.localVertex[3], color_);
    SEED::DrawLine(body_.localVertex[3], body_.localVertex[0], color_);
}

//////////////////////////////////////////////////////////////////
// 衝突判定
//////////////////////////////////////////////////////////////////
void Collider_Plane::CheckCollision(Collider* collider){
    // Planeはまだ未実装
    collider;
}

//////////////////////////////////////////////////////////////////
// 八分木用のAABB更新
//////////////////////////////////////////////////////////////////
void Collider_Plane::UpdateBox(){
    Vector3 min;
    Vector3 max;

    min.x = (std::min)({ body_.localVertex[0].x, body_.localVertex[1].x, body_.localVertex[2].x, body_.localVertex[3].x });
    min.y = (std::min)({ body_.localVertex[0].y, body_.localVertex[1].y, body_.localVertex[2].y, body_.localVertex[3].y });
    min.z = (std::min)({ body_.localVertex[0].z, body_.localVertex[1].z, body_.localVertex[2].z, body_.localVertex[3].z });

    max.x = (std::max)({ body_.localVertex[0].x, body_.localVertex[1].x, body_.localVertex[2].x, body_.localVertex[3].x });
    max.y = (std::max)({ body_.localVertex[0].y, body_.localVertex[1].y, body_.localVertex[2].y, body_.localVertex[3].y });
    max.z = (std::max)({ body_.localVertex[0].z, body_.localVertex[1].z, body_.localVertex[2].z, body_.localVertex[3].z });

    coverAABB_.center = (min + max) * 0.5f;
    coverAABB_.halfSize = (max - min) * 0.5f;
}

//////////////////////////////////////////////////////////////////
// ローカル座標の設定
//////////////////////////////////////////////////////////////////
void Collider_Plane::SetLocalVertices(const Vector3& v0, const Vector3& v1, const Vector3& v2, const Vector3& v3){
    local_.localVertex[0] = v0;
    local_.localVertex[1] = v1;
    local_.localVertex[2] = v2;
    local_.localVertex[3] = v3;
}
