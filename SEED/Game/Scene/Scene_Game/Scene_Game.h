#pragma once
#include "Scene_Base.h"

// external
#include <list>
#include <memory>
#include <string>
#include <vector>

// local
#include <Model.h>
#include <Sprite.h>
#include <Triangle.h>

// camera
#include "FollowCamera.h"


class Scene_Game : public Scene_Base
{

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

public:


private:


};