#pragma once
#include <memory>
#include <cstdint>
#include <string>
#include <algorithm>
#include <unordered_set>

#include <SEED/Lib/Structs/Model.h>
#include <Environment/Physics.h>
#include <SEED/Source/Basic/Collision/3D/Collider.h>
#include <SEED/Source/Basic/Collision/2D/Collider2D.h>
#include <SEED/Source/Manager/CollisionManager/CollisionManager.h>
#include <SEED/Source/Manager/CollisionManager/ColliderEditor.h>
#include <SEED/Source/Manager/ClockManager/ClockManager.h>
#include <SEED/Lib/Functions/MyFunc/MyMath.h>
#include <SEED/Lib/Functions/MyFunc/MyFunc.h>
#include <SEED/Source/Basic/Components/ComponentDictionary.h>

class Scene_Base;
class Hierarchy;

class GameObject2D{

    ////////////////////////////////////////////////////////////////////////////
    // メンバー関数
    ////////////////////////////////////////////////////////////////////////////

    /*----------- 基本関数 ----------*/
public:
    GameObject2D(Scene_Base* pScene);
    ~GameObject2D();
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
    void OnCollision(GameObject2D* other,Collider2D* collider);
    void CheckCollisionExit();
protected:
    void OnCollisionEnter(GameObject2D* other);
    void OnCollisionStay(GameObject2D* other);
    void OnCollisionExit(GameObject2D* other);

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
    void SetObjectType(ObjectType type){ objectType_ = type; }
    std::string GetName() const{ return objectName_; }
    void SetName(const std::string& name){ objectName_ = name; }
    void SetIsActive(bool isActive){ isActive_ = isActive; }
    bool GetIsActive() const{ return isActive_; }
    void SetIsMustDraw(bool isMustDraw){ isMustDraw_ = isMustDraw; }
    bool GetIsMustDraw() const{ return isMustDraw_; }

    //=====================================
    // 親子付け関連
    //=====================================
    void SetParent(GameObject2D* parent);
    GameObject2D* GetParent(){ return parent_; }
    const std::list<GameObject2D*>& GetChildren() const{ return children_; }
    std::list<GameObject2D*> GetAllChildren()const;
    void RemoveChild(GameObject2D* child);
    void ReleaseParent();
    void ReleaseChildren();
    bool IsDescendant(GameObject2D* obj) const;
    Vector2 GetTargetPos()const{ return GetWorldTranslate() + targetOffset_; }


    //=====================================
    // トランスフォーム
    //=====================================
    Transform2D GetLocalTransform(){ return localTransform_; }
    Transform2D GetWorldTransform(){ return worldTransform_; }
    /*------ scale -------*/
    Vector2 GetWorldScale() const{ return worldTransform_.scale; }
    const Vector2& GetLocalScale() const{ return localTransform_.scale; }
    void SetWorldScale(const Vector2& scale);
    void SetLocalScale(const Vector2& scale);
    /*------ rotate -------*/
    float GetWorldRotate() const{ return worldTransform_.rotate; }
    float GetLocalRotate() const{ return localTransform_.rotate; }
    void SetWorldRotate(float rotate);
    void SetLocalRotate(float rotate);
    /*------ translate -------*/
    Vector2 GetWorldTranslate() const{ return worldTransform_.translate; }
    const Vector2& GetLocalTranslate() const{ return localTransform_.translate; }
    void AddWorldTranslate(const Vector2& addValue);
    void SetWorldTranslate(const Vector2& translate);
    void SetLocalTranslate(const Vector2& translate);
    const Vector2& GetPrePos() const{ return prePos_; }
    Vector2 GetMoveVector() const{ return GetWorldTranslate() - prePos_; }
    /*------ matrix -------*/
    const Matrix3x3& GetLocalMat() const{ return localMat_; }
    const Matrix3x3& GetWorldMat() const{ return worldMat_; }
    const Matrix3x3* GetWorldMatPtr() const{ return &worldMat_; }
    /*------- velocity ------*/
    Vector2 GetVelocity()const{ return velocity_; }
    void SetVelocity(const Vector2& velocity){ velocity_ = velocity; }
    void SetVelocityX(float x){ velocity_.x = x; }
    void SetVelocityY(float y){ velocity_.y = y; }
    /*-------- state --------*/
    bool GetIsOnGround()const{ return isOnGround_; }
    void SetIsOnGround(bool flag){ isOnGround_ = flag; }
    bool GetIsOnGroundTrigger()const{ return isOnGround_ && !preIsOnGround_; }
    bool GetIsCeiling()const{ return isCeiling_; }
    void SetIsCeiling(bool flag){ isCeiling_ = flag; }
    bool GetIsCeilingTrigger()const{ return isCeiling_ && !preIsCeiling_; }
    bool GetIsCollideAny()const{ return isCollideAny_; }
    bool GetIsCollideAnyTrigger()const{ return isCollideAny_ && !preIsCollideAny_; }
    bool GetIsCollideGhost()const{ return isCollideGhost_; }
    bool GetIsCollideGhostTrigger()const{ return isCollideGhost_ && !preIsCollideGhost_; }
    bool GetIsCollideSolid()const{ return isCollideSolid_; }
    bool GetIsCollideSolidTrigger()const{ return isCollideSolid_ && !preIsCollideSolid_; }

    //=====================================
    // json
    //=====================================
    nlohmann::json GetJsonData(int32_t depth);
    void LoadFromJson(const nlohmann::json& jsonData);
    static GameObject2D* CreateFromJson(const nlohmann::json& jsonData);
    static std::vector<GameObject2D*> CreateFamily(const nlohmann::json& jsonData, GameObject2D* parent = nullptr);

    ////////////////////////////////////////////////////////////////////////////
    // メンバー変数
    ////////////////////////////////////////////////////////////////////////////

    /*----------- 基礎情報----------*/
protected:
    static uint32_t nextID_;
    uint32_t objectID_;
    ObjectType objectType_;
    std::string objectName_;
    Vector2 targetOffset_;
    bool isActive_ = true;
    bool isMustDraw_ = false;
    std::list<std::unique_ptr<IComponent>> components_;

public:
    //---------- 親子付け情報 ---------//
    GameObject2D* parent_ = nullptr;
    std::list<GameObject2D*> children_;
    bool isParentRotate_ = true;
    bool isParentScale_ = true;
    bool isParentTranslate_ = true;

    //------- トランスフォーム情報 ------//
    Transform2D localTransform_;
private:
    Transform2D worldTransform_;
    Matrix3x3 localMat_;
    Matrix3x3 worldMat_;
    Vector2 velocity_;

    /*----------- 衝突判定 ----------*/
protected:

    bool isCollideAny_ = false;// 何かしらのオブジェクトと衝突したかどうか
    bool preIsCollideAny_ = false;
    bool isCollideGhost_ = false;// すり抜けるオブジェクトと衝突したかどうか
    bool preIsCollideGhost_ = false;
    bool isCollideSolid_ = false;// すり抜けないオブジェクトと衝突したかどうか
    bool preIsCollideSolid_ = false;
    std::unordered_set<GameObject2D*>preCollideObjects_;

    // 接地などの判定
    bool isOnGround_ = true;
    bool preIsOnGround_ = true;
    bool isCeiling_ = false;
    bool preIsCeiling_ = false;

    // 前の座標
    Vector2 prePos_;
};