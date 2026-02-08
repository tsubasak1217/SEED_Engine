#pragma once
#include <SEED/Source/Basic/Object/GameObject.h>
#include <SEED/Source/Basic/Object/GameObject2D.h>
#include <SEED/Source/Basic/Collision/2D/Collider2D.h>
#include <SEED/Source/Basic/Components/IComponent.h>
#include <SEED/Source/Manager/CollisionManager/ColliderEditor2D.h>
// stl
#include <memory>
#include <vector>

namespace SEED{
    /// <summary>
    /// 2次元衝突判定コンポーネント
    /// </summary>
    class Collision2DComponent : public IComponent{
    public:
        Collision2DComponent(GameObject2D* pOwner, const std::string& tagName = "");
        ~Collision2DComponent() = default;
        void Initialize(const std::string& fileName, ObjectType objectType);
        void BeginFrame()override;
        void Update()override;
        void Draw()override;
        void EndFrame()override;
        void Finalize()override;
        void EditGUI()override;

    public:
        virtual void HandOverColliders();
        void InitColliders(nlohmann::json json, ObjectType objectType);
        void ResetCollider();
        void AddCollider(Collider2D* collider);
        void LoadColliders(nlohmann::json json, ObjectType objectType);

        // アクセッサ
        std::vector<std::unique_ptr<Collider2D>>& GetColliders(){ return colliders_; }
        virtual void AddSkipPushBackType(ObjectType skipType);
        void SetIsActive(bool _collidable){ isActive_ = _collidable; }

    public:// json
        void LoadFromJson(const nlohmann::json& jsonData) override;
        nlohmann::json GetJsonData() const override;

    protected:
        std::vector<std::unique_ptr<Collider2D>> colliders_;
        bool isCollide_ = false;
        bool preIsCollide_ = false;
        bool isActive_ = true;
        bool isColliderVisible_ = true;
        Vector3 prePos_;
        ObjectType objectType_ = ObjectType::All; // オブジェクトの種類
        std::unique_ptr<ColliderEditor2D> colliderEditor_;
    };
}