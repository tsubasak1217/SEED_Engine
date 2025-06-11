#pragma once
#include <memory>
#include <cstdint>
#include <string>
#include <algorithm>

#include <SEED/Lib/Structs/Model.h>
#include <Environment/Physics.h>
#include <SEED/Source/Basic/Collision/Collider.h>
#include <SEED/Source/Manager/CollisionManager/CollisionManager.h>
#include <SEED/Source/Manager/CollisionManager/ColliderEditor.h>
#include <SEED/Source/Manager/ClockManager/ClockManager.h>
#include <SEED/Lib/Functions/MyFunc/MyMath.h>
#include <SEED/Lib/Functions/MyFunc/MyFunc.h>
#include <SEED/Source/Basic/Components/ComponentDictionary.h>

class GameObject{

    ////////////////////////////////////////////////////////////////////////////
    // メンバー関数
    ////////////////////////////////////////////////////////////////////////////

    /*----------- 基本関数 ----------*/
public:
    GameObject();
    GameObject(const std::string& modelFilePath, const std::string& tagName);
    virtual ~GameObject();
    virtual void Initialize();
    virtual void Update();
    virtual void Draw();
    virtual void BeginFrame();
    virtual void EndFrame();

public:// コンポーネントの管理関数
    template<typename TComponent>
    TComponent* AddComponent(const std::string& tagName){
        static_assert(std::is_base_of<IComponent, TComponent>::value, "TComponent must inherit from IComponent");
        components_.emplace_back(std::make_unique<TComponent>(this, tagName));
        return static_cast<TComponent*>(components_.back().get());
    }

    template<typename TComponent>
    TComponent* GetComponent(const std::string& tagName){
        static_assert(std::is_base_of<IComponent, TComponent>::value, "TComponent must inherit from IComponent");
        for(auto& component : components_){
            // 名前が一致しているか
            if(component->GetTagName() == tagName){
                // 型が一致しているか
                if(auto* foundComponent = dynamic_cast<TComponent*>(component.get())){
                    return foundComponent;
                }
            }
        }
        return nullptr;
    }

    /*--- 物理・トランスフォーム関連 ---*/
public:
    void UpdateMatrix();
    void EditCollider();
protected:
    void EndFrameDropFlagUpdate();
    void MoveByVelocity();

    /*-------- コライダー関連 --------*/
public:
    virtual void HandOverColliders();
    virtual void OnCollision(GameObject* other, ObjectType objectType);
    void InitColliders(const std::string& fileName, ObjectType objectType);
    virtual void DiscardPreCollider();
    void ResetCollider();
protected:
    void LoadColliders(ObjectType objectType);
    void LoadColliders(const std::string& fileName, ObjectType objectType);
    virtual void InitColliders(ObjectType objectType);
    void EraseCheckColliders();


    ////////////////////////////////////////////////////////////////////////////
    // アクセッサ
    ////////////////////////////////////////////////////////////////////////////
public:

    //=====================================
    // 基礎情報
    //=====================================
    uint32_t GetObjectID() const{ return objectID_; }
    ObjectType GetObjectType() const{ return objectType_; }
    std::string GetName() const{ return objectName_; }
    void SetName(const std::string& name){ objectName_ = name; }
    const std::string& GetIdolName() const{ return idolName_; }
    /*------ parent -------*/
    void SetParent(const GameObject* parent){ parent_ = parent; }
    const GameObject* GetParent() const{ return parent_; }
    void ReleaseParent(){ parent_ = nullptr; }
    Vector3 GetTargetPos()const{ return GetWorldTranslate() + targetOffset_; }


    //=====================================
    // トランスフォーム
    //=====================================

    /*------ scale -------*/
    Vector3 GetWorldScale() const{ return ExtractScale(worldMat_); }
    const Vector3& GetLocalScale() const{ return transform_.scale; }
    /*------ rotate -------*/
    Vector3 GetWorldRotate() const{ return ExtractRotation(worldMat_); }
    const Vector3& GetLocalRotate() const{ return transform_.rotate; }

    /*------ translate -------*/
    Vector3 GetWorldTranslate() const{ return ExtractTranslation(worldMat_); }
    const Vector3& GetLocalTranslate() const{ return transform_.translate; }
    void AddWorldTranslate(const Vector3& addValue);
    const Vector3& GetPrePos() const{ return prePos_; }
    /*------ matrix -------*/
    const Matrix4x4& GetLocalMat() const{ return localMat_; }
    const Matrix4x4& GetWorldMat() const{ return worldMat_; }
    const Matrix4x4* GetWorldMatPtr() const{ return &worldMat_; }

    //=====================================
    // 物理
    //=====================================

    /*------- 重力・落下 ------*/
    void SetIsApplyGravity(bool isApplyGravity){ isApplyGravity_ = isApplyGravity; }
    bool GetIsApplyGravity()const{ return isApplyGravity_; }
    void SetIsDrop(bool isDrop){ isDrop_ = isDrop; }
    float GetDropSpeed()const{ return dropSpeed_; }
    void SetDropSpeed(float _dropSpeed){ dropSpeed_ = _dropSpeed; }
    bool GetIsDrop()const{ return isDrop_; }

    /*------- 重さ ------*/
    float GetWeight()const{ return weight_; }
    void SetWeight(float weight){ weight_ = weight; }

    /*------- 速度 ------*/
    Vector3 GetVelocity()const{ return velocity_; }
    void SetVelocity(const Vector3& velocity){ velocity_ = velocity; }
    void SetVelocityX(float x){ velocity_.x = x; }
    void SetVelocityY(float y){ velocity_.y = y; }
    void SetVelocityZ(float z){ velocity_.z = z; }

    //=====================================
    // コライダー関連
    //=====================================
    void AddCollider(Collider* collider);
    std::vector<std::unique_ptr<Collider>>& GetColliders(){ return colliders_; }
    virtual void AddSkipPushBackType(ObjectType skipType);
    bool GetIsCollide()const{ return isCollide_; }
    bool GetIsCollideEnter()const{ return isCollide_ && !preIsCollide_; }
    void SetCollidable(bool _collidable);

    //=====================================
    // json
    //=====================================
    virtual const nlohmann::json& GetJsonData();

    ////////////////////////////////////////////////////////////////////////////
    // メンバー変数
    ////////////////////////////////////////////////////////////////////////////

    /*----------- 基礎情報----------*/
protected:
    static uint32_t nextID_;
    uint32_t objectID_;
    ObjectType objectType_;
    std::string idolName_;
    std::string objectName_;
    Vector3 targetOffset_;
    std::list<std::unique_ptr<IComponent>> components_;

public:
    // 親子付け
    const GameObject* parent_ = nullptr;
    bool isParentRotate_ = true;
    bool isParentScale_ = true;
    bool isParentTranslate_ = true;

    // トランスフォーム情報
    Transform transform_;
    bool isRotateWithQuaternion_ = true;

private:
    // 行列情報
    Matrix4x4 localMat_;
    Matrix4x4 worldMat_;

    /*----------- 衝突判定 ----------*/
protected:
    std::vector<std::unique_ptr<Collider>> colliders_;
    bool isCollide_ = false;
    bool preIsCollide_ = false;
    bool isHandOverColliders_ = true;
    Vector3 prePos_;
    std::unique_ptr<ColliderEditor> colliderEditor_;

    /*----------- 物理 ----------*/
protected:
    bool isApplyGravity_ = true;
    bool isDrop_ = false;
    float dropSpeed_ = 0.f;
    float weight_ = 1.f;
    Vector3 velocity_ = { 0.f,0.f,0.f };
};