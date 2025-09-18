#pragma once
#include <SEED/Source/Basic/Scene/Scene_Base.h>

// stl
#include <list>
#include <memory>
#include <string>
#include <vector>

// local
#include <SEED/Lib/Structs/Model.h>
#include <SEED/Lib/Structs/Sprite.h>
#include <SEED/Lib/Shapes/Triangle.h>

class Scene_Load
    : public Scene_Base{

public:
    Scene_Load();
    ~Scene_Load() override;
    void Initialize() override;
    void Finalize() override;
    void Update() override;
    void Draw() override;
    void BeginFrame() override;
    void EndFrame() override;
    void HandOverColliders() override;

private:

    // EngineObjects
    std::unique_ptr<DirectionalLight> directionalLight_ = nullptr;
};