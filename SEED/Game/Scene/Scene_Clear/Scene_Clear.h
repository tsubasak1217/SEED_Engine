#pragma once

//host
#include "Scene_Base.h"

///stl
#include <memory>

///local
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
    std::unique_ptr<Model> dinosaur_;
    std::unique_ptr<Model> egg_;
public:
    Model* GetDinosaur(){ return dinosaur_.get(); };
    Model* GetEgg(){ return egg_.get(); };
};