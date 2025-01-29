#pragma once
#include "Scene_Base.h"

///stl
//pointer
#include <memory>

//object
#include "Model.h"

class Scene_Title : public Scene_Base{
public:
    Scene_Title();
    ~Scene_Title() override;

public:
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Finalize() override;
    void BeginFrame() override;
    void EndFrame() override;
    void HandOverColliders() override;

private:


};