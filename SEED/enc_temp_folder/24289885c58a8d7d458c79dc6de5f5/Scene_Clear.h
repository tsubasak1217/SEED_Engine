#pragma once

//host
#include "Scene_Base.h"

///stl
#include <memory>

///local
//light
#include "DirectionalLight.h"
//objects
#include "Model.h"
#include "Base/BaseObject.h"

class Scene_Clear
    : public Scene_Base{
public:
    Scene_Clear();
    ~Scene_Clear()override;
    void Initialize()override;
    void Finalize()override;
    void Update()override;
    void Draw()override;
    void BeginFrame()override;
    void EndFrame()override;
    void HandOverColliders()override;

private:
    void UpdateCorpseParticles();

private:
    // EngineObjects
    std::unique_ptr<DirectionalLight> directionalLight_ = nullptr;

    // model
    std::unique_ptr<Model> dinosaur_;

    //================== Emitter ==================//
    struct CorpseEmitter{
        std::array<std::unique_ptr<Model>,128> particles_;
        std::array<bool,128> particleActiveStatus_;
        std::array<float,128> particleDropSpeed_;

        Vector3 min_;
        Vector3 max_;

        int emitValueMin_;
        int emitValueMax_;

        float emitCoolTime_ = .0f;
        float leftCoolTime_ = .0f;
    };
    std::unique_ptr<CorpseEmitter> corpseEmitter_ = nullptr;


public:
    Model* GetDinosaur(){ return dinosaur_.get(); };
};