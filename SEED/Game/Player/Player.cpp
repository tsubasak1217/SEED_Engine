#include "Player.h"

// Engine
#include "InputManager.h"

// Behaviors
#include "../PlayerBehavior/IPlayerBehavior.h"
#include "../PlayerBehavior/PlayerRootBehavior.h"

// Objects
#include "../Egg/EggManager.h"
#include "Model.h"

// math
#include "Matrix4x4.h"
#include "MatrixFunc.h"
#include "MyMath.h"

Player::Player()
    : GameObject("Player"){}

Player::~Player(){}

void Player::Initialize(){
    // isAlive 初期化
    isAlive_ = true;

    // model 読み込み
    model_ = std::make_unique<Model>("teapot.obj");

    // Quaternion を off に
    model_->isRotateWithQuaternion_ = false;

    // 初期 Behavior を セット
    currentBehavior_ = std::make_unique<PlayerRootBehavior>(this);
}

void Player::Update(){
    if(!isAlive_){
        return;
    }
    currentBehavior_->Update();

    model_->Update();
}

void Player::RotateUpdate(){
    // 入力された値を 取得 & Player の 方向 を 更新
    Vector3 inputMoveDirection = {0.0f,0.0f,0.0f};
    {
        // 入力された値を 取得 
        Vector2 padIntputDirection = Input::GetStickValue(LR::LEFT);
        inputMoveDirection  =  Vector3(padIntputDirection.x,0.0f,padIntputDirection.y);
        inputMoveDirection += Vector3(
            (float)Input::IsPressKey(DIK_D) - (float)Input::IsPressKey(DIK_A),
            0.0f,
            (float)Input::IsPressKey(DIK_W) - (float)Input::IsPressKey(DIK_S)
        );

        inputMoveDirection = MyMath::Normalize(inputMoveDirection);

        if(MyMath::LengthSq(inputMoveDirection) <= 0.00001f){
            return; // 動いてないから スキップ
        }

        // Player の 方向 を 更新
        model_->rotate_.y = (atan2f(inputMoveDirection.x,inputMoveDirection.y));
    }
}

EggManager* Player::GetEggManager() const{
    return eggManager_;
}

void Player::SetEggManager(EggManager* _eggManager){
    eggManager_ = _eggManager;
}

void Player::ChangeBehavior(std::unique_ptr<IPlayerBehavior> _nextBehavior){
    currentBehavior_ = std::move(_nextBehavior);
    currentBehavior_->Initialize();
}
