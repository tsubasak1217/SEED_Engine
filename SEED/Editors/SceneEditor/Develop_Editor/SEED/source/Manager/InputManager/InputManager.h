#pragma once

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <Xinput.h>

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib, "XInput.lib")

#include <cstdint>
#include <cstring>

// local
#include "Vector2.h"

// パッドのボタンに名前と番号を割り当て
enum class PAD_BUTTON : uint32_t{
    UP = XINPUT_GAMEPAD_DPAD_UP,
    DOWN = XINPUT_GAMEPAD_DPAD_DOWN,
    LEFT = XINPUT_GAMEPAD_DPAD_LEFT,
    RIGHT = XINPUT_GAMEPAD_DPAD_RIGHT,
    START = XINPUT_GAMEPAD_START,
    BACK = XINPUT_GAMEPAD_BACK,
    L_STICK = XINPUT_GAMEPAD_LEFT_THUMB,
    R_STICK = XINPUT_GAMEPAD_RIGHT_THUMB,
    LB = XINPUT_GAMEPAD_LEFT_SHOULDER,
    RB = XINPUT_GAMEPAD_RIGHT_SHOULDER,
    A = XINPUT_GAMEPAD_A,
    B = XINPUT_GAMEPAD_B,
    X = XINPUT_GAMEPAD_X,
    Y = XINPUT_GAMEPAD_Y,
    LT = 0,
    RT = 0
};

// パッドの状態(今か前か)
enum class PAD_STATE : BYTE{
    CURRENT = 0,// 今のフレームの状態を取得したいとき
    BEFORE = 1// 前のフレームの状態を取得したいとき
};

// パッドのスティック番号
enum class PAD_STICK : BYTE{
    LEFT = 0,// 右スティック
    RIGHT = 1// 左スティック
};

class InputManager{

private:
    InputManager() = default;

    // コピー禁止
    InputManager(const InputManager&) = delete;
    void operator=(const InputManager&) = delete;

public:
    ~InputManager();
    static const InputManager* GetInstance();
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
    static bool IsTriggerKey(uint8_t key);
    static bool IsReleaseKey(uint8_t key);

    /*------------ ゲームパッド -----------*/
    static bool IsPressPadButton(PAD_BUTTON button,uint8_t padNumber = 0);
    static bool IsTriggerPadButton(PAD_BUTTON button, uint8_t padNumber = 0);
    static bool IsReleasePadButton(PAD_BUTTON button, uint8_t padNumber = 0);
    static bool IsPressAnyPadButton(PAD_BUTTON button);
    // length 0 ~ 1 のベクトルで返ってくる
    static Vector2 GetStickValue(
        PAD_STICK stick, uint8_t padNumber = 0, 
        PAD_STATE padState = PAD_STATE::CURRENT
    );
    // 方向を返す (入力がなければ0ベクトル)
    static Vector2 GetStickDirection(
        PAD_STICK stick, uint8_t padNumber = 0,
        PAD_STATE padState = PAD_STATE::CURRENT
    );

    // スティックのデッドゾーン設定
    static void SetDeadZone(float deadZone){ instance_->deadZone_ = deadZone; }

private:

    bool IsPressPadButton(uint8_t padNumber, PAD_BUTTON button, PAD_STATE padState);

private:
    static InputManager* instance_;

    // キーボード入力を格納する変数
    IDirectInputDevice8* keyboard_ = nullptr;
    static const int32_t kMaxKey_ = 256;
    BYTE keys_[kMaxKey_];
    BYTE preKeys_[kMaxKey_];

    // ゲームパッド
    XINPUT_STATE xInputState_[XUSER_MAX_COUNT];
    XINPUT_STATE preXInputState_[XUSER_MAX_COUNT];
    bool connected_[XUSER_MAX_COUNT];
    float deadZone_;
};