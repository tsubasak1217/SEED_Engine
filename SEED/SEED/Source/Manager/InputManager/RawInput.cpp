#include "RawInput.h"
#include <SEED/Source/SEED.h>

namespace SEED{
    // 入力デバイスの作成
    void RawInputMouse::Create(){
        // ウィンドウハンドル保存
        hwnd = WindowManager::GetHWND(SEED::Instance::GetInstance()->windowTitle_);

        // デバイス設定
        RAWINPUTDEVICE rid[1];
        rid[0].usUsagePage = 0x01;
        rid[0].usUsage = 0x02;
        rid[0].dwFlags = RIDEV_INPUTSINK;
        rid[0].hwndTarget = hwnd;

        // 登録
        RegisterRawInputDevices(rid, 1, sizeof(RAWINPUTDEVICE));
    }

    // 入力デバイスの破棄
    void RawInputMouse::Destroy(){
        RAWINPUTDEVICE rid;
        rid.usUsagePage = 0x01;
        rid.usUsage = 0x02;
        rid.dwFlags = RIDEV_REMOVE;
        rid.hwndTarget = NULL;

        RegisterRawInputDevices(&rid, 1, sizeof(rid));
    }

    // 状態取得
    void RawInputMouse::GetRawInputState(LPARAM lparam){
        UINT size;
        GetRawInputData((HRAWINPUT)lparam, RID_INPUT, nullptr, &size, sizeof(RAWINPUTHEADER));

        std::vector<BYTE> buffer(size);
        GetRawInputData((HRAWINPUT)lparam, RID_INPUT, buffer.data(), &size, sizeof(RAWINPUTHEADER));

        RAWINPUT* raw = (RAWINPUT*)buffer.data();

        // マウスの情報取得
        //auto& curState = state[(int)INPUT_STATE::CURRENT];

        // 移動量
        pendingDelta.delta.x += (float)raw->data.mouse.lLastX;
        pendingDelta.delta.y += (float)raw->data.mouse.lLastY;

        USHORT f = raw->data.mouse.usButtonFlags;


        // 左ボタン
        if(f & RI_MOUSE_BUTTON_1_DOWN) pendingDelta.buttons[(int)MOUSE_BUTTON::LEFT] = true;
        if(f & RI_MOUSE_BUTTON_1_UP)   pendingDelta.buttons[(int)MOUSE_BUTTON::LEFT] = false;

        // 右ボタン
        if(f & RI_MOUSE_BUTTON_2_DOWN) pendingDelta.buttons[(int)MOUSE_BUTTON::RIGHT] = true;
        if(f & RI_MOUSE_BUTTON_2_UP)   pendingDelta.buttons[(int)MOUSE_BUTTON::RIGHT] = false;

        // 中ボタン
        if(f & RI_MOUSE_BUTTON_3_DOWN) pendingDelta.buttons[(int)MOUSE_BUTTON::MIDDLE] = true;
        if(f & RI_MOUSE_BUTTON_3_UP)   pendingDelta.buttons[(int)MOUSE_BUTTON::MIDDLE] = false;

        // サイドボタン1
        if(f & RI_MOUSE_BUTTON_4_DOWN) pendingDelta.buttons[(int)MOUSE_BUTTON::X1] = true;
        if(f & RI_MOUSE_BUTTON_4_UP)   pendingDelta.buttons[(int)MOUSE_BUTTON::X1] = false;

        // サイドボタン2
        if(f & RI_MOUSE_BUTTON_5_DOWN) pendingDelta.buttons[(int)MOUSE_BUTTON::X2] = true;
        if(f & RI_MOUSE_BUTTON_5_UP)   pendingDelta.buttons[(int)MOUSE_BUTTON::X2] = false;


        // ホイール
        if(f & RI_MOUSE_WHEEL){
            SHORT delta = (SHORT)raw->data.mouse.usButtonData;
            pendingDelta.wheelDelta += delta;
        }
    }

    // リセット
    void RawInputMouse::Reset(){

        state[(int)INPUT_STATE::BEFORE] = state[(int)INPUT_STATE::CURRENT];
        state[(int)INPUT_STATE::CURRENT] = pendingDelta;
        pendingDelta.delta = Vector2(0.0f, 0.0f);
        pendingDelta.wheelDelta = 0;

    }
}