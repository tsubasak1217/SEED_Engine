#include "PlayerSpawnFromEgg.h"

// Others Behavior
#include "PlayerRootBehavior.h"

// stl
#include <algorithm>

// Engine
#include "ClockManager.h"
#include "InputManager.h"

// Object
#include "../Player/Player.h"

#include "../Egg/Egg.h"
#include "../Egg/EggManager.h"

// math
#include "Matrix4x4.h"
#include "MatrixFunc.h"
#include "MyMath.h"

PlayerSpawnFromEgg::PlayerSpawnFromEgg(Player* _player)
    :IPlayerBehavior(_player){}

PlayerSpawnFromEgg::~PlayerSpawnFromEgg(){}

void PlayerSpawnFromEgg::Initialize(){
    auto eggManager = player_->GetEggManager();
    egg_ = eggManager->GetFrontEgg().get();

    //失敗
    if(egg_ && !egg_->GetThrow()){
        player_->ChangeBehavior(std::make_unique<PlayerRootBehavior>(player_));
        return;
    }
}

void PlayerSpawnFromEgg::Update(){

    player_->SetTranslate(egg_->GetTranslate());
    // 卵を破壊
    egg_->SetIsAlive(false);

    player_->ChangeBehavior(std::make_unique<PlayerRootBehavior>(player_));
    return;
}
