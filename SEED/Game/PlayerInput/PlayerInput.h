#pragma once

///local
//lib
#include "InputManager.h"

//math
#include "Vector2.h"

// keyboard か Pad か
enum class KeyOrPad{
    KEY,
    PAD
};

/// <summary>
/// 各操作ごとの入力をまとめた構造体
/// </summary>
namespace PlayerInput{
    namespace CharacterMove{
        static Vector2 GetCharacterMoveDirection();
        static bool Jump();
        static bool Hovering();

        static bool FocusEggInput();
        static bool CancelFocusEgg(bool siwtchIsPress);
        static bool ThrowEgg();
        static bool ThrowEggOnFocus(bool siwtchIsPress);

        static bool Eat();

        static bool FastForwardEggTimer();

        static bool GoNextStage();
    };

    namespace Pause{
        static bool Pause();
        static bool Exit();

        static bool addItemIndex();
        static bool subItemIndex();

        static bool ExecuteItem();
    }

    namespace StageSelect{
        static bool addStageNum();
        static bool subStageNum();
        static bool DecideStage();
    };

    namespace TitleScene{
        static bool IsStartGame();
    };
};