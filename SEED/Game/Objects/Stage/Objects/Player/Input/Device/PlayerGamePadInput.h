#pragma once

//============================================================================
//	include
//============================================================================
#include <SEED/Lib/Input/IInputDevice.h>
#include <Game/Objects/Stage/Objects/Player/Input/PlayerInputActionEnum.h>

//============================================================================
//	PlayerGamePadInput class
//============================================================================
class PlayerGamePadInput :
    public IInputDevice<PlayerInputAction> {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	PlayerGamePadInput() = default;
	~PlayerGamePadInput() = default;

	//--------- accessor -----------------------------------------------------

    // 連続入力
    float GetVector(PlayerInputAction axis)  const override;

    // 単入力
    bool IsPressed(PlayerInputAction button) const override;
    bool IsTriggered(PlayerInputAction button) const override;
};