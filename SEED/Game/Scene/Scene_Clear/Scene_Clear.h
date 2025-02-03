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
    // EngineObjects
    std::unique_ptr<DirectionalLight> directionalLight_ = nullptr;

    // model
    std::unique_ptr<Model> dinosaur_;
public:
    Model* GetDinosaur(){ return dinosaur_.get(); };
 };