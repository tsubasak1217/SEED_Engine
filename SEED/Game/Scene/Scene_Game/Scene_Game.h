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

// camera
#include <SEED/Source/Basic/Camera/FollowCamera.h>


class Scene_Game
    : public Scene_Base{

public:
    Scene_Game();
    ~Scene_Game() override;
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

    GameObject* player_ = nullptr;
    GameObject* brainStem_ = nullptr;
    std::unique_ptr<FollowCamera> followCamera_ = nullptr;
};