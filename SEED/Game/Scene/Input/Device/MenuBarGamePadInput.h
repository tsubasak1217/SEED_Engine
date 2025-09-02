#pragma once
//============================================================================
//	include
//============================================================================
#include <SEED/Lib/Input/IInputDevice.h>
#include <Game/Scene/Input/PauseMenuInputActionEnum.h>

//============================================================================
//	MenuBarGamePadInput class
//============================================================================
class MenuBarGamePadInput : public IInputDevice<PauseMenuInputAction> {
    public:
    //========================================================================
    //	public Methods
    //========================================================================
    MenuBarGamePadInput() = default;
    ~MenuBarGamePadInput() = default;
    //--------- accessor -----------------------------------------------------
    // 連続入力
    float GetVector(PauseMenuInputAction axis)  const override;
    // 単入力
    bool IsPressed(PauseMenuInputAction button) const override;
    bool IsTriggered(PauseMenuInputAction button) const override;
};

