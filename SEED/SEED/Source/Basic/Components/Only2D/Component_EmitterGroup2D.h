#pragma once
#include <SEED/Source/Basic/Object/GameObject2D.h>
#include <SEED/Source/Basic/Components/IComponent.h>
#include <SEED/Source/Manager/ParticleManager/CPUParticle/Emitter/EmitterGroup2D.h>
// stl
#include <memory>
#include <vector>

namespace SEED{
    /// <summary>
    /// 2Dパーティクルのエミッターを追加するコンポーネント
    /// </summary>
    class Component_EmitterGroup2D : public IComponent{
    public:
        Component_EmitterGroup2D(GameObject2D* pOwner, const std::string& tagName = "");
        ~Component_EmitterGroup2D() = default;
        void BeginFrame()override;
        void Update()override;
        void Draw()override;
        void EndFrame()override;
        void Finalize()override;
        void EditGUI()override;

    public:// accessor
        void Activate(){ isActive_ = true; }
        void Deactivate(){ isActive_ = false; }
        void InitEmitters();

    public:// json
        void LoadFromJson(const nlohmann::json& jsonData) override;
        nlohmann::json GetJsonData() const override;

    protected:
        std::unique_ptr<EmitterGroup2D> emitterGroup_;
        bool isParentToOwner_ = true;
        bool isActive_ = true;
    };
}