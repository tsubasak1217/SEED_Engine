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
    if (std::abs(value) < std::numeric_limits<float>::epsilon()) {
        return 0.0f;
    }
    switch (axis) {
    case PlayerInputAction::MoveX: {
        // プラス値かマイナス値で返す値を決定する
        if (value < 0.0f) {

            return -1.0f;
        } else {

            return 1.0f;
        }
    }
    }
    return 0.0f;
}

bool PlayerGamePadInput::IsPressed(PlayerInputAction button) const {

    switch (button) {
    case PlayerInputAction::Jump: {

        return Input::IsPressPadButton({ PAD_BUTTON::A });
    }
    case PlayerInputAction::PutBorder: {

        return Input::IsPressPadButton({ PAD_BUTTON::RB });
    }
    case PlayerInputAction::RemoveBorder: {

        return Input::IsPressPadButton({ PAD_BUTTON::RB });
    }
    }
    return false;
}

bool PlayerGamePadInput::IsTriggered(PlayerInputAction button) const {

    switch (button) {
    case PlayerInputAction::Jump: {

        return Input::IsTriggerPadButton({ PAD_BUTTON::A });
    }
    case PlayerInputAction::PutBorder: {

        return Input::IsTriggerPadButton({ PAD_BUTTON::RB });
    }
    case PlayerInputAction::RemoveBorder: {

        return Input::IsTriggerPadButton({ PAD_BUTTON::RB });
    }
    }
    return false;
}