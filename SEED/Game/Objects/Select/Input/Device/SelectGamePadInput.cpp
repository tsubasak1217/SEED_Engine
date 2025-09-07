#include "SelectGamePadInput.h"

//============================================================================
//	SelectGamePadInput classMethods
//============================================================================
#include <SEED/Source/Manager/InputManager/InputManager.h>

//============================================================================
//	SelectGamePadInput classMethods
//============================================================================

float SelectGamePadInput::GetVector(SelectInputEnum axis) const {

    float value = Input::GetStickValue(LR::LEFT).x;
    if (std::abs(value) < std::numeric_limits<float>::epsilon()) {
        return 0.0f;
    }
    // プラス値かマイナス値で返す値を決定する
    switch (axis) {
    case SelectInputEnum::Left: {
        if (value < 0.0f) {

            return -1.0f;
        }
        break;
    }
    case SelectInputEnum::Right: {
        if (0.0f < value) {

            return 1.0f;
        }
        break;
    }
    }
    return 0.0f;
}

bool SelectGamePadInput::IsPressed(SelectInputEnum button) const {

    switch (button) {
    case SelectInputEnum::Decide: {

        return Input::IsPressPadButton({ PAD_BUTTON::A });
    }
    case SelectInputEnum::Right: {

        return Input::IsPressPadButton({ PAD_BUTTON::RIGHT });
    }
    case SelectInputEnum::Left: {

        return Input::IsPressPadButton({ PAD_BUTTON::LEFT });
    }
    case SelectInputEnum::Return: {

        return Input::IsPressPadButton({ PAD_BUTTON::START });
    }
    }
    return false;
}

bool SelectGamePadInput::IsTriggered(SelectInputEnum button) const {

    switch (button) {
    case SelectInputEnum::Decide: {

        return Input::IsTriggerPadButton({ PAD_BUTTON::A });
    }
    case SelectInputEnum::Right: {

        return Input::IsTriggerPadButton({ PAD_BUTTON::RIGHT });
    }
    case SelectInputEnum::Left: {

        return Input::IsTriggerPadButton({ PAD_BUTTON::LEFT });
    }
    case SelectInputEnum::Return: {

        return Input::IsTriggerPadButton({ PAD_BUTTON::START });
    }
    }
    return false;
}