#pragma once
#include <cstdint>
#include <vector>
#include "Vector3.h"
#include "Matrix4x4.h"
#include "ObjectType.h"

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

public:// 基本関数--------------------------------------------------------------
    Collider() = default;
    Collider(ColliderType colliderType);
    ~Collider();

public:
    void Update();
    void UpdateMatrix();
    void Draw();
    virtual void OnCollision(Collider* collider);

public:// アクセッサ--------------------------------------------------------------

    // ペアレント情報
    void SetParentMatrix(Matrix4x4* parentMat,bool isParentScale = true){
        parentMat_ = parentMat; 
        isParentScale_ = isParentScale;
    }
    void SetIsParentRotate(bool isParentRotate){ isParentRotate_ = isParentRotate; }
    void SetIsParentScale(bool isParentScale){ isParentScale_ = isParentScale; }
    void SetIsParentTranslate(bool isParentTranslate){ isParentTranslate_ = isParentTranslate; }

    // ワールド、ローカル行列
    Matrix4x4 GetWorldMat()const{ return worldMat_; }
    Matrix4x4 GetLocalMat()const{ return localMat_; }

private:// 基礎情報--------------------------------------------------------------
    ColliderType colliderType_;
    ObjectType objectType_;
    static uint32_t nextID_;
    uint32_t colliderID_;
    bool isCollision_ = false;

private:// 親子付け情報-----------------------------------------------------------
    Matrix4x4* parentMat_ = nullptr;
    bool isParentRotate_ = true;
    bool isParentScale_ = true;
    bool isParentTranslate_ = true;

public:// 物理パラメータ----------------------------------------------------------
    bool isMovable_ = false;
    float mass_;
    float miu_;

public:// トランスフォーム情報------------------------------------------------------
    Vector3 scale_;
    Vector3 rotate_;
    Vector3 translate_;
private:
    Matrix4x4 localMat_;
    Matrix4x4 worldMat_;

public:// 衝突に使用するパラメータ--------------------------------------------------
    std::vector<Vector3> prePoints_;
    std::vector<Vector3> currentPoints_;
    Vector3 halfSize_;// OBB,AABB用
    float radius_;// Sphere,カプセル用
};