#include "Egg.h"

//Engine
#include "ClockManager.h"

#include "MyMath.h"

// Object
#include "Model.h"

Egg::Egg()
    :GameObject("Egg"){}

Egg::~Egg(){}

void Egg::Initialize(){
    // model 読み込み
    model_ = std::make_unique<Model>("suzanne2.obj");

    isAlive_ = true;
}

void Egg::Update(){
    if(!isAlive_){
        return;
    }

    // 投げられているなら 重力計算
    if(isThrowed_){
        { // 重力 計算
            const float kGravity = -9.8f;
            velocity_.y += kGravity * ClockManager::DeltaTime();
        }

        // maxSpeed より Velocity が 大きかったら maxSpeed 揃える
        if(MyMath::LengthSq(velocity_) <= maxSpeed_ * maxSpeed_){
            velocity_ = MyMath::Normalize(velocity_) * maxSpeed_;
        }
        // 座標計算
        model_->translate_ += velocity_;
    }


    model_->Update();
}

void Egg::ThrowThis(const Vector3& _velocity){
    isThrowed_  = true;
    velocity_ = _velocity;
}