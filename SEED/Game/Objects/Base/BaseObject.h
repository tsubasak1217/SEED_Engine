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
    void Drop();
    void UpdateMatrix();
    void EditCollider();

public:// アクセッサ

    // 基礎情報
    uint32_t GetObjectID() const{ return objectID_; }
    ObjectType GetObjectType() const{ return objectType_; }
    std::string GetName() const{ return name_; }
    void SetName(const std::string& name){ name_ = name; }

    // トランスフォーム
    Vector3 GetLocalTranslate() const{ return model_->translate_; }
    Vector3 GetWorldTranslate() const{ return model_->GetWorldTranslate(); }
    void AddWorldTranslate(const Vector3& addValue);
    Vector3 GetLocalRotate() const{ return model_->rotate_; }
    Vector3 GetWorldRotate() const{ return model_->GetWorldRotate(); }
    Vector3 GetLocalScale() const{ return model_->scale_; }
    Vector3 GetWorldScale() const{ return model_->GetWorldScale(); }
    Matrix4x4 GetLocalMat() const{ return model_->GetLocalMat(); }
    Matrix4x4 GetWorldMat() const{ return model_->GetWorldMat(); }
    const Matrix4x4* GetWorldMatPtr() const{ return model_->GetWorldMatPtr(); }
    void SetParent(const Model* parent){ model_->parent_ = parent; }
    void SetParent(const BaseObject* parent){ model_->parent_ = parent->model_.get(); }
    const Model* GetParent() const{ return model_->parent_; }
    Vector3 GetTargetPos()const{ return GetWorldTranslate() + targetOffset_; }

    // 物理
    void SetIsApplyGravity(bool isApplyGravity){ isApplyGravity_ = isApplyGravity; }
    bool GetIsApplyGravity()const{ return isApplyGravity_; }
    void SetIsDrop(bool isDrop){ isDrop_ = isDrop; }
    bool GetIsDrop()const{ return isDrop_; }

public:// コライダー関連
    void AddCollider(Collider* collider);
    void ResetCollider();
    virtual void HandOverColliders();
    virtual void OnCollision(const BaseObject* other, ObjectType objectType);

protected:
    void LoadColliders(ObjectType objectType);
    virtual void InitColliders(ObjectType objectType);
    void UpdateColliders();

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
    float dropSpeed_ = 0.0f;

protected:
    Vector3 targetOffset_;

protected:// Colliderの作成用メンバー
    std::unique_ptr<ColliderEditor> colliderEditor_;

protected:
    std::unique_ptr<Model> model_;
};