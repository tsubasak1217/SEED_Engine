#include "PlayerKeyInput.h"

//============================================================================
//	include
//============================================================================
#include <SEED/Source/Manager/InputManager/InputManager.h>

//============================================================================
//	PlayerKeyInput classMethods
//============================================================================

float PlayerKeyInput::GetVector(PlayerInputAction axis) const {

    float vector = 0.0f;
    if (axis == PlayerInputAction::MoveX) {
        if (Input::IsPressKey({ DIK_A })) {

            vector -= 1.0f;
        } else if (Input::IsPressKey({ DIK_D })) {

            vector += 1.0f;
        }
        return vector;
    }
    return vector;
}

bool PlayerKeyInput::IsPressed(PlayerInputAction button) const {

    switch (button) {
    case PlayerInputAction::Jump: {

        return Input::IsPressKey({ DIK_SPACE });
    }
    case PlayerInputAction::PutBorder: {

        return Input::IsPressKey({ DIK_E });
    }
    case PlayerInputAction::RemoveBorder: {

        return Input::IsPressKey({ DIK_E });
    }
    }
    return false;
}

bool PlayerKeyInput::IsTriggered(PlayerInputAction button) const {

    switch (button) {
    case PlayerInputAction::Jump: {

        return Input::IsTriggerKey({ DIK_SPACE });
    }
    case PlayerInputAction::PutBorder: {

        return Input::IsTriggerKey({ DIK_E });
    }
    case PlayerInputAction::RemoveBorder: {

        return Input::IsTriggerKey({ DIK_E });
    }
    }
    return false;
}