#pragma once
#include <cstdint>
#include <vector>
#include <unordered_set>
#include <json.hpp>
#include <Environment/Physics.h>
#include <SEED/Lib/Tensor/Vector3.h>
#include <SEED/Lib/Tensor/Matrix4x4.h>
#include <SEED/Source/Manager/ImGuiManager/ImGuiManager.h>
#include <Game/Objects/Enums/ObjectType.h>
#include <SEED/Lib/Shapes/Line.h>
#include <SEED/Lib/Shapes/Quad.h>
#include <SEED/Lib/Shapes/Circle.h>
#include <SEED/Lib/Shapes/AABB.h>
#include <SEED/Source/Basic/Collision/ColliderAnimationData.h>
#include <SEED/Source/Manager/CollisionManager/CollisionData.h>

// 前方宣言
class GameObject;
class GameObject2D;

// コライダーの種類
enum class ColliderType2D : uint32_t{
    Circle = 0,
    AABB,
    Quad,
};

// コライダーの基底クラス
class Collider2D{
    friend class ColliderEditor;
    friend class ColliderEditor2D;
public:// 基本関数--------------------------------------------------------------
    Collider2D();
    virtual ~Collider2D();

public:
    virtual void Update();
    virtual void UpdateMatrix();
    virtual void Draw();
    virtual void BeginFrame();
    virtual void CheckCollision(Collider2D* collider);
    virtual bool CheckCollision(const Quad2D& quad);
    virtual bool CheckCollision(const Circle& circle);
    virtual void OnCollision(Collider2D* collider, ObjectType objectType);

protected:
    virtual void UpdateBox();
    void PushBack(Collider2D* collider1, Collider2D* collider2, CollisionData2D collisionData);
    bool CanSkipPushBack(ObjectType objectType);
public:
    void AddSkipPushBackType(ObjectType objectType);
    void RemoveSkipPushBackType(ObjectType objectType);
    virtual void DiscardPreCollider();

public:// 編集用関数--------------------------------------------------------------
    virtual void Edit();
    virtual nlohmann::json GetJsonData();
    virtual void LoadFromJson(const nlohmann::json& jsonData);

protected:
    void EditAnimation();

public:// アクセッサ--------------------------------------------------------------

    // 基礎情報
    void SetOwnerObject(GameObject2D* parentObject);
    GameObject2D* GetOwnerObject()const{ return ownerObject_; }
    void SetObjectType(ObjectType objectType){ objectType_ = objectType;}
    ObjectType GetObjectType()const{ return objectType_; }
    ColliderType2D GetColliderType()const{ return colliderType_; }
    uint32_t GetColliderID()const{ return colliderID_; }
    void SetColliderID(uint32_t colliderID){ colliderID_ = colliderID; }

    // ペアレント情報
    void SetParentMatrix(const Matrix3x3* parentMat, bool isParentScale = true){
        parentMat_ = parentMat;
        isParentScale_ = isParentScale;
    }
    void SetIsParentRotate(bool isParentRotate){ isParentRotate_ = isParentRotate; }
    void SetIsParentScale(bool isParentScale){ isParentScale_ = isParentScale; }
    void SetIsParentTranslate(bool isParentTranslate){ isParentTranslate_ = isParentTranslate; }

    // ワールド、ローカル行列
    Matrix3x3 GetWorldMat()const{ return worldMat_; }
    Matrix3x3 GetLocalMat()const{ return localMat_; }

    // トランスフォーム取得
    Vector2 GetWoarldTranslate()const{ return ExtractTranslation(worldMat_) + offset_; }
    Vector2 GetLocalTranslate()const{ return ExtractTranslation(localMat_) + offset_; }
    void AddTranslate(const Vector2& addValue){ translate_ += addValue; }
    float GetWorldRotate()const{ return ExtractRotation(worldMat_); }
    float GetLocalRotate()const{ return ExtractRotation(localMat_); }
    Vector2 GetWorldScale()const{ return ExtractScale(worldMat_); }
    Vector2 GetLocalScale()const{ return ExtractScale(localMat_); }
    virtual bool IsMoved();
    const Vector2& GetCollidedPosition()const{ return collidedPosition_; }

    // 衝突判定八分木用
    const AABB2D& GetBox()const{ return coverAABB_; }

private:// 衝突判定を行わないリスト
    std::unordered_set<ObjectType> skipPushBackTypes_;

protected:// 基礎情報--------------------------------------------------------------
    GameObject2D* ownerObject_ = nullptr;
    ColliderType2D colliderType_;
    ObjectType objectType_ = ObjectType::Editor;
    static uint32_t nextID_;
    uint32_t colliderID_;
    bool isCollision_ = false;
    bool preIsCollision_ = false;
    Vector2 collidedPosition_;
    Vector4 color_;

protected:// 親子付け情報-----------------------------------------------------------
    const Matrix3x3* parentMat_ = nullptr;
    bool isParentRotate_ = true;
    bool isParentScale_ = true;
    bool isParentTranslate_ = true;

public:// 物理パラメータ----------------------------------------------------------
    bool isMovable_ = true;
    float mass_ = 1.0f;
    float miu_;
    bool isGhost_ = false;// 当たり判定のみ行い押し戻ししない場合true

public:// トランスフォーム情報------------------------------------------------------
    Vector2 scale_{ 1.0f,1.0f };
    float rotate_;
    Vector2 translate_;
    Vector2 offset_;

protected:
    Matrix3x3 localMat_;
    Matrix3x3 worldMat_;

protected:// 衝突に使用するパラメータ--------------------------------------------------
    AABB2D coverAABB_;// 頂点を包含するAABB
    std::unordered_set<uint32_t> collisionList_;// 今のフレームですでに衝突したオブジェクトのIDリスト

protected:
    bool isEdit_ = false;
};