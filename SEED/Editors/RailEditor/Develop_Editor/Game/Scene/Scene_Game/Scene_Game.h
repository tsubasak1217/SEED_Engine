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
#include "RailInfo.h"
#include "RailCamera.h"
#include "Player.h"
#include "Informations/PlayerGage.h"
#include "Obstacles.h"

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

    // 解像度
    float resolutionRate_ = 1.0f;
    float preRate_ = resolutionRate_;

    // 天球
    std::unique_ptr<Model>skySphere_ = nullptr;

    // レールカメラ
    std::unique_ptr<RailInfo> railInfo_ = nullptr;
    std::unique_ptr<RailCamera> railCamera_ = nullptr;

    // プレイヤー
    std::unique_ptr<Player> player_ = nullptr;

    // 障害物たち
    std::unique_ptr<Obstacles> obstacles_ = nullptr;

    // ゲージ
    std::unique_ptr<PlayerGage> playerGage_ = nullptr;
};