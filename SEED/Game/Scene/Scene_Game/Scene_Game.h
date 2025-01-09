#pragma once
#include "Scene_Base.h"
#include "State_Base.h"

// external
#include <string>
#include <vector>
#include <memory>
#include <list>

// local
#include <Triangle.h>
#include <Model.h>
#include <State_Base.h>
#include <Sprite.h>
#include "FollowCamera.h"
#include "../Game/Objects/Player/Player.h"
#include "../Game/Objects/Enemy/Enemy.h"
#include "../Game/Editor/FieldEditor.h"

class Scene_Game : public Scene_Base{

public:
    Scene_Game() = default;
    Scene_Game(SceneManager* pSceneManager);
    ~Scene_Game()override;
    void Initialize()override;
    void Finalize()override;
    void Update()override;
    void Draw()override;
    void BeginFrame()override;
    void EndFrame()override;

private:


private:

    std::unique_ptr<FollowCamera> followCamera_;

    std::unique_ptr<FieldEditor> fieldEditor_;
};