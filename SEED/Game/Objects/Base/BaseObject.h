#pragma once
#include "Model.h"
#include <memory>
#include <cstdint>
#include <string>
#include <algorithm>
#include <Physics.h>
#include "Collision/Collider.h"
#include "CollisionManaer/CollisionManager.h"
#include "CollisionManaer/ColliderEditor.h"
#include "ClockManager.h"
#include "MyMath.h"
#include "MyFunc.h"

class BaseObject{
public:
    BaseObject();
    virtual ~BaseObject();
    virtual void Initialize();
    virtual void Update();
    virtual void Draw();
    virtual void BeginFrame();
    virtual void EndFrame();

public:
    void UpdateMatrix();
    void EditCollider();

protected:
    void EndFrameDropFlagUpdate();
    void MoveByVelocity();
public:// アクセッサ

    // 基礎情報
    uint32_t GetObjectID() const{ return objectID_; }
    ObjectType GetObjectType() const{ return objectType_; }
    std::string GetName() const{ return name_; }
    void SetName(const std::string& name){ name_ = name; }

    // トランスフォーム
    const Vector3& GetLocalTranslate() const{ return model_->translate_; }
    Vector3 GetWorldTranslate() const{ return model_->GetWorldTranslate(); }
    void AddWorldTranslate(const Vector3& addValue);
    const Vector3& GetLocalRotate() const{ return model_->rotate_; }
    Vector3 GetWorldRotate() const{ return model_->GetWorldRotate(); }
    const Vector3& GetLocalScale() const{ return model_->scale_; }
    Vector3 GetWorldScale() const{ return model_->GetWorldScale(); }
    Matrix4x4 GetLocalMat() const{ return model_->GetLocalMat(); }
    Matrix4x4 GetWorldMat() const{ return model_->GetWorldMat(); }
    const Matrix4x4* GetWorldMatPtr() const{ return model_->GetWorldMatPtr(); }
    void SetParent(const Model* parent){ model_->parent_ = parent; }
    void SetParent(const BaseObject* parent){ model_->parent_ = parent->model_.get(); }
    const Model* GetParent() const{ return model_->parent_; }
    Vector3 GetTargetPos()const{ return GetWorldTranslate() + targetOffset_; }

    void SetScale(const Vector3& scale){ model_->scale_ = scale; }
    void SetScaleX(float x){ model_->scale_.x = x; }
    void SetScaleY(float y){ model_->scale_.y = y; }
    void SetScaleZ(float z){ model_->scale_.z = z; }

    void SetRotateWithQuaternion(bool isRotateWithQuaternion){ model_->isRotateWithQuaternion_ = isRotateWithQuaternion; }
    void SetRotate(const Vector3& rotate){ model_->rotate_ = rotate; }
    void SetRotate(const Quaternion& rotate){ model_->rotateQuat_ = rotate; }
    void SetRotateX(float x){ model_->rotate_.x = x; }
    void SetRotateY(float y){ model_->rotate_.y = y; }
    void SetRotateZ(float z){ model_->rotate_.z = z; }

    void SetTranslate(const Vector3& translate){ model_->translate_ = translate; }
    void SetTranslateX(float x){ model_->translate_.x = x; }
    void SetTranslateY(float y){ model_->translate_.y = y; }
    void SetTranslateZ(float z){ model_->translate_.z = z; }
    void AddTranslate(const Vector3& addValue){ model_->translate_ += addValue; }

    // マテリアル
    void SetColor(const Vector4& color){ model_->color_ = color; }

    // 物理
    void SetIsApplyGravity(bool isApplyGravity){ isApplyGravity_ = isApplyGravity; }
    bool GetIsApplyGravity()const{ return isApplyGravity_; }
    void SetIsDrop(bool isDrop){ isDrop_ = isDrop; }
    float GetDropSpeed()const{ return dropSpeed_; }
    bool GetIsDrop()const{ return isDrop_; }

    // 重さ
    float GetWeight()const{ return weight_; }
    void SetWeight(float weight){ weight_ = weight; }

    // 速度
    Vector3 GetVelocity()const{ return velocity_; }
    void SetVelocity(const Vector3& velocity){ velocity_ = velocity; }
    void SetVelocityX(float x){ velocity_.x = x; }
    void SetVelocityY(float y){ velocity_.y = y; }
    void SetVelocityZ(float z){ velocity_.z = z; }

public:// コライダー関連
    void AddCollider(Collider* collider);
    void ResetCollider();
    virtual void HandOverColliders();
    virtual void OnCollision(const BaseObject* other,ObjectType objectType);
    std::vector<std::unique_ptr<Collider>>& GetColliders(){ return colliders_; }
    virtual void DiscardPreCollider();

protected:
    void LoadColliders(ObjectType objectType);
    virtual void InitColliders(ObjectType objectType);
    void EraseCheckColliders();

protected:
    static uint32_t nextID_;
    uint32_t objectID_;
    ObjectType objectType_;
    std::string className_;
    std::string name_;

protected:// 衝突判定用
    std::vector<std::unique_ptr<Collider>> colliders_;

protected:// 物理
    bool isApplyGravity_ = true;
    bool isDrop_ = false;
    float dropSpeed_ = 0.f;
    // 重さ
    float weight_ = 1.f;
    Vector3 velocity_ = {0.f,0.f,0.f};

protected:
    Vector3 targetOffset_;

protected:// Colliderの作成用メンバー
    std::unique_ptr<ColliderEditor> colliderEditor_;

protected:
    std::unique_ptr<Model> model_;
};