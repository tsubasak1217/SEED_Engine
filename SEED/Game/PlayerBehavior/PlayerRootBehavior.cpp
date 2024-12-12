#include "PlayerRootBehavior.h"

// stl
#include <cmath>

// Engine
#include "ClockManager.h"
#include "InputManager.h"
#include "MyMath.h"

// objects
#include "../Player/Player.h"

// structs
#include "Vector2.h"

PlayerRootBehavior::PlayerRootBehavior(Player* _player)
    :IPlayerBehavior(_player){}

PlayerRootBehavior::~PlayerRootBehavior(){}

void PlayerRootBehavior::Update(){
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
        player_->SetRotateY(atan2f(inputMoveDirection.y,inputMoveDirection.x));
    }

    // 移動
    {
        const Vector3& playerPos = player_->GetTranslate();
        Vector3 velocity = inputMoveDirection * (moveSpeed_ * ClockManager::DeltaTime());
        player_->SetTranslate(playerPos + velocity);
    }
}
