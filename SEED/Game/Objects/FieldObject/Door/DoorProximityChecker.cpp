#include "DoorProximityChecker.h"

// local
#include "../../Player/Player.h"
#include "FieldObject_Door.h"

// engine
#include "../../Manager/EventManager/EventManager.h"
#include "../../Manager/StageManager.h"

//lib
#include "../lib/Functions/myFunc/LocalFunc.h"

DoorProximityChecker::DoorProximityChecker(
    EventManager& eventManager,
    StageManager& fieldObjectManager,
    Player& player
)
    : eventManager_(eventManager)
    , stageManager_(fieldObjectManager)
    , player_(player){}

void DoorProximityChecker::Update(){
    CheckAndNotify();
}

void DoorProximityChecker::CheckAndNotify(){
    Vector3 playerPos = player_.GetWorldTranslate();

    std::vector<FieldObject_Door*> doors = stageManager_.GetStages()[stageManager_.GetCurrentStageNo()]->GetObjectsOfType<FieldObject_Door>();
    for (auto& door : doors){
        if (!door) continue;  // ドアがnullptrならスキップ

        // スイッチを持っているドアはプレイヤーの距離による開閉対象外にする
        if (door->GetHasActivator()){
            continue;
        }

        Vector3 doorPos = door->GetModel()->GetWorldTranslate();
        Vector3 diff = playerPos - doorPos;
        float distSq = LengthSquared(diff);
        float limitSq = checkRadius_ * checkRadius_;

        // プレイヤーが一定距離内にいるかどうかで処理を分岐
        if (distSq <= limitSq){
            // ドアが閉じていれば開くイベントを通知
            if (!door->GetIsOpened()){
                eventManager_.Notify("SwitchActivated", door);
            }
        } else{
            // ドアが開いていれば閉じるイベントを通知
            if (door->GetIsOpened()){
                eventManager_.Notify("SwitchDeactivated", door);
            }
        }
    }
}
