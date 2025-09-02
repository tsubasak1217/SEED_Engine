#pragma once

//============================================================================
//	include
//============================================================================
#include <SEED/Lib/Input/IInputDevice.h>
#include <Game/Objects/Stage/Objects/Player/Input/PlayerInputActionEnum.h>

//============================================================================
//	PlayerKeyInput class
//============================================================================
class PlayerKeyInput :
    public IInputDevice<PlayerInputAction> {
public:
	//========================================================================
	//	public Methods
	//========================================================================

    PlayerKeyInput() = default;
    ~PlayerKeyInput() = default;

	//--------- accessor -----------------------------------------------------

    // 連続入力
    float GetVector(PlayerInputAction axis)  const override;

    // 単入力
    bool IsPressed(PlayerInputAction button) const override;
    bool IsTriggered(PlayerInputAction button) const override;
};