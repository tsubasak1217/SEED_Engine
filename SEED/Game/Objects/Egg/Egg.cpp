#include "Egg.h"

// State
#include "Egg/State/EggState_Follow.h"

//Engine
#include "ClockManager.h"

#include "MyMath.h"

// Object
#include "Model.h"

Egg::Egg(BaseObject* _player)
    :BaseCharacter(),
    player_(_player){
    className_ = "Egg";
}

Egg::~Egg(){}

void Egg::Initialize(){
    // model 読み込み
    model_ = std::make_unique<Model>("Assets/cube.obj");

    currentState_ = std::make_unique<EggState_Follow>(this,player_);
}

void Egg::Update(){
    BaseCharacter::Update();
}

void Egg::ThrowThis(){
    isThrowed_  = true;
}