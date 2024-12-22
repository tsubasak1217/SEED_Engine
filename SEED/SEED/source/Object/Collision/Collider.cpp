#include "Collider.h"
#include "SEED.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//     初期化・終了関数
// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Collider::Collider(ColliderType colliderType){
    switch(colliderType){
    case ColliderType::Line:
        currentPoints_.resize(2);;
        break;
    case ColliderType::Capsule:
        currentPoints_.resize(2);
        break;
    case ColliderType::Plane:
        currentPoints_.resize(4);
        break;
    case ColliderType::Sphere:
        currentPoints_.resize(1);
        break;
    case ColliderType::AABB:
        currentPoints_.resize(1);
        break;
    case ColliderType::OBB:
        currentPoints_.resize(1);
        break;
    default:
        break;
    }

    prePoints_.resize(currentPoints_.size());
    this->colliderType_ = colliderType;
}

Collider::~Collider(){}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//     更新関数
// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Collider::Update(){
    UpdateMatrix();
}

void Collider::UpdateMatrix(){

    // ローカル行列の更新
    localMat_ = AffineMatrix(scale_, rotate_, translate_);

    // ワールド行列の更新
    if(parentMat_){

        if(isParentRotate_ + isParentScale_ + isParentTranslate_ == 3){
            worldMat_ = *parentMat_ * localMat_;
            return;
        } else{
            
            Matrix4x4 cancelMat = IdentityMat4();

            // 親の行列から取り出した要素を打ち消す行列を作成
            if(!isParentScale_){
                Vector3 inverseScale = Vector3(1.0f,1.0f,1.0f)/ExtractScale(*parentMat_);
                cancelMat = cancelMat * ScaleMatrix(inverseScale);
            }

            if(!isParentRotate_){
                Vector3 inverseRotate = ExtractRotation(*parentMat_) * -1.0f;
                cancelMat = cancelMat * RotateMatrix(inverseRotate);
            }

            if(!isParentTranslate_){
                Vector3 inverseTranslate = ExtractTranslation(*parentMat_) * -1.0f;
                cancelMat = cancelMat * TranslateMatrix(inverseTranslate);
            }

            worldMat_ = (*parentMat_ * cancelMat)* localMat_;

        }

    } else{
        worldMat_ = localMat_;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//     描画関数
// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Collider::Draw(){

    Vector4 color = isCollision_ ? 
        Vector4(1.0f, 0.0f, 0.0f, 1.0f) :
        Vector4(0.0f, 1.0f, 0.0f, 1.0f);

    switch(colliderType_){
    case ColliderType::Line:/*----------------------------------*/
    {
        SEED::DrawLine(currentPoints_[0], currentPoints_[1], color);
        break;
    }
    case ColliderType::Capsule:/*----------------------------------*/
    {
        SEED::DrawCapsule(currentPoints_[0], currentPoints_[1], scale_ * radius_, 8, color);
        break;
    }
    case ColliderType::Plane:/*----------------------------------*/
    {
        SEED::DrawLine(currentPoints_[0], currentPoints_[1], color);
        SEED::DrawLine(currentPoints_[1], currentPoints_[2], color);
        SEED::DrawLine(currentPoints_[2], currentPoints_[3], color);
        SEED::DrawLine(currentPoints_[3], currentPoints_[0], color);
        break;
    }
    case ColliderType::Sphere:/*----------------------------------*/
    {
        SEED::DrawSphere(currentPoints_[0], scale_ * radius_, 8, color);
        break;
    }
    case ColliderType::AABB:/*----------------------------------*/
    {
        AABB aabb;
        aabb.center = currentPoints_[0];
        aabb.halfSize = halfSize_ * scale_;
        SEED::DrawAABB(aabb, color);
        break;
    }
    case ColliderType::OBB:/*----------------------------------*/
    {
        OBB obb;
        obb.center = currentPoints_[0];
        obb.halfSize = halfSize_ * scale_;
        obb.rotate = rotate_;
        break;
    }
    default:
        break;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//     衝突時関数
// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Collider::OnCollision(Collider* collider){
    collider;
    isCollision_ = true;
}
