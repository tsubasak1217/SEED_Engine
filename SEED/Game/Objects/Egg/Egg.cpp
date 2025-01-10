#include "Egg.h"

// State
#include "Egg/State/EggState_Follow.h"

//Engine
#include "ClockManager.h"

#include "MyMath.h"

// Object
#include "Model.h"
#include "Player/Player.h"

// Manager
#include "JsonManager/JsonCoordinator.h"
#include "Manager/EggManager.h"

Egg::Egg(BaseObject* _player)
    :BaseCharacter(),
    player_(_player){
    className_ = "Egg";
}

Egg::~Egg(){}

void Egg::Initialize(){
    // model 読み込み
    model_ = std::make_unique<Model>("Assets/cube.obj");

    currentState_ = std::make_unique<EggState_Follow>(this,this->player_);

    JsonCoordinator::RegisterItem("Egg","weight",weight_);
}

void Egg::Update(){
    BaseCharacter::Update();
}

void Egg::Break(){
    //Player を spawn させる
    Player* player = dynamic_cast<Player*>(player_);
    if(player_){
        player->Spawn(this->GetWorldTranslate());
    }

    isBreak_ = true;
}
