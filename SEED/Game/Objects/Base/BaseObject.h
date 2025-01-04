#pragma once
#include "Model.h"
#include <memory>
#include <cstdint>
#include <string>
#include <algorithm>
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

public:
    void UpdateMatrix();

public:// アクセッサ

    // 基礎情報
    uint32_t GetObjectID() const{ return objectID_; }
    std::string GetName() const{ return name_; }
    void SetName(const std::string& name){ name_ = name; }

    // トランスフォーム
    Vector3 GetLocalTranslate() const{ return model_->translate_; }
    Vector3 GetWorldTranslate() const{ return model_->GetWorldTranslate(); }
    Vector3 GetLocalRotate() const{ return model_->rotate_; }
    Vector3 GetWorldRotate() const{ return model_->GetWorldRotate(); }
    Vector3 GetLocalScale() const{ return model_->scale_; }
    Vector3 GetWorldScale() const{ return model_->GetWorldScale(); }
    Matrix4x4 GetLocalMat() const{ return model_->GetLocalMat(); }
    Matrix4x4 GetWorldMat() const{ return model_->GetWorldMat(); }
    void SetParent(const Model* parent){ model_->parent_ = parent; }
    void SetParent(const BaseObject* parent){ model_->parent_ = parent->model_.get(); }
    Vector3 GetTargetPos()const{ return GetWorldTranslate() + targetOffset_; }

public:// コライダー関連
    void AddCollider(Collider* collider);
    void ResetCollider();
    virtual void HandOverColliders();
    virtual void OnCollision(const BaseObject* other){ other; }

private:
    virtual void InitCollider(){};

protected:
    static uint32_t nextID_;
    uint32_t objectID_;
    std::string className_;
    std::string name_;

protected:// 衝突判定用
    std::vector<std::unique_ptr<Collider>> colliders_;

protected:

    Vector3 targetOffset_;

protected:// Colliderの作成用メンバー
    std::unique_ptr<ColliderEditor> colliderEditor_;

protected:
    std::unique_ptr<Model> model_;
};