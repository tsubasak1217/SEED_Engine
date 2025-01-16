#include "DoorProximityChecker.h"

// local
#include "../../Player/Player.h"
#include "FieldObject_Door.h"

// engine
#include "../../Manager/EventManager/EventManager.h"
#include "../../Manager/Stage.h"

//lib
#include "../lib/Functions/myFunc/LocalFunc.h"

DoorProximityChecker::DoorProximityChecker(
    EventManager& eventManager,
    Stage& fieldObjectManager,
    Player& player
)
    : eventManager_(eventManager)
    , fieldObjectManager_(fieldObjectManager)
    , player_(player){}

void DoorProximityChecker::Update(){
    CheckAndNotify();
}

void DoorProximityChecker::CheckAndNotify(){
    Vector3 playerPos = player_.GetWorldTranslate();

    auto& objects = fieldObjectManager_.GetObjects();
    for (auto& obj : objects){
        FieldObject_Door* door = dynamic_cast< FieldObject_Door* >(obj.get());
        if (!door) continue;

        Vector3 doorPos = door->GetModel()->GetWorldTranslate();
        Vector3 diff = playerPos - doorPos;
        float distSq = LengthSquared(diff);
        float limitSq = checkRadius_ * checkRadius_;

        // ====================
        //  distSq <= limitSq: プレイヤーが一定距離内にいる → ドアを開く
        //  distSq >  limitSq: プレイヤーが遠い → ドアを閉じる
        // ====================
        if (distSq <= limitSq){
            // ドアがまだ閉じていれば、「開く」イベントを発行
            if (!door->GetIsOpened()){
                eventManager_.Notify("DoorShouldOpen", door);
            }
        } else{
            // ドアがまだ開いていれば、「閉じる」イベントを発行
            if (door->GetIsOpened()){
                eventManager_.Notify("DoorShouldClose", door);
            }
        }
    }
}