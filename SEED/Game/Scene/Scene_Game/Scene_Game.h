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

#include "../Player/Player.h"
#include "../Egg/EggManager.h"

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

private:
    // Objects
    std::unique_ptr<Player> player_ = nullptr;
    std::unique_ptr<EggManager> eggManager_ = nullptr;

    //editor
    std::unique_ptr<FieldEditor> fieldEditor_ = nullptr;
};