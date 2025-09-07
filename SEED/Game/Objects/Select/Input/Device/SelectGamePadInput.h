#pragma once

//============================================================================
//	include
//============================================================================
#include <SEED/Lib/Input/IInputDevice.h>
#include <Game/Objects/Select/Input/SelectInputEnum.h>

//============================================================================
//	SelectGamePadInput class
//============================================================================
class SelectGamePadInput :
    public IInputDevice<SelectInputEnum> {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	SelectGamePadInput() = default;
	~SelectGamePadInput() = default;

	//--------- accessor -----------------------------------------------------

    // 連続入力
    float GetVector(SelectInputEnum axis)  const override;

    // 単入力
    bool IsPressed(SelectInputEnum button) const override;
    bool IsTriggered(SelectInputEnum button) const override;
};