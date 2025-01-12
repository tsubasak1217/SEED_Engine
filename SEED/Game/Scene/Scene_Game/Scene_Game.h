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
#include "Manager/EventManager/EventManager.h"

// camera
#include "FollowCamera.h"
#include "../Game/Objects/Player/Player.h"
#include "../Game/Objects/Enemy/Enemy.h"
#include "../Game/Manager/EnemyManager.h"
#include "../Game/Editor/EnemyEditor.h"

// objects
#include "Player/Player.h"
#include "Egg/Manager/EggManager.h"

// manager
#include "PlayerCorpse/Manager/PlayerCorpseManager.h"
#include "../Game/Editor/FieldEditor.h"
#include "../Game/Manager/FieldObjectManager.h"

class Scene_Game : public Scene_Base
{

public:
    Scene_Game() = default;
    Scene_Game(SceneManager *pSceneManager);
    ~Scene_Game() override;
    void Initialize() override;
    void Finalize() override;
    void Update() override;
    void Draw() override;
    void BeginFrame() override;
    void EndFrame() override;

private:
    // Objects
    std::unique_ptr<Player> player_ = nullptr;
    std::unique_ptr<PlayerCorpseManager> playerCorpseManager_ = nullptr;

    std::unique_ptr<EggManager> eggManager_ = nullptr;

    // Enemy
    std::unique_ptr<EnemyManager> enemyManager_ = nullptr;
    std::unique_ptr<EnemyEditor> enemyEditor_ = nullptr;

    std::unique_ptr<ColliderEditor> fieldColliderEditor_;
    std::unique_ptr<FollowCamera> followCamera_ = nullptr;

    // Field
    EventManager eventManager_;
    std::unique_ptr<FieldEditor> fieldEditor_;
    std::unique_ptr<FieldObjectManager> fieldObjectManager_;
};