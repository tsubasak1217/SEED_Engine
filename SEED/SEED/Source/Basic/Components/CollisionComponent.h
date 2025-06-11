#pragma once
#include <SEED/Source/Basic/Object/GameObject.h>
#include <SEED/Source/Basic/Collision/Collider.h>
#include <SEED/Source/Basic/Components/IComponent.h>
#include <SEED/Source/Manager/CollisionManager/ColliderEditor.h>
// stl
#include <memory>
#include <vector>

/*----------- 衝突判定をするやつ ----------*/
class CollisionComponent : public IComponent{
public:
    CollisionComponent(GameObject* pOwner, const std::string& tagName);
    ~CollisionComponent() = default;
    void Initialize(const std::string& fileName, ObjectType objectType);
    void BeginFrame();
    void Update();
    void Draw();
    void EndFrame();
    void Finalize();

public:
    //=====================================
    // コライダー関連
    //=====================================
    virtual void HandOverColliders();
    void InitColliders(const std::string& fileName, ObjectType objectType);
    void ResetCollider();
    void AddCollider(Collider* collider);
    void LoadColliders(ObjectType objectType);
    void LoadColliders(const std::string& fileName, ObjectType objectType);
    virtual void InitColliders(ObjectType objectType);
    void EraseCheckColliders();

    // アクセッサ
    std::vector<std::unique_ptr<Collider>>& GetColliders(){ return colliders_; }
    virtual void AddSkipPushBackType(ObjectType skipType);
    bool GetIsCollide()const{ return isCollide_; }
    bool GetIsCollideEnter()const{ return isCollide_ && !preIsCollide_; }
    void SetCollidable(bool _collidable){isHandOverColliders_ = _collidable;}

protected:
    std::vector<std::unique_ptr<Collider>> colliders_;
    bool isCollide_ = false;
    bool preIsCollide_ = false;
    bool isHandOverColliders_ = true;
    bool isColliderVisible_ = false;
    Vector3 prePos_;
    std::unique_ptr<ColliderEditor> colliderEditor_;
};