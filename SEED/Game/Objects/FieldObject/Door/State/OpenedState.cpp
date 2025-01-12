#include "OpenedState.h"
#include "../Door.h"

void OpenedState::Enter([[maybe_unused]] Door* door){
    // ドアが完全に開いた際の処理（例：サウンド再生など）
}

void OpenedState::Update([[maybe_unused]] Door* door, [[maybe_unused]] float deltaTime){
    // 開いた状態での処理（必要に応じて）
}

void OpenedState::Exit([[maybe_unused]] Door* door){
    // ドアが他の状態に移行する際の処理
}