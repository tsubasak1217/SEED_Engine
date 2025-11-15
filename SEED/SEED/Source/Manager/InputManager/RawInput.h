#pragma once
#include <Windows.h>
#include <cstdint>
#include <SEED/Lib/Tensor/Vector2.h>
#include <SEED/Source/Manager/InputManager/MouseDefinitions.h>
#include <SEED/Source/Manager/InputManager/PadDefinitions.h>

struct RawInputMouseState{
    Vector2 delta;
    int32_t wheelDelta;
    bool buttons[int(MOUSE_BUTTON::kMouseButtonCount)];
};

struct RawInputMouse{
    HWND hwnd;
    RawInputMouseState state[2];// 0:今回 1:前回

    // 入力デバイスの作成
    void Create(HWND hwnd_){
        // ウィンドウハンドル保存
        hwnd = hwnd_;

        // デバイス設定
        RAWINPUTDEVICE rid[1];
        rid[0].usUsagePage = 0x01;
        rid[0].usUsage = 0x02;  // マウス
        rid[0].dwFlags = RIDEV_INPUTSINK;
        rid[0].hwndTarget = hwnd;

        // 登録
        RegisterRawInputDevices(rid, 1, sizeof(RAWINPUTDEVICE));
    }

    // 入力デバイスの破棄
    void Destroy(){
        RAWINPUTDEVICE rid;
        rid.usUsagePage = 0x01;   // マウス
        rid.usUsage = 0x02;
        rid.dwFlags = RIDEV_REMOVE;
        rid.hwndTarget = NULL;

        RegisterRawInputDevices(&rid, 1, sizeof(rid));
    }

    // 状態リセット
    void Reset(){
        auto& curState = state[(int)INPUT_STATE::CURRENT];
        curState.delta = Vector2(0.0f);
        curState.wheelDelta = 0;
        for(int i = 0; i < int(MOUSE_BUTTON::kMouseButtonCount); i++){
            curState.buttons[i] = false;
        }
    }
};