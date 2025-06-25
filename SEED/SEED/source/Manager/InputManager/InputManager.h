#pragma once

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <Xinput.h>

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib, "XInput.lib")

#include <cstdint>
#include <cstring>
#include <unordered_map>
#include <string>
#include <initializer_list>

// local
#include <SEED/Lib/Tensor/Vector2.h>
#include <SEED/Source/Manager/InputManager/PadDefinitions.h>
#include <SEED/Source/Manager/InputManager/MouseDefinitions.h>
#include <SEED/Lib/enums/Direction.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                        //
//                                                   InputManager                                                         //
//                                                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Input{

private:
    Input() = default;

    // コピー禁止
    Input(const Input&) = delete;
    void operator=(const Input&) = delete;

public:
    ~Input();
    static const Input* GetInstance();
    static void Initialize();
    static void GetAllInput();

private:
    void InitializeDInput();
    void InitializeXInput();

    void GetDInputState();
    void GetXInputState();

public:// キーの状態を返す関数

    /*------------- キーボード ------------*/
    static bool IsPressKey(uint8_t key);
    static bool IsPressKey(const std::initializer_list<uint8_t>& keys);
    static bool IsPressAnyKey();
    static bool IsTriggerKey(uint8_t key);
    static bool IsTriggerKey(const std::initializer_list<uint8_t>& keys);
    static bool IsTriggerAnyKey();
    static bool IsReleaseKey(uint8_t key);
    static bool IsReleaseKey(const std::initializer_list<uint8_t>& keys);
    static bool IsReleaseAnyKey();

    /*-------------- マウス --------------*/
    static bool IsPressMouse(MOUSE_BUTTON button);
    static bool IsTriggerMouse(MOUSE_BUTTON button);
    static bool IsReleaseMouse(MOUSE_BUTTON button);
    static int32_t GetMouseWheel(INPUT_STATE inputState = INPUT_STATE::CURRENT);
    static Vector2 GetMouseVector(INPUT_STATE inputState = INPUT_STATE::CURRENT);
    static Vector2 GetMouseDirection(INPUT_STATE inputState = INPUT_STATE::CURRENT);
    static Vector2 GetMousePosition(INPUT_STATE inputState = INPUT_STATE::CURRENT);

    /*------------ ゲームパッド -----------*/
    static bool IsPressPadButton(PAD_BUTTON button, uint8_t padNumber = 0);
    static bool IsTriggerPadButton(PAD_BUTTON button, uint8_t padNumber = 0);
    static bool IsReleasePadButton(PAD_BUTTON button, uint8_t padNumber = 0);
    static bool IsPressAnyPadButton(uint8_t padNumber = 0);
    static bool IsTriggerAnyPadButton(uint8_t padNumber = 0);
    static bool IsConnectedPad(uint8_t padNumber = 0);

    // 左右トリガーの値を0.0 ~ 1.0で取得
    static float GetLRTriggerValue(
        LR LEFTorRIGHT, uint8_t padNumber = 0,
        INPUT_STATE padState = INPUT_STATE::CURRENT
    );
    // length 0 ~ 1 のベクトルで返ってくる
    static Vector2 GetStickValue(
        LR stick_LorR, uint8_t padNumber = 0,
        INPUT_STATE padState = INPUT_STATE::CURRENT
    );

    // スティックをトリガーとして取得
    static bool IsTriggerStick(
        LR stick_LorR, DIRECTION4 direction, float border = 0.5f,
        uint8_t padNumber = 0
    );

    // スティックのデッドゾーン設定
    static void SetDeadZone(float deadZone){ instance_->deadZone_ = deadZone; }

private:

    bool IsPressPadButton(uint8_t padNumber, PAD_BUTTON button, INPUT_STATE padState);

private:
    static Input* instance_;

    // キーボード入力を格納する変数
    std::unordered_map<HWND,IDirectInputDevice8*> keyboards_;
    static const int32_t kMaxKey_ = 256;
    BYTE keys_[kMaxKey_];
    BYTE preKeys_[kMaxKey_];

    // マウス入力を格納する変数
    std::unordered_map<HWND,IDirectInputDevice8*> mouses_;
    DIMOUSESTATE mouseState_;
    DIMOUSESTATE preMouseState_;

    // ゲームパッド
    XINPUT_STATE xInputState_[XUSER_MAX_COUNT];
    XINPUT_STATE preXInputState_[XUSER_MAX_COUNT];
    bool connected_[XUSER_MAX_COUNT];
    float deadZone_;

    // ボタン管理用変数
    static std::unordered_map<PAD_BUTTON, uint32_t>buttonMap_;
};