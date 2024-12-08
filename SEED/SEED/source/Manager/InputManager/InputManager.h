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

// local
#include "Vector2.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                        //
//                                               Inputで使用するenumなど                                                    //
//                                                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// パッドのボタンに名前と番号を割り当て
enum class PAD_BUTTON : uint32_t{
    UP = 0b1,
    DOWN = 0b1 << 2,
    LEFT = 0b1 << 3,
    RIGHT = 0b1 << 4,
    START = 0b1 << 5,
    BACK = 0b1 << 6,
    L_STICK = 0b1 << 7,
    R_STICK = 0b1 << 8,
    LB = 0b1 << 9,
    RB = 0b1 << 10,
    A = 0b1 << 11,
    B = 0b1 << 12,
    X = 0b1 << 13,
    Y = 0b1 << 14,
    LT = 0b1 << 15,
    RT = 0b1 << 16
};

constexpr PAD_BUTTON operator|(PAD_BUTTON lhs, PAD_BUTTON rhs){
    return PAD_BUTTON(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator&(PAD_BUTTON lhs, PAD_BUTTON rhs){
    return static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs);
}

// 比較演算子: PAD_BUTTON == uint32_t
constexpr bool operator==(PAD_BUTTON lhs, uint32_t rhs){
    return static_cast<uint32_t>(lhs) == rhs;
}

constexpr bool operator==(uint32_t lhs, PAD_BUTTON rhs){
    return lhs == static_cast<uint32_t>(rhs);
}

// 比較演算子: PAD_BUTTON != uint32_t
constexpr bool operator!=(PAD_BUTTON lhs, uint32_t rhs){
    return !(lhs == rhs);
}

constexpr bool operator!=(uint32_t lhs, PAD_BUTTON rhs){
    return !(lhs == rhs);
}


enum class LR : uint32_t{
    LEFT = 0,
    RIGHT
};


// マウスのボタン
enum class MOUSE_BUTTON : uint32_t{
    LEFT = 0,
    RIGHT,
    MIDDLE
};


// パッドの状態(今か前か)
enum class PAD_STATE : BYTE{
    CURRENT = 0,// 今のフレームの状態を取得したいとき
    BEFORE = 1// 前のフレームの状態を取得したいとき
};

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
    static bool IsTriggerKey(uint8_t key);
    static bool IsReleaseKey(uint8_t key);

    /*-------------- マウス --------------*/
    static bool IsPressMouse(MOUSE_BUTTON button);
    static bool IsTriggerMouse(MOUSE_BUTTON button);
    static bool IsReleaseMouse(MOUSE_BUTTON button);
    static Vector2 GetMouseVector();
    static Vector2 GetMouseDirection();
    static Vector2 GetMousePosition();

    /*------------ ゲームパッド -----------*/
    static bool IsPressPadButton(PAD_BUTTON button, uint8_t padNumber = 0);
    static bool IsTriggerPadButton(PAD_BUTTON button, uint8_t padNumber = 0);
    static bool IsReleasePadButton(PAD_BUTTON button, uint8_t padNumber = 0);
    static bool IsPressAnyPadButton(uint8_t padNumber = 0);
    static bool IsTriggerAnyPadButton(uint8_t padNumber = 0);

    // 左右トリガーの値を0.0 ~ 1.0で取得
    static float GetLRTriggerValue(
        LR LEFTorRIGHT, uint8_t padNumber = 0,
        PAD_STATE padState = PAD_STATE::CURRENT
    );
    // length 0 ~ 1 のベクトルで返ってくる
    static Vector2 GetStickValue(
        LR stick_LorR, uint8_t padNumber = 0,
        PAD_STATE padState = PAD_STATE::CURRENT
    );

    // スティックのデッドゾーン設定
    static void SetDeadZone(float deadZone){ instance_->deadZone_ = deadZone; }

private:

    bool IsPressPadButton(uint8_t padNumber, PAD_BUTTON button, PAD_STATE padState);

private:
    static Input* instance_;

    // キーボード入力を格納する変数
    IDirectInputDevice8* keyboard_ = nullptr;
    static const int32_t kMaxKey_ = 256;
    BYTE keys_[kMaxKey_];
    BYTE preKeys_[kMaxKey_];

    // マウス入力を格納する変数
    IDirectInputDevice8* mouse_ = nullptr;
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