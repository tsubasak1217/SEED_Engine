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
//camera
#include "FollowCamera.h"

// objects
#include "Player/Player.h"
#include "Egg/Manager/EggManager.h"

//manager
#include "PlayerCorpse/Manager/PlayerCorpseManager.h"
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
    std::unique_ptr<PlayerCorpseManager> playerCorpseManager_ = nullptr;

    std::unique_ptr<EggManager> eggManager_ = nullptr;

    std::unique_ptr<FollowCamera> followCamera_ = nullptr;

};