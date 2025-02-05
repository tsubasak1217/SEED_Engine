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
    std::unique_ptr<Model> eggBottom_;
    std::unique_ptr<Model> eggTop_;

    std::unique_ptr<Model> corpsesPile_;

    //================== Emitter ==================//
    struct CorpseEmitter{
        std::array<std::unique_ptr<Model>,128> particles_;
        std::array<bool,128> particleActiveStatus_;
        std::array<float,128> particleDropSpeed_;

        Vector3 min_ = {-10.f,6.77f,15.f};
        Vector3 max_ = {10.f,11.9f,20.f};

        int emitValueMin_ = 3;
        int emitValueMax_ = 6;

        float emitCoolTime_ = .3f;
        float leftCoolTime_ = .0f;
    };
    std::unique_ptr<CorpseEmitter> corpseEmitter_ = nullptr;

public:
    Model* GetDinosaur(){ return dinosaur_.get(); };
    void SetDinosaur(std::unique_ptr<Model> model){ dinosaur_ = std::move(model); };

    Model* GetEggTop(){ return eggTop_.get(); };
    void SetEggTop(std::unique_ptr<Model> model){ eggTop_ = std::move(model); };

    Model* GetEggBottom(){ return eggBottom_.get(); };
    void SetEggBottom(std::unique_ptr<Model> model){ eggBottom_ = std::move(model); };

    Model* GetCorpsesPile(){ return corpsesPile_.get(); };

};