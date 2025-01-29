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
#include "../Game/Objects/FieldObject/Door/DoorProximityChecker.h"

// camera
#include "FollowCamera.h"

//editor
#include "../Game/Editor/EnemyEditor.h"

// objects
#include "../Game/Objects/Player/Player.h"
#include "../Game/Objects/Enemy/Enemy.h"
#include "Egg/Manager/EggManager.h"

// manager
#include "Player/PredationRange/PredationRange.h"
#include "../Game/Manager/EnemyManager.h"
#include "PlayerCorpse/Manager/PlayerCorpseManager.h"
#include "../Game/Manager/StageManager.h"

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
    StageManager* Get_pStageManager(){ return stageManager_.get(); }
    StageManager& Get_StageManager(){ return *stageManager_; }
    FollowCamera* Get_pCamera(){ return followCamera_.get(); }
    Player* Get_pPlayer(){ return player_.get(); }
    void SetIsPaused(bool isPaused){ isPaused_ = isPaused; }

private:

    // flags
    bool isPaused_ = false;

    // EngineObjects
    std::unique_ptr<DirectionalLight> directionalLight_ = nullptr;
    std::vector<std::unique_ptr<PointLight>> pointLights_;
    std::vector<std::unique_ptr<SpotLight>> spotLights_;
    std::unique_ptr<FollowCamera> followCamera_ = nullptr;

    // GameObjects
    std::unique_ptr<Player> player_ = nullptr;
    std::unique_ptr<PlayerCorpseManager> playerCorpseManager_ = nullptr;
    std::unique_ptr<EggManager> eggManager_ = nullptr;
    std::unique_ptr<Model> ground_;
    std::unique_ptr<Model> cylinderWall_[3];
    std::unique_ptr<Quad> underCloud_;
    Vector3 cloudUV_translate_;

    // Manager
    std::unique_ptr<EnemyEditor> enemyEditor_ = nullptr;
    EventManager eventManager_;
    std::unique_ptr<StageManager> stageManager_;
    std::unique_ptr<DoorProximityChecker> doorProximityChecker_;

    // Sprite
    std::unique_ptr<Sprite> backSprite_ = nullptr;

};