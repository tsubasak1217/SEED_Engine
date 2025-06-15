#pragma once
#include <SEED/Source/Basic/Components/IComponent.h>
#include <SEED/Lib/Structs/DirectionalLight.h>
#include <SEED/Lib/Structs/SpotLight.h>
#include <SEED/Lib/Structs/PointLight.h>

class SpotLightComponent : public IComponent{
public:
    SpotLightComponent(GameObject* pOwner, const std::string& tagName = "");
    ~SpotLightComponent() = default;
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

private:
    std::unique_ptr<SpotLight> light_;
    Transform localTransform_; // ライトのローカル
    bool isParentRotate_ = true; // 親の回転を反映するか
};