#pragma once
#include <SEED/Source/Basic/Scene/Scene_Base.h>

// stl
#include <memory>

// object
#include <SEED/Lib/Structs/Sprite.h>
#include <SEED/Lib/Structs/Model.h>

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

};