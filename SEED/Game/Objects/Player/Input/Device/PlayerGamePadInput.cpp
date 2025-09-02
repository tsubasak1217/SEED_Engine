#include "PlayerGamePadInput.h"

//============================================================================
//	include
//============================================================================
#include <SEED/Source/Manager/InputManager/InputManager.h>

//============================================================================
//	PlayerGamePadInput classMethods
//============================================================================

float PlayerGamePadInput::GetVector(PlayerInputAction axis) const {

    float value = Input::GetStickValue(LR::LEFT).x;
    switch (axis) {
    case PlayerInputAction::MoveX: {

        return value;
    }
    }
    return 0.0f;
}

bool PlayerGamePadInput::IsPressed(PlayerInputAction button) const {

    switch (button) {
    case PlayerInputAction::Jump: {

        return Input::IsPressPadButton({ PAD_BUTTON::A });
    }
    }
    return false;
}

bool PlayerGamePadInput::IsTriggered(PlayerInputAction button) const {

    switch (button) {
    case PlayerInputAction::Jump: {

        return Input::IsTriggerPadButton({ PAD_BUTTON::A });
    }
    }
    return false;
}