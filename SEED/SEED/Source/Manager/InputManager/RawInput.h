#pragma once
#include <Windows.h>
#include <cstdint>
#include <SEED/Lib/Tensor/Vector2.h>
#include <SEED/Source/Manager/InputManager/MouseDefinitions.h>
#include <SEED/Source/Manager/InputManager/PadDefinitions.h>

// 生のマウス入力情報(ボタンはDInputで)
struct RawInputMouseState{
    Vector2 delta;
    int32_t wheelDelta;
    bool buttons[(int)MOUSE_BUTTON::kMouseButtonCount] = { false };
};

struct RawInputMouse{
    HWND hwnd;
    RawInputMouseState state[2];// 0:今回 1:前回

private:
    RawInputMouseState pendingDelta;

public:
    // 入力デバイスの作成
    void Create();
    // 入力デバイスの破棄
    void Destroy();
    // 状態取得
    void GetRawInputState(LPARAM lparam);
    // リセット
    void Reset();
};