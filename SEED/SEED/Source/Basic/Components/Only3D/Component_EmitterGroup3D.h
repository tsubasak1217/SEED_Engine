#pragma once
#include <SEED/Source/Basic/Object/GameObject.h>
#include <SEED/Source/Basic/Components/IComponent.h>
#include <SEED/Source/Manager/ParticleManager/CPUParticle/Emitter/EmitterGroup3D.h>
// stl
#include <memory>
#include <vector>


class Component_EmitterGroup3D : public IComponent{
public:
    Component_EmitterGroup3D(GameObject* pOwner, const std::string& tagName = "");
    ~Component_EmitterGroup3D() = default;
    void BeginFrame()override;
    void Update()override;
    void Draw()override;
    void EndFrame()override;
    void Finalize()override;
    void EditGUI()override;

public:// accessor

public:// json
    void LoadFromJson(const nlohmann::json& jsonData) override;
    nlohmann::json GetJsonData() const override;

protected:
    std::unique_ptr<EmitterGroup3D> emitterGroup_;
    bool isParentToOwner_ = true;
};