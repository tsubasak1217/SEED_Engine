#include "Player.h"

// Behaviors
#include "../PlayerBehavior/IPlayerBehavior.h"
#include "../PlayerBehavior/PlayerRootBehavior.h"

// Objects
#include "Model.h"

Player::Player()
    : GameObject("Player"){}

Player::~Player(){}

void Player::Initialize(){
    // Quaternion を off に
    model_->isRotateWithQuaternion_ = false;

    // 初期 Behavior を セット
    currentBehavior_ = std::make_unique<PlayerRootBehavior>(this);
}

void Player::Update(){
    currentBehavior_->Update();

    model_->Update();
}
