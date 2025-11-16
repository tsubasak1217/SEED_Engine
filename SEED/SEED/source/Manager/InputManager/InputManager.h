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
#include <Environment/Environment.h>
#include <SEED/Lib/Tensor/Vector2.h>
#include <SEED/Source/Manager/InputManager/RawInput.h>
#include <SEED/Source/Manager/InputManager/PadDefinitions.h>
#include <SEED/Source/Manager/InputManager/MouseDefinitions.h>
#include <SEED/Lib/enums/Direction.h>
#include <SEED/Lib/Structs/Range2D.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                        //
//                                                   InputManager                                                         //
//                                                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum class InputDevice{
    KEYBOARD,
    GAMEPAD
};

class Input{
private:
    Input() = default;

    // コピー禁止
    Input(const Input&) = delete;
    void operator=(const Input&) = delete;

public:
    ~Input();
    static Input* GetInstance();
    static void Initialize();
    static void GetAllInput();
    static void EndFrame();

private:
    void InitializeRawInput();
    void InitializeDInput();
    void InitializeXInput();

    // 各種入力取得関数
    void GetRawInputState(LPARAM lparam);
    void GetDInputState();
    void GetXInputState();

    // 破棄
    void DestroyRawInput();

    // ウィンドウプロシージャにアクセス権を与える
    friend LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:// キーの状態を返す関数

    /*------------- キーボード ------------*/
    static bool IsPressKey(uint8_t key);
    static bool IsPressKey(const std::initializer_list<uint8_t>& keys);
    static bool IsPressKey(const std::vector<uint8_t>& keys);
    static bool IsPressAnyKey();
    static bool IsTriggerKey(uint8_t key);
    static bool IsTriggerKey(const std::initializer_list<uint8_t>& keys);
    static bool IsTriggerKey(const std::vector<uint8_t>& keys);
    static bool IsTriggerAnyKey();
    static bool IsReleaseKey(uint8_t key);
    static bool IsReleaseKey(const std::initializer_list<uint8_t>& keys);
    static bool IsReleaseKey(const std::vector<uint8_t>& keys);
    static bool IsReleaseAnyKey();

    /*-------------- マウス --------------*/
    static bool IsPressMouse(MOUSE_BUTTON button);
    static bool IsTriggerMouse(MOUSE_BUTTON button);
    static bool IsReleaseMouse(MOUSE_BUTTON button);
    static int32_t GetMouseWheel(INPUT_STATE inputState = INPUT_STATE::CURRENT);
    static Vector2 GetMouseVector(INPUT_STATE inputState = INPUT_STATE::CURRENT);
    static Vector2 GetMouseDirection(INPUT_STATE inputState = INPUT_STATE::CURRENT);
    static Vector2 GetMousePosition(INPUT_STATE inputState = INPUT_STATE::CURRENT);
    static bool IsMouseMoved(INPUT_STATE inputState = INPUT_STATE::CURRENT);
    static bool IsMouseInputAny();

    // カーソル関連関数
    static void RepeatCursor(const Range2D& repeatRange = {Vector2(0.0f),kWindowSize});
    static void SetMouseCursorPos(const Vector2& pos);
    static void SetMouseCursorVisible(bool isVisible);
    static void ToggleMouseCursorVisible();

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
    static float GetDeadZone() { return deadZone_; }

    // アクティブかどうか
    static void SetIsActive(bool isActive){ instance_->isActive_ = isActive; }
    static bool GetIsActive(){ return instance_->isActive_; }
    // 直近で使用した入力デバイス
    static InputDevice GetRecentInputDevice(){ return instance_->recentInputDevice_; }
    static bool IsChangedInputDevice(){ return instance_->recentInputDevice_ != instance_->prevDevice_; }
    static bool GetIsAnyInput(bool isIgnoreActiveFlag = false);

private:

    bool IsPressPadButton(uint8_t padNumber, PAD_BUTTON button, INPUT_STATE padState);
    void RepeatCursorInternal();

private:
    static Input* instance_;

    // キーボード入力を格納する変数
    std::unordered_map<HWND,IDirectInputDevice8*> keyboards_;
    static const int32_t kMaxKey_ = 256;
    BYTE keys_[kMaxKey_];
    BYTE preKeys_[kMaxKey_];

    // マウス入力を格納する変数
    RawInputMouse mouse_;
    Vector2 preMousePos_;

    // ゲームパッド
    XINPUT_STATE xInputState_[XUSER_MAX_COUNT];
    XINPUT_STATE preXInputState_[XUSER_MAX_COUNT];
    bool connected_[XUSER_MAX_COUNT];
    static float deadZone_;

    // ボタン管理用変数
    static std::unordered_map<PAD_BUTTON, uint32_t>buttonMap_;

    // アクティブかどうか
    bool isActive_ = true;

    // 直近で使用した入力デバイス
    InputDevice recentInputDevice_ = InputDevice::KEYBOARD;
    InputDevice prevDevice_ = InputDevice::KEYBOARD;

    // カーソル
    bool isCursorRepeat_ = false;
    bool isSetCursorPos_ = false;
    Vector2 cursorPos_ = { 0.0f,0.0f };
    Range2D repeatRange_;
    bool isCursorVisible_ = true;
};