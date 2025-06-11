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
    CollisionComponent(GameObject* pOwner, const std::string& tagName = "");
    ~CollisionComponent() = default;
    void Initialize(const std::string& fileName, ObjectType objectType);
    void BeginFrame()override;
    void Update()override;
    void Draw()override;
    void EndFrame()override;
    void Finalize()override;
    void EditGUI()override;

public:
    virtual void HandOverColliders();
    void InitColliders(const std::string& fileName, ObjectType objectType);
    void ResetCollider();
    void AddCollider(Collider* collider);
    void LoadColliders(const std::string& fileName, ObjectType objectType);
    void EraseCheckColliders();

    // アクセッサ
    std::vector<std::unique_ptr<Collider>>& GetColliders(){ return colliders_; }
    virtual void AddSkipPushBackType(ObjectType skipType);
    void SetIsActive(bool _collidable){isActive_ = _collidable;}

public:// json
    void LoadFromJson(const nlohmann::json& jsonData) override;
    nlohmann::json GetJsonData() const override;

protected:
    std::vector<std::unique_ptr<Collider>> colliders_;
    bool isCollide_ = false;
    bool preIsCollide_ = false;
    bool isActive_ = true;
    bool isColliderVisible_ = false;
    Vector3 prePos_;
    std::unique_ptr<ColliderEditor> colliderEditor_;
};