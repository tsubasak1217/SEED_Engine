#include "OpeningState.h"
#include "OpenedState.h"
#include "../FieldObject_Door.h"

void OpeningState::Enter([[maybe_unused]] FieldObject_Door* door){
    // 開始時の処理（必要に応じて）
}

void OpeningState::Update(FieldObject_Door* door, float deltaTime){
    float& currentY = door->GetModel()->translate_.y;
    currentY += door->GetOpenSpeed() * deltaTime;   // 上がっていく
    if (currentY >= door->GetMaxOpenHeight()){
        currentY = door->GetMaxOpenHeight();
        door->ChangeState(new OpenedState());
    }
}

void OpeningState::Exit([[maybe_unused]] FieldObject_Door* door){
    // 終了時の処理（必要に応じて）
}
