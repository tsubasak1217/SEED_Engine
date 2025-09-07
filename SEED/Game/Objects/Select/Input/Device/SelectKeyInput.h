#pragma once

//============================================================================
//	include
//============================================================================
#include <SEED/Lib/Input/IInputDevice.h>
#include <Game/Objects/Select/Input/SelectInputEnum.h>

//============================================================================
//	SelectKeyInput class
//============================================================================
class SelectKeyInput :
    public IInputDevice<SelectInputEnum> {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	SelectKeyInput() = default;
	~SelectKeyInput() = default;

	//--------- accessor -----------------------------------------------------

    // 連続入力
    float GetVector(SelectInputEnum axis)  const override;

    // 単入力
    bool IsPressed(SelectInputEnum button) const override;
    bool IsTriggered(SelectInputEnum button) const override;
};