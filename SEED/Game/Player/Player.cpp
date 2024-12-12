#include "Player.h"

#include "Model.h"

Player::Player()
    : BaseObject("Player"){}

Player::~Player(){}

void Player::Initialize(){
    model_ = std::make_unique<Model>("teapot.obj");
}

void Player::Update(){}
