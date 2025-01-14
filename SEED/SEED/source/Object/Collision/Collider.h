#pragma once
#include <cstdint>
#include <vector>
#include <unordered_set>
#include <json.hpp>
#include <Physics.h>
#include "ImGuiManager/ImGuiManager.h"
#include "Vector3.h"
#include "Matrix4x4.h"
#include "ObjectType.h"
#include "AABB.h"
#include "OBB.h"
#include "Line.h"
#include "Quad.h"
#include "Sphere.h"
#include "Capsule.h"
#include "ColliderAnimationData.h"
#include "CollisionManaer/CollisionData.h"

// 前方宣言
class BaseObject;

// コライダーの種類
enum class ColliderType : uint32_t{
    Sphere = 0,
    AABB,
    OBB,
    Line,
    Capsule,
    Plane
};

// コライダーの基底クラス
class Collider{
    friend class ColliderEditor;
public:// 基本関数--------------------------------------------------------------
    Collider();
    virtual ~Collider();

public:
    virtual void Update();
    virtual void UpdateMatrix();
    virtual void Draw();
    virtual void BeginFrame();
    virtual void CheckCollision(Collider* collider);
    virtual bool CheckCollision(const AABB& aabb);
    virtual bool CheckCollision(const OBB& obb);
    virtual bool CheckCollision(const Line& line);
    virtual bool CheckCollision(const Sphere& sphere);
    virtual void OnCollision(Collider* collider, ObjectType objectType);

protected:
    virtual void UpdateBox();

public:// 編集用関数--------------------------------------------------------------
    virtual void Edit();
    virtual nlohmann::json GetJsonData();
    virtual void LoadFromJson(const nlohmann::json& jsonData);

protected:
    void EditAnimation();

public:// アクセッサ--------------------------------------------------------------

    // 基礎情報
    void SetParentObject(BaseObject* parentObject){ parentObject_ = parentObject; }
    BaseObject* GetParentObject()const{ return parentObject_; }
    void SetObjectType(ObjectType objectType){ objectType_ = objectType; }
    ObjectType GetObjectType()const{ return objectType_; }
    ColliderType GetColliderType()const{ return colliderType_; }
    uint32_t GetColliderID()const{ return colliderID_; }
    void SetColliderID(uint32_t colliderID){ colliderID_ = colliderID; }

    // ペアレント情報
    void SetParentMatrix(const Matrix4x4* parentMat, bool isParentScale = true){
        parentMat_ = parentMat;
        isParentScale_ = isParentScale;
        if(animationData_){
            animationData_->SetParentMat(parentMat_);
        }
    }
    void SetIsParentRotate(bool isParentRotate){ isParentRotate_ = isParentRotate; }
    void SetIsParentScale(bool isParentScale){ isParentScale_ = isParentScale; }
    void SetIsParentTranslate(bool isParentTranslate){ isParentTranslate_ = isParentTranslate; }

    // ワールド、ローカル行列
    Matrix4x4 GetWorldMat()const{ return worldMat_; }
    Matrix4x4 GetLocalMat()const{ return localMat_; }

    // トランスフォーム取得
    Vector3 GetWoarldTranslate()const{ return ExtractTranslation(worldMat_) + offset_; }
    Vector3 GetLocalTranslate()const{ return ExtractTranslation(localMat_) + offset_; }
    void AddTranslate(const Vector3& addValue){ translate_ += addValue; }
    Vector3 GetWorldRotate()const{ return ExtractRotation(worldMat_); }
    Vector3 GetLocalRotate()const{ return ExtractRotation(localMat_); }
    Vector3 GetWorldScale()const{ return ExtractScale(worldMat_); }
    Vector3 GetLocalScale()const{ return ExtractScale(localMat_); }
    virtual bool IsMoved();

    // 衝突判定八分木用
    const AABB& GetBox()const{ return coverAABB_; }

    // アニメーション
    bool IsEndAnimation()const{
        if(animationData_ == std::nullopt){ return false; }
        return animationTime_ >= animationData_->GetDuration() && !isLoop_;
    }

protected:// 基礎情報--------------------------------------------------------------
    BaseObject* parentObject_ = nullptr;
    ColliderType colliderType_;
    ObjectType objectType_ = ObjectType::None;
    static uint32_t nextID_;
    uint32_t colliderID_;
    bool isCollision_ = false;
    bool preIsCollision_ = false;
    Vector4 color_;

protected:// 親子付け情報-----------------------------------------------------------
    const Matrix4x4* parentMat_ = nullptr;
    bool isParentRotate_ = true;
    bool isParentScale_ = true;
    bool isParentTranslate_ = true;

public:// 物理パラメータ----------------------------------------------------------
    bool isMovable_ = true;
    float mass_ = 1.0f;
    float miu_;
    bool isGhost_ = false;// 当たり判定のみ行い押し戻ししない場合true

public:// トランスフォーム情報------------------------------------------------------
    Vector3 scale_{ 1.0f,1.0f,1.0f };
    Vector3 rotate_;
    Vector3 translate_;
    Vector3 offset_;

protected:
    Matrix4x4 localMat_;
    Matrix4x4 worldMat_;

protected:// 衝突に使用するパラメータ--------------------------------------------------
    AABB coverAABB_;// 頂点を包含するAABB
    std::unordered_set<uint32_t> collisionList_;// 今のフレームですでに衝突したオブジェクトのIDリスト

protected:// コライダーが動く場合のアニメーションデータ-----------------------------------
    bool isAnimation_ = false;
    std::optional<ColliderAnimationData> animationData_ = std::nullopt;
    float animationTime_ = 0.0f;
    bool isLoop_ = false;
    bool isEditorLoop_ = true;
    bool isEdit_ = false;
};