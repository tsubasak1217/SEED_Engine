#pragma once
#include <memory>
#include <cstdint>
#include <string>
#include <algorithm>

#include <SEED/Lib/Structs/Model.h>
#include <Environment/Physics.h>
#include <SEED/Source/Basic/Collision/3D/Collider.h>
#include <SEED/Source/Manager/CollisionManager/CollisionManager.h>
#include <SEED/Source/Manager/CollisionManager/ColliderEditor.h>
#include <SEED/Source/Manager/ClockManager/ClockManager.h>
#include <SEED/Lib/Functions/MyMath.h>
#include <SEED/Lib/Functions/MyFunc.h>
#include <SEED/Source/Basic/Components/ComponentDictionary.h>

class Scene_Base;
class Hierarchy;

struct ParentComponentInfo{
    IComponent* pComponent = nullptr;
    Matrix4x4* pMatrix = nullptr;
};

class GameObject{

    ////////////////////////////////////////////////////////////////////////////
    // メンバー関数
    ////////////////////////////////////////////////////////////////////////////

    /*----------- 基本関数 ----------*/
public:
    GameObject(Scene_Base* pScene);
    ~GameObject();
    void Initialize();
    void Update();
    void Draw();
    void BeginFrame();
    void EndFrame();

public:// コンポーネントの管理関数
    template<typename TComponent>
    TComponent* AddComponent(const std::string& tagName){
        static_assert(std::is_base_of<IComponent, TComponent>::value, "TComponent must inherit from IComponent");
        components_.emplace_back(std::make_unique<TComponent>(this, tagName));
        return static_cast<TComponent*>(components_.back().get());
    }

    template<typename TComponent>
    TComponent* AddComponent(){
        static_assert(std::is_base_of<IComponent, TComponent>::value, "TComponent must inherit from IComponent");
        components_.emplace_back(std::make_unique<TComponent>(this));
        return static_cast<TComponent*>(components_.back().get());
    }

    // 型と名前で取得
    template<typename TComponent>
    TComponent* GetComponent(const std::string& tagName = ""){
        static_assert(std::is_base_of<IComponent, TComponent>::value, "TComponent must inherit from IComponent");
        for(auto& component : components_){
            // 名前が一致しているか
            if(!tagName.empty()){
                if(component->GetTagName() == tagName){
                    // 型が一致しているか
                    if(auto* foundComponent = dynamic_cast<TComponent*>(component.get())){
                        return foundComponent;
                    }
                }
            } else{
                // 型が一致しているか
                if(auto* foundComponent = dynamic_cast<TComponent*>(component.get())){
                    return foundComponent;
                }
            }
        }
        return nullptr;
    }

    // 型とインデックスで取得
    template<typename TComponent>
    TComponent* GetComponent(uint32_t index){
        static_assert(std::is_base_of<IComponent, TComponent>::value, "TComponent must inherit from IComponent");
        uint32_t count = 0;
        for(auto& component : components_){
            // 型が一致しているか
            if(auto* foundComponent = dynamic_cast<TComponent*>(component.get())){
                // インデックスが一致しているか
                if(count == index){
                    return foundComponent;
                }
                count++;
            }
        }
        return nullptr;
    }

    /*--- 物理・トランスフォーム関連 ---*/
public:
    void UpdateMatrix();

    /*-------- 当たり判定時関数 --------*/
public:
    void OnCollision(GameObject* other);
protected:
    void OnCollisionEnter(GameObject* other);
    void OnCollisionStay(GameObject* other);
    void OnCollisionExit(GameObject* other);

    /*----------- デバッグ用 ----------*/
public:
    void EditGUI();

protected:
    void ContextMenu();
    IComponent* contextMenuComponent_;


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

    //=====================================
    // 親子付け関連
    //=====================================
    void SetParent(GameObject* parent);
    void SetParentComponentInfo(const ParentComponentInfo& info){ parentComponentInfo_ = info; }
    GameObject* GetParent(){ return parent_; }
    const std::list<GameObject*>& GetChildren() const{ return children_; }
    std::list<GameObject*> GetAllChildren()const;
    void RemoveChild(GameObject* child);
    void ReleaseParent();
    void ReleaseChildren();
    bool IsDescendant(GameObject* obj) const;
    Vector3 GetTargetPos()const{ return GetWorldTranslate() + targetOffset_; }


    //=====================================
    // トランスフォーム
    //=====================================
    const Transform& GetLocalTransform(){ return localTransform_; }
    const Transform& GetWorldTransform(){ return worldTransform_; }
    /*------ scale -------*/
    const Vector3& GetWorldScale() const{ return worldTransform_.scale; }
    const Vector3& GetLocalScale() const{ return localTransform_.scale; }
    void SetWorldScale(const Vector3& scale);
    void SetLocalScale(const Vector3& scale);
    /*------ rotate -------*/
    const Quaternion& GetWorldRotate() const{ return worldTransform_.rotate; }
    const Quaternion& GetLocalRotate() const{ return localTransform_.rotate; }
    Vector3 GetWorldEulerRotate() const{ return worldTransform_.rotate.ToEuler(); }
    Vector3 GetLocalEulerRotate() const{ return localTransform_.rotate.ToEuler(); }
    void AddWorldRotate(const Quaternion& addValue);
    void AddWorldRotate(const Vector3& addValue);
    void SetWorldRotate(const Quaternion& rotate);
    void SetLocalRotate(const Quaternion& rotate);
    /*------ translate -------*/
    const Vector3& GetWorldTranslate() const{ return worldTransform_.translate; }
    const Vector3& GetLocalTranslate() const{ return localTransform_.translate; }
    void AddWorldTranslate(const Vector3& addValue);
    void SetWorldTranslate(const Vector3& translate);
    void SetLocalTranslate(const Vector3& translate);
    const Vector3& GetPrePos() const{ return prePos_; }
    /*------ matrix -------*/
    const Matrix4x4& GetLocalMat() const{ return localMat_; }
    const Matrix4x4& GetWorldMat() const{ return worldMat_; }
    const Matrix4x4* GetWorldMatPtr() const{ return &worldMat_; }
    /*------- velocity ------*/
    const Vector3& GetVelocity()const{ return velocity_; }
    void SetVelocity(const Vector3& velocity){ velocity_ = velocity; }
    void SetVelocityX(float x){ velocity_.x = x; }
    void SetVelocityY(float y){ velocity_.y = y; }
    void SetVelocityZ(float z){ velocity_.z = z; }
    /*-------- state --------*/
    bool GetIsOnGrounnd()const{ return isOnGround_; }
    void SetIsOnGround(bool flag){ isOnGround_ = flag; }

    //=====================================
    // json
    //=====================================
    nlohmann::json GetJsonData(int32_t depth);
    void LoadFromJson(const nlohmann::json& jsonData);
    static GameObject* CreateFromJson(const nlohmann::json& jsonData);
    static std::vector<GameObject*> CreateFamily(const nlohmann::json& jsonData, GameObject* parent = nullptr);

    ////////////////////////////////////////////////////////////////////////////
    // メンバー変数
    ////////////////////////////////////////////////////////////////////////////

private:
    /*----------- 基礎情報----------*/
    static uint32_t nextID_;
    uint32_t objectID_;
    ObjectType objectType_;
    std::string objectName_;
    Vector3 targetOffset_;
    std::list<std::unique_ptr<IComponent>> components_;

public:
    bool isAlive_ = true;
    bool isActive_ = true;
    bool isMustDraw_ = false;

public:
    //---------- 親子付け情報 ---------//
    GameObject* parent_ = nullptr;
    ParentComponentInfo parentComponentInfo_;
    Matrix4x4* parentJointMat_ = nullptr;
    std::list<GameObject*> children_;
    bool isParentRotate_ = true;
    bool isParentScale_ = true;
    bool isParentTranslate_ = true;

    //---------- 描画情報 ---------//
    Color masterColor_ = { 1.0f,1.0f,1.0f,1.0f };// オブジェクト全体の色

    //------- トランスフォーム情報 ------//
    Transform localTransform_;
private:
    Transform worldTransform_;
    Matrix4x4 localMat_;
    Matrix4x4 worldMat_;
    Vector3 velocity_;

private:
    /*----------- 衝突判定 ----------*/
    bool isCollide_ = false;
    bool preIsCollide_ = false;
    bool isOnGround_ = true;
    bool preIsOnGround_ = true;
    Vector3 prePos_;
};