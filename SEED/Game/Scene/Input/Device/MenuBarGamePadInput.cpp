#include "MenuBarGamePadInput.h"
//============================================================================
//	include
//============================================================================

#include <SEED/Source/Manager/InputManager/InputManager.h>
//============================================================================
// enuBarGamePadInput classMethods
//============================================================================

float MenuBarGamePadInput::GetVector(PauseMenuInputAction axis) const {
    
    float vector = 0.0f;
    if (axis == PauseMenuInputAction::MoveY) {
        if (Input::IsTriggerStick(LR::LEFT,DIRECTION4::UP) || Input::IsTriggerPadButton(PAD_BUTTON::UP)) {
            vector -= 1.0f;
        } else if (Input::IsTriggerStick(LR::LEFT,DIRECTION4::DOWN) || Input::IsTriggerPadButton(PAD_BUTTON::DOWN)) {
            vector += 1.0f;
        }
        return vector;
    }
    return vector;
}

bool MenuBarGamePadInput::IsPressed(PauseMenuInputAction button) const {
    
    switch (button) {
    case PauseMenuInputAction::Pause: {
        return Input::IsPressPadButton(PAD_BUTTON::START);
    }
    case PauseMenuInputAction::Enter: {
        return Input::IsPressPadButton(PAD_BUTTON::A);
    }
    }
    return false;
}

bool MenuBarGamePadInput::IsTriggered(PauseMenuInputAction button) const {
    
    switch (button) {
    case PauseMenuInputAction::Pause: {
        return Input::IsTriggerPadButton(PAD_BUTTON::START);
    }
    case PauseMenuInputAction::Enter: {
        return Input::IsTriggerPadButton(PAD_BUTTON::A);
    }
    }
    return false;
}
