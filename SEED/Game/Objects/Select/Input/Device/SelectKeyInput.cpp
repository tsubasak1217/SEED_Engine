#include "SelectKeyInput.h"

//============================================================================
//	include
//============================================================================
#include <SEED/Source/Manager/InputManager/InputManager.h>

//============================================================================
//	SelectKeyInput classMethods
//============================================================================

// 使わない
float SelectKeyInput::GetVector(SelectInputEnum axis) const {
    axis;
    return 0.0f;
}

bool SelectKeyInput::IsPressed(SelectInputEnum button) const {

    switch (button) {
    case SelectInputEnum::Decide: {

        return Input::IsPressKey({ DIK_SPACE });
    }
    case SelectInputEnum::Right: {

        return Input::IsPressKey({ DIK_D, DIK_RIGHT });
    }
    case SelectInputEnum::Left: {

        return Input::IsPressKey({ DIK_A, DIK_LEFT });
    }
    case SelectInputEnum::Return: {

        return Input::IsPressKey({ DIK_ESCAPE });
    }
    }
    return false;
}

bool SelectKeyInput::IsTriggered(SelectInputEnum button) const {

    switch (button) {
    case SelectInputEnum::Decide: {

        return Input::IsTriggerKey({ DIK_SPACE });
    }
    case SelectInputEnum::Right: {

        return Input::IsTriggerKey({ DIK_D, DIK_RIGHT });
    }
    case SelectInputEnum::Left: {

        return Input::IsTriggerKey({ DIK_A, DIK_LEFT });
    }
    case SelectInputEnum::Return: {

        return Input::IsTriggerKey({ DIK_ESCAPE });
    }
    }
    return false;
}
