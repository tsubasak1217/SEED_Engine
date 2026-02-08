#pragma once
#include <SEED/Source/Basic/Components/IComponent.h>
#include <Environment/Physics.h>

namespace SEED{
    /// <summary>
    /// 重力を適用するコンポーネント
    /// </summary>
    class Gravity3DComponent : public IComponent{
    public:
        Gravity3DComponent(GameObject* pOwner, const std::string& tagName = "");
        ~Gravity3DComponent() = default;
        void Initialize()override;
        void BeginFrame() override;
        void Update() override;
        void Draw() override;
        void EndFrame() override;
        void Finalize() override;
        // GUI編集
        void EditGUI() override;
        // json
        nlohmann::json GetJsonData() const override;
        void LoadFromJson(const nlohmann::json& jsonData) override;

        //=====================================
        // 物理
        //=====================================

        /*------- 重力・落下 ------*/
        float GetDropSpeed()const{ return dropSpeed_; }
        void SetDropSpeed(float _dropSpeed){ dropSpeed_ = _dropSpeed; }
        bool GetIsDrop()const{ return isDrop_; }

    private:
        /*----------- 物理 ----------*/
        bool isUseCostomGravity_ = false;
        float customGravity_ = Physics::kGravity;
        bool isDrop_ = false;
        float dropSpeed_ = 0.0f;
        Vector3 gravityDirection_ = Vector3(0.0f, -1.0f, 0.0f);
        Vector3 velocity_;
    };
}