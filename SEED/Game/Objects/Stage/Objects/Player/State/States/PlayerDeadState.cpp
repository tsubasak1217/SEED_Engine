#include "PlayerDeadState.h"

void PlayerDeadState::Enter([[maybe_unused]] Player& player) {

    isDead_ = true;
}


void PlayerDeadState::Update([[maybe_unused]] Player& player) {


}

void PlayerDeadState::Exit([[maybe_unused]] Player& player) {


}

void PlayerDeadState::Edit([[maybe_unused]] const Player& player) {

}

void PlayerDeadState::FromJson([[maybe_unused]] const nlohmann::json& data) {


}

void PlayerDeadState::ToJson([[maybe_unused]] nlohmann::json& data) {


}