#include "MenuBarKeyInput.h"
//============================================================================
//	include
//============================================================================

#include <SEED/Source/Manager/InputManager/InputManager.h>

//============================================================================
//	MenuBarKeyInput classMethods
//============================================================================

float MenuBarKeyInput::GetVector(PauseMenuInputAction axis) const {
    
    float vector = 0.0f;
    if (axis == PauseMenuInputAction::MoveY) {
        if (Input::IsTriggerKey({ DIK_W })) {

            vector -= 1.0f;
        } else if (Input::IsTriggerKey({ DIK_S })) {

            vector += 1.0f;
        }
        return vector;
    }

    return vector;
}

bool MenuBarKeyInput::IsPressed(PauseMenuInputAction button) const {
    
    switch (button) {
    case PauseMenuInputAction::Pause: {
        return Input::IsPressKey({ DIK_ESCAPE });
    }
    case PauseMenuInputAction::Enter: {
        return Input::IsPressKey({ DIK_SPACE });
    }
    }
    return false;
}

bool MenuBarKeyInput::IsTriggered(PauseMenuInputAction button) const {
    switch (button) {
    case PauseMenuInputAction::Pause: {
        return Input::IsTriggerKey({ DIK_ESCAPE });
    }
    case PauseMenuInputAction::Enter: {
        return Input::IsTriggerKey({ DIK_SPACE });
    }
    }
    return false;
}
