#include "ClosingState.h"
#include "ClosedState.h"

#include "../FieldObject_Door.h"

void ClosingState::Enter([[maybe_unused]] FieldObject_Door* door){
    // 開始時の処理（必要に応じて）
}

void ClosingState::Update(FieldObject_Door* door, float deltaTime){
    float& currentY = door->GetModel()->translate_.y;
    currentY -= door->GetOpenSpeed() * deltaTime;
    if (currentY <= 0.0f){
        currentY = 0.0f;
        door->ChangeState(new ClosedState());
    }
}

void ClosingState::Exit([[maybe_unused]] FieldObject_Door* door){
    // 終了時の処理（必要に応じて）
}