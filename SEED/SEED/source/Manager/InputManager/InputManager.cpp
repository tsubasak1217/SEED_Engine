#include <SEED/Source/Manager/InputManager/InputManager.h>
#include <SEED/Source/SEED.h>
#include <SEED/Lib/Functions/Math.h>
#include <SEED/Source/Manager/ImGuiManager/ImGuiManager.h>

namespace SEED{
    // static変数初期化
    Input* Input::instance_ = nullptr;

    std::unordered_map<PAD_BUTTON, uint32_t>Input::buttonMap_ = {
        {PAD_BUTTON::UP,XINPUT_GAMEPAD_DPAD_UP},
        {PAD_BUTTON::DOWN,XINPUT_GAMEPAD_DPAD_DOWN},
        {PAD_BUTTON::LEFT,XINPUT_GAMEPAD_DPAD_LEFT},
        {PAD_BUTTON::RIGHT,XINPUT_GAMEPAD_DPAD_RIGHT},
        {PAD_BUTTON::START,XINPUT_GAMEPAD_START},
        {PAD_BUTTON::BACK,XINPUT_GAMEPAD_BACK},
        {PAD_BUTTON::L_STICK,XINPUT_GAMEPAD_LEFT_THUMB},
        {PAD_BUTTON::R_STICK,XINPUT_GAMEPAD_RIGHT_THUMB},
        {PAD_BUTTON::LB,XINPUT_GAMEPAD_LEFT_SHOULDER},
        {PAD_BUTTON::RB,XINPUT_GAMEPAD_RIGHT_SHOULDER},
        {PAD_BUTTON::A,XINPUT_GAMEPAD_A},
        {PAD_BUTTON::B,XINPUT_GAMEPAD_B},
        {PAD_BUTTON::X,XINPUT_GAMEPAD_X},
        {PAD_BUTTON::Y,XINPUT_GAMEPAD_Y},
        {PAD_BUTTON::LT,VK_PAD_LTRIGGER},
        {PAD_BUTTON::RT,VK_PAD_RTRIGGER}
    };
    float Input::deadZone_ = 16000.0f;

    // デストラクタ
    Input::~Input(){
        if(instance_){
            delete instance_;
            instance_ = nullptr;
        }
    }

    // 単一のインスタンスを返す関数
    Input* Input::GetInstance(){
        if(!instance_){
            instance_ = new Input();
        }

        return instance_;
    }

    /////////////////////////////////////////////////////////////////////////////////////

    /*                                   初期化関数                                      */

    /////////////////////////////////////////////////////////////////////////////////////

    void Input::Initialize(){
        // 実体がなければ作る
        GetInstance();

        /*=========== 初期化 ===========*/
    #ifdef USE_RAWINPUT// RawInput使用時
        // RawInput
        instance_->InitializeRawInput();
    #endif
        // DirectInput
        instance_->InitializeDInput();
        // XInput
        instance_->InitializeXInput();

        // マウス位置の初期化
        instance_->preMousePos_ = GetMousePosition();
    }


    // RawInputの初期化-------------------------------------------------------
    void Input::InitializeRawInput(){
        // ウィンドウハンドルの一覧
        auto hwnds = WindowManager::GetAllHWNDs();

        // デバイスの生成
        mouse_.Create();
    }

    // DirectInputの初期化----------------------------------------------------
    void Input::InitializeDInput(){
        HRESULT hr;

        // Inputオブジェクトの生成
        IDirectInput8* directInput = nullptr;
        hr = DirectInput8Create(
            WindowManager::GetHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8,
            (void**)&directInput, nullptr
        );
        assert(SUCCEEDED(hr));

        // ウィンドウハンドルの一覧
        auto hwnds = WindowManager::GetAllHWNDs();

        // デバイスの生成
        for(const auto& hwnd : hwnds){
            /*======================== キーボード ========================*/

            //  キーボードデバイスの生成
            keyboards_[hwnd] = nullptr;
            hr = directInput->CreateDevice(GUID_SysKeyboard, &keyboards_[hwnd], NULL);
            assert(SUCCEEDED(hr));

            // 入力形式のセット
            hr = keyboards_[hwnd]->SetDataFormat(&c_dfDIKeyboard);// 標準
            assert(SUCCEEDED(hr));

            // 制御レベルの設定
            hr = keyboards_[hwnd]->SetCooperativeLevel(
                hwnd,
                DISCL_FOREGROUND | DISCL_NOWINKEY | DISCL_NONEXCLUSIVE
            );
            assert(SUCCEEDED(hr));

            /*========================== マウス ==========================*/

            // マウスデバイスの生成
            hr = directInput->CreateDevice(GUID_SysMouse, &mouses_[hwnd], NULL);
            assert(SUCCEEDED(hr));

            // 入力形式のセット
            hr = mouses_[hwnd]->SetDataFormat(&c_dfDIMouse);
            assert(SUCCEEDED(hr));

            // 制御レベルの設定
            hr = mouses_[hwnd]->SetCooperativeLevel(
                hwnd,
                DISCL_FOREGROUND | DISCL_NONEXCLUSIVE
            );
        }
    }

    // XInputの初期化-----------------------------------------------------------------
    void Input::InitializeXInput(){
        for(DWORD i = 0; i < XUSER_MAX_COUNT; i++){
            ZeroMemory(&xInputState_[i], sizeof(XINPUT_STATE));
            ZeroMemory(&preXInputState_[i], sizeof(XINPUT_STATE));
            connected_[i] = false;
        }

        deadZone_ = (float)XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE / (float)SHRT_MAX;
        deadZone_ = std::clamp(deadZone_, 0.0f, 1.0f);
    }


    /////////////////////////////////////////////////////////////////////////////////////

    /*                                  入力受付関数                                     */

    /////////////////////////////////////////////////////////////////////////////////////

    void Input::GetAllInput(){

        // DirectInputの取得
        instance_->GetDInputState();
        // XInputの取得
        instance_->GetXInputState();

        // カーソル操作関連
        ClipCursor(NULL);
        if(instance_->isCursorRepeat_){
            instance_->RepeatCursorInternal();
            instance_->isCursorRepeat_ = false;
        }

        if(instance_->isSetCursorPos_){
            SetCursorPos((int)instance_->cursorPos_.x, (int)instance_->cursorPos_.y);
            instance_->isSetCursorPos_ = false;
        }

        // 最近使った入力デバイスの更新
        instance_->prevDevice_ = instance_->recentInputDevice_;
        if(IsPressAnyKey()){
            instance_->recentInputDevice_ = InputDevice::KEYBOARD;
        } else{
            if(IsPressAnyPadButton() or Methods::Math::HasLength(GetStickValue(Enums::LR::LEFT)) or Methods::Math::HasLength(GetStickValue(Enums::LR::RIGHT))){
                instance_->recentInputDevice_ = InputDevice::GAMEPAD;
            }
        }
    }

    // RawInputの状態取得------------------------------------------------------
    void Input::GetRawInputState(LPARAM lparam){
        mouse_.GetRawInputState(lparam);
    }

    // DirectInputの状態取得-----------------------------------------------------
    void Input::GetDInputState(){

        // 現在フォーカスされているウィンドウを取得
        HWND activeWindow = GetForegroundWindow();
        if(keyboards_.find(activeWindow) == keyboards_.end()){
            // フォーカスされているウィンドウがない場合は何もしない
            return;
        }

        // DirectInputのキーボード情報取得開始
        keyboards_[activeWindow]->Acquire();
        // 前のフレームのキー情報を保存
        std::memcpy(preKeys_, keys_, sizeof(BYTE) * kMaxKey_);
        // 現在の全キーの状態を取得する
        keyboards_[activeWindow]->GetDeviceState(sizeof(keys_), keys_);

        // マウスの情報取得開始
        mouses_[activeWindow]->Acquire();
        // 前のフレームのマウス情報を保存
        std::memcpy(&mouseStates_[(int)INPUT_STATE::BEFORE], &mouseStates_[(int)INPUT_STATE::CURRENT], sizeof(DIMOUSESTATE));
        // 現在のマウス状態を取得
        mouses_[activeWindow]->GetDeviceState(sizeof(DIMOUSESTATE), &mouseStates_[(int)INPUT_STATE::CURRENT]);
    }

    // XInputの状態取得--------------------------------------------------------
    void Input::GetXInputState(){
        for(DWORD i = 0; i < XUSER_MAX_COUNT; i++){

            // 前フレームの状態保存
            preXInputState_[i] = xInputState_[i];

            // 現在のフレームの状態取得
            DWORD result = XInputGetState(i, &xInputState_[i]);

            // 接続しているかどうか
            if(result == ERROR_SUCCESS){
                connected_[i] = true;
            } else{
                connected_[i] = false;
            }
        }
    }


    /////////////////////////////////////////////////////////////////////////////////////

    /*                              フレーム終了時関数                                    */

    /////////////////////////////////////////////////////////////////////////////////////

    void Input::EndFrame(){
        instance_->preMousePos_ = GetMousePosition();

        // フレームの終了時に初期化
        instance_->mouse_.Reset();
    }

    // RawInputの破棄-----------------------------------------------------------
    void Input::DestroyRawInput(){
        mouse_.Destroy();
    }


    /////////////////////////////////////////////////////////////////////////////////////

    /*                         指定したキーの入力の状態を返す関数                            */

    /////////////////////////////////////////////////////////////////////////////////////


    /********************* 外部から使えるやつ *******************/


    //--------------------------------- キーボード -----------------------------------------//

    bool Input::IsPressKey(uint8_t key){
        return instance_->keys_[key] * instance_->isActive_;
    }

    bool Input::IsPressKey(const std::initializer_list<uint8_t>& keys){
        for(auto& key : keys){
            if(instance_->keys_[key]){ return true * instance_->isActive_; }
        }

        return false;
    }

    bool Input::IsPressKey(const std::vector<uint8_t>& keys){
        for(auto& key : keys){
            if(instance_->keys_[key]){ return true * instance_->isActive_; }
        }
        return false;
    }

    bool Input::IsPressAnyKey(){
        for(int i = 0; i < kMaxKey_; i++){
            if(instance_->keys_[i]){ return true * instance_->isActive_; }
        }

        return false;
    }

    bool Input::IsTriggerKey(uint8_t key){
        return (instance_->keys_[key] && !instance_->preKeys_[key]) * instance_->isActive_;
    }

    bool Input::IsTriggerKey(const std::initializer_list<uint8_t>& keys){
        for(auto& key : keys){
            if(instance_->keys_[key] && !instance_->preKeys_[key]){ return true * instance_->isActive_; }
        }

        return false;
    }

    bool Input::IsTriggerKey(const std::vector<uint8_t>& keys){
        for(auto& key : keys){
            if(instance_->keys_[key] && !instance_->preKeys_[key]){ return true * instance_->isActive_; }
        }
        return false;
    }

    bool Input::IsTriggerAnyKey(){
        for(int i = 0; i < kMaxKey_; i++){
            if(instance_->keys_[i] && !instance_->preKeys_[i]){ return true * instance_->isActive_; }
        }

        return false;
    }

    bool Input::IsReleaseKey(uint8_t key){
        return !instance_->keys_[key] && instance_->preKeys_[key];
    }

    bool Input::IsReleaseKey(const std::initializer_list<uint8_t>& keys){
        for(auto& key : keys){
            if(!instance_->keys_[key] && instance_->preKeys_[key]){ return true * instance_->isActive_; }
        }

        return false;
    }

    bool Input::IsReleaseKey(const std::vector<uint8_t>& keys){
        for(auto& key : keys){
            if(!instance_->keys_[key] && instance_->preKeys_[key]){ return true * instance_->isActive_; }
        }
        return false;
    }

    bool Input::IsReleaseAnyKey(){
        for(int i = 0; i < kMaxKey_; i++){
            if(!instance_->keys_[i] && instance_->preKeys_[i]){ return true * instance_->isActive_; }
        }

        return false;
    }


    //----------------------------------- マウス -------------------------------------------//

    // マウスのボタンが押されているかどうか
    bool Input::IsPressMouse(MOUSE_BUTTON button){
        // 例外処理
        if(!instance_->isActive_){ return false; }

    #ifdef USE_RAWINPUT// RawInput使用時
        if(button == MOUSE_BUTTON::kMouseButtonCount){ return false; }
        // 押されているかどうか
        return instance_->mouse_.state[(int)INPUT_STATE::CURRENT].buttons[(int)button];

    #else// DirectInput使用時
        switch(button){
        case MOUSE_BUTTON::LEFT:
            return instance_->mouseStates_[(int)INPUT_STATE::CURRENT].rgbButtons[0] & 0x80;
        case MOUSE_BUTTON::RIGHT:
            return instance_->mouseStates_[(int)INPUT_STATE::CURRENT].rgbButtons[1] & 0x80;
        case MOUSE_BUTTON::MIDDLE:
            return instance_->mouseStates_[(int)INPUT_STATE::CURRENT].rgbButtons[2] & 0x80;
        default:
            return false;
        }
    #endif
    }

    // マウスのボタンが押された瞬間
    bool Input::IsTriggerMouse(MOUSE_BUTTON button){
        // 例外処理
        if(!instance_->isActive_){ return false; }
        if(button == MOUSE_BUTTON::kMouseButtonCount){ return false; }

    #ifdef USE_RAWINPUT// RawInput使用時
        // 押されていて、前のフレームでは押されていなかったらtrue
        return instance_->mouse_.state[(int)INPUT_STATE::CURRENT].buttons[(int)button] &&
            !instance_->mouse_.state[(int)INPUT_STATE::BEFORE].buttons[(int)button];

    #else// DirectInput使用時
        switch(button){
        case MOUSE_BUTTON::LEFT:
            return (instance_->mouseStates_[(int)INPUT_STATE::CURRENT].rgbButtons[0] & 0x80) &&
                !(instance_->mouseStates_[(int)INPUT_STATE::BEFORE].rgbButtons[0] & 0x80);
        case MOUSE_BUTTON::RIGHT:
            return (instance_->mouseStates_[(int)INPUT_STATE::CURRENT].rgbButtons[1] & 0x80) &&
                !(instance_->mouseStates_[(int)INPUT_STATE::BEFORE].rgbButtons[1] & 0x80);
        case MOUSE_BUTTON::MIDDLE:
            return (instance_->mouseStates_[(int)INPUT_STATE::CURRENT].rgbButtons[2] & 0x80) &&
                !(instance_->mouseStates_[(int)INPUT_STATE::BEFORE].rgbButtons[2] & 0x80);
        default:
            return false;
        }
    #endif
    }

    // マウスのボタンが離された瞬間かどうか
    bool Input::IsReleaseMouse(MOUSE_BUTTON button){
        // 例外処理
        if(!instance_->isActive_){ return false; }
        if(button == MOUSE_BUTTON::kMouseButtonCount){ return false; }

    #ifdef USE_RAWINPUT// RawInput使用時
        // 離されていて、前のフレームでは押されていたらtrue
        return !instance_->mouse_.state[(int)INPUT_STATE::CURRENT].buttons[(int)button] &&
            instance_->mouse_.state[(int)INPUT_STATE::BEFORE].buttons[(int)button];

    #else// DirectInput使用時
        switch(button){
        case MOUSE_BUTTON::LEFT:
            return !(instance_->mouseStates_[(int)INPUT_STATE::CURRENT].rgbButtons[0] & 0x80) &&
                (instance_->mouseStates_[(int)INPUT_STATE::BEFORE].rgbButtons[0] & 0x80);
        case MOUSE_BUTTON::RIGHT:
            return !(instance_->mouseStates_[(int)INPUT_STATE::CURRENT].rgbButtons[1] & 0x80) &&
                (instance_->mouseStates_[(int)INPUT_STATE::BEFORE].rgbButtons[1] & 0x80);
        case MOUSE_BUTTON::MIDDLE:
            return !(instance_->mouseStates_[(int)INPUT_STATE::CURRENT].rgbButtons[2] & 0x80) &&
                (instance_->mouseStates_[(int)INPUT_STATE::BEFORE].rgbButtons[2] & 0x80);
        default:
            return false;
        }
    #endif
    }

    // マウスのホイールの回転量を取得
    int32_t Input::GetMouseWheel(INPUT_STATE inputState){
        if(!instance_->isActive_){ return 0; }

    #ifdef USE_RAWINPUT// RawInput使用時
        return instance_->mouse_.state[(int)inputState].wheelDelta;

    #else// DirectInput使用時
        return instance_->mouseStates_[(int)inputState].lZ;
    #endif
    }

    // マウスの移動量を取得
    Vector2 Input::GetMouseVector(INPUT_STATE inputState){
        if(!instance_->isActive_){ return Vector2(0.0f, 0.0f); }

    #ifdef USE_RAWINPUT// RawInput使用時
        return instance_->mouse_.state[(int)inputState].delta;

    #else// DirectInput使用時
        DIMOUSESTATE& state = instance_->mouseStates_[(int)inputState];
        return Vector2((float)state.lX, (float)state.lY);
    #endif
    }

    // マウスの方向を取得
    Vector2 Input::GetMouseDirection(INPUT_STATE inputState){

        Vector2 vec = GetMouseVector(inputState);
        if(Methods::Math::Length(vec)){
            return Methods::Math::Normalize(vec);
        }

        return Vector2();
    }

    // マウスの座標を取得
    Vector2 Input::GetMousePosition(INPUT_STATE inputState){

        if(inputState == INPUT_STATE::CURRENT){
            // マウスの座標を取得
            POINT point;
            HWND activeWindow = GetForegroundWindow();
            GetCursorPos(&point);
            ScreenToClient(activeWindow, &point);
            Vector2 result = { (float)point.x,(float)point.y };

            // 比率をかけてもとに戻す
            Vector2 adjustScale = Vector2(1.0f, 1.0f) / WindowManager::GetWindowScale(SEED::Instance::GetWindowTitle());

            return result * adjustScale;
        } else{
            return instance_->preMousePos_;
        }
    }

    // マウスが移動したか
    bool Input::IsMouseMoved(INPUT_STATE inputState){
        return Methods::Math::LengthSq(GetMouseVector(inputState)) > 0.0f;
    }

    // マウスのボタンが何かしら押されているか
    bool Input::IsMouseInputAny(){
        if(!instance_->isActive_){ return false; }

    #ifdef USE_RAWINPUT// RawInput使用時
        auto& mouse = instance_->mouse_.state[(int)INPUT_STATE::CURRENT];
        for(int i = 0; i < (int)MOUSE_BUTTON::kMouseButtonCount; i++){
            if(mouse.buttons[i]){
                return true;
            }
        }

    #else// DirectInput使用時
        auto& mouseState = instance_->mouseStates_[(int)INPUT_STATE::CURRENT];
        for(int i = 0; i < 3; i++){
            if(mouseState.rgbButtons[i] & 0x80){
                return true;
            }
        }
    #endif

        return false;
    }


    /*------------------ マウスカーソルをリピートさせる関数 ------------------*/

    void Input::RepeatCursor(const Range2D& repeatRange){
        instance_->isCursorRepeat_ = true;
        instance_->repeatRange_ = repeatRange;
    }

    void Input::SetMouseCursorPos(const Vector2& pos){
        instance_->isSetCursorPos_ = true;
        instance_->cursorPos_ = pos;
    }

    void Input::SetMouseCursorVisible(bool isVisible){
        if(instance_->isCursorVisible_ == isVisible){ return; }
        instance_->isCursorVisible_ = isVisible;
        if(isVisible){
            ShowCursor(TRUE);
        } else{
            ShowCursor(FALSE);
        }
    }

    void Input::ToggleMouseCursorVisible(){
        instance_->isCursorVisible_ = !instance_->isCursorVisible_;
        if(instance_->isCursorVisible_){
            ShowCursor(TRUE);
        } else{
            ShowCursor(FALSE);
        }
    }

    //---------------------------------- ゲームパッド --------------------------------------//

    bool Input::IsPressPadButton(PAD_BUTTON button, uint8_t padNumber){
        if(!instance_->isActive_){ return false; }
        // 押されているボタンがあれば加算される
        uint32_t result = 0;

        // ビットの立っているボタンを取得
        std::vector<PAD_BUTTON>buttons;
        for(auto& [key, value] : instance_->buttonMap_){
            if((button & key) != 0){
                buttons.push_back(key);
            }
        }

        // ボタンの状態を取得
        for(auto& b : buttons){
            if(b != PAD_BUTTON::LT && b != PAD_BUTTON::RT){
                result += instance_->IsPressPadButton(padNumber, b, INPUT_STATE::CURRENT);
            } else{
                if(b == PAD_BUTTON::LT){
                    result += GetLRTriggerValue(Enums::LR::LEFT) >= 0.9f;
                } else{
                    result += GetLRTriggerValue(Enums::LR::RIGHT) >= 0.9f;
                }
            }
        }

        // 1以上ならtrue
        return result > 0;
    }


    bool Input::IsTriggerPadButton(PAD_BUTTON button, uint8_t padNumber){

        if(!instance_->isActive_){ return false; }
        // 押されているボタンがあれば加算される
        uint32_t result = 0;

        // ビットの立っているボタンを取得
        std::vector<PAD_BUTTON>buttons;
        for(auto& [key, value] : instance_->buttonMap_){
            if((button & key) != 0){
                buttons.push_back(key);
            }
        }

        // ボタンの状態を取得
        for(auto& b : buttons){
            if(b != PAD_BUTTON::LT && b != PAD_BUTTON::RT){

                result +=
                    instance_->IsPressPadButton(padNumber, b, INPUT_STATE::CURRENT) &&
                    !instance_->IsPressPadButton(padNumber, b, INPUT_STATE::BEFORE);

            } else{
                if(b == PAD_BUTTON::LT){
                    result +=
                        GetLRTriggerValue(Enums::LR::LEFT, padNumber, INPUT_STATE::CURRENT) >= 0.9f &&
                        GetLRTriggerValue(Enums::LR::LEFT, padNumber, INPUT_STATE::BEFORE) < 0.9f;
                } else{
                    result +=
                        GetLRTriggerValue(Enums::LR::RIGHT, padNumber, INPUT_STATE::CURRENT) >= 0.9f &&
                        GetLRTriggerValue(Enums::LR::RIGHT, padNumber, INPUT_STATE::BEFORE) < 0.9f;
                }
            }
        }

        // 1以上ならtrue
        return result > 0;
    }

    bool Input::IsReleasePadButton(PAD_BUTTON button, uint8_t padNumber){

        if(!instance_->isActive_){ return false; }
        // 押されているボタンがあれば加算される
        uint32_t result = 0;

        // ビットの立っているボタンを取得
        std::vector<PAD_BUTTON>buttons;
        for(auto& [key, value] : instance_->buttonMap_){
            if((button & key) != 0){
                buttons.push_back(key);
            }
        }

        // ボタンの状態を取得
        for(auto& b : buttons){
            if(b != PAD_BUTTON::LT && b != PAD_BUTTON::RT){

                result +=
                    !instance_->IsPressPadButton(padNumber, b, INPUT_STATE::CURRENT) &&
                    instance_->IsPressPadButton(padNumber, b, INPUT_STATE::BEFORE);

            } else{
                if(b == PAD_BUTTON::LT){
                    result +=
                        GetLRTriggerValue(Enums::LR::LEFT, padNumber, INPUT_STATE::CURRENT) < 0.9f &&
                        GetLRTriggerValue(Enums::LR::LEFT, padNumber, INPUT_STATE::BEFORE) >= 0.9f;
                } else{
                    result +=
                        GetLRTriggerValue(Enums::LR::RIGHT, padNumber, INPUT_STATE::CURRENT) < 0.9f &&
                        GetLRTriggerValue(Enums::LR::RIGHT, padNumber, INPUT_STATE::BEFORE) >= 0.9f;
                }
            }
        }

        // 1以上ならtrue
        return result > 0;
    }


    bool Input::IsPressAnyPadButton(uint8_t padNumber){

        if(!instance_->isActive_){ return false; }
        for(auto& [key, value] : instance_->buttonMap_){
            if(instance_->IsPressPadButton(padNumber, key, INPUT_STATE::CURRENT)){ return true; }
        }

        return false;
    }

    bool Input::IsTriggerAnyPadButton(uint8_t padNumber){
        if(!instance_->isActive_){ return false; }
        for(auto& [key, value] : instance_->buttonMap_){
            if(instance_->IsTriggerPadButton(key, padNumber)){ return true; }
        }

        return false;
    }

    bool Input::IsConnectedPad(uint8_t padNumber){
        return instance_->connected_[padNumber];
    }

    float Input::GetLRTriggerValue(Enums::LR LEFTorRIGHT, uint8_t padNumber, INPUT_STATE padState){

        if(!instance_->isActive_){ return 0.0f; }
        float triggerValue;

        // 対応するパッドの状態取得
        XINPUT_GAMEPAD* pad;
        if(padState == INPUT_STATE::CURRENT){// 現在の状態
            pad = &instance_->xInputState_[padNumber].Gamepad;
        } else{// 前フレームの状態
            pad = &instance_->preXInputState_[padNumber].Gamepad;
        }

        // 方向を取得
        triggerValue = (float)(LEFTorRIGHT == Enums::LR::LEFT ? pad->bLeftTrigger : pad->bRightTrigger);
        triggerValue /= 255.0f;

        return triggerValue;
    }

    Vector2 Input::GetStickValue(Enums::LR stick, uint8_t padNumber, INPUT_STATE padState){

        if(!instance_->isActive_){ return Vector2(); }
        // パッド番号の範囲外の場合アサート
        if(padNumber >= XUSER_MAX_COUNT){ assert(0); }
        // 番号に対応するパッドが存在しなければreturn
        if(!instance_->connected_[padNumber]){ return Vector2(); }

        // 対応するパッドの状態取得
        XINPUT_GAMEPAD* pad;
        if(padState == INPUT_STATE::CURRENT){// 現在の状態
            pad = &instance_->xInputState_[padNumber].Gamepad;
        } else{// 前フレームの状態
            pad = &instance_->preXInputState_[padNumber].Gamepad;
        }

        // 方向を取得
        Vector2 value = {
            (float)(stick == Enums::LR::LEFT ? pad->sThumbLX : pad->sThumbRX),
            (float)(stick == Enums::LR::LEFT ? pad->sThumbLY : pad->sThumbRY)
        };

        // SHORT型の最大値で割って-1 ~ 1にして返す
        value /= float(SHRT_MAX);
        // デッドゾーン未満なら0に
        if(Methods::Math::Length(value) < instance_->deadZone_){ value = { 0.0f,0.0f }; }

        // 範囲内におさめてreturn
        return { std::clamp(value.x,-1.0f,1.0f),std::clamp(value.y,-1.0f,1.0f) };
    }


    bool Input::IsTriggerStick(Enums::LR stick_LorR, Enums::DIRECTION4 direction, float border, uint8_t padNumber){
        if(!instance_->isActive_){ return false; }
        Vector2 stickValue[2] = {
            GetStickValue(stick_LorR,padNumber,INPUT_STATE::CURRENT),
            GetStickValue(stick_LorR,padNumber,INPUT_STATE::BEFORE)
        };

        // 方向によって判定
        switch(direction){
        case Enums::DIRECTION4::UP:
            return stickValue[0].y >= border && stickValue[1].y < border;
        case Enums::DIRECTION4::DOWN:
            return stickValue[0].y <= -border && stickValue[1].y > -border;
        case Enums::DIRECTION4::LEFT:
            return stickValue[0].x <= -border && stickValue[1].x > -border;
        case Enums::DIRECTION4::RIGHT:
            return stickValue[0].x >= border && stickValue[1].x < border;
        default:
            return false;
        }
    }

    // 何かしらの入力があったか
    bool Input::GetIsAnyInput(bool isIgnoreActiveFlag){
        if(!isIgnoreActiveFlag){
            if(!instance_->isActive_){ return false; }
        }

        instance_->isActive_ = true;
        bool result = IsPressAnyKey() or IsPressAnyPadButton()
            or Methods::Math::HasLength(GetStickValue(Enums::LR::LEFT))
            or Methods::Math::HasLength(GetStickValue(Enums::LR::RIGHT))
            or IsMouseInputAny();
        instance_->isActive_ = false;

        return result;
    }


    /*********************** 内部で使うやつ *********************/


    bool Input::IsPressPadButton(uint8_t padNumber, PAD_BUTTON button, INPUT_STATE padState){
        if(!instance_->isActive_){ return false; }
        // パッド番号の範囲外の場合アサート
        if(padNumber >= XUSER_MAX_COUNT){ assert(0); }
        // 番号に対応するパッドが存在しなければreturn
        if(!instance_->connected_[padNumber]){ return false; }


        // 対応するパッドの状態取得
        XINPUT_GAMEPAD* pad;
        if(padState == INPUT_STATE::CURRENT){// 現在の状態
            pad = &instance_->xInputState_[padNumber].Gamepad;
        } else{// 前フレームの状態
            pad = &instance_->preXInputState_[padNumber].Gamepad;
        }

        if(button == PAD_BUTTON::LT){// 左トリガー(押し切った時)
            return pad->bLeftTrigger >= 255;

        } else if(button == PAD_BUTTON::RT){// 右トリガー(押し切った時)
            return pad->bRightTrigger >= 255;

        } else{// トリガーボタン以外のボタン
            return pad->wButtons & buttonMap_[button];
        }
    }

    // マウスカーソルを範囲内でリピートさせる
    void Input::RepeatCursorInternal(){
        POINT cursorPos;
        GetCursorPos(&cursorPos);

        // クライアント座標 -> スクリーン座標に変換（左上）
        POINT topLeft = { (int)repeatRange_.min.x, (int)repeatRange_.min.y };
    #ifdef _DEBUG
        ClientToScreen(WindowManager::GetHWND(SEED::Instance::systemWindowTitle_), &topLeft);
    #else
        ClientToScreen(WindowManager::GetHWND(SEED::windowTitle_), &topLeft);
    #endif

        int clientWidth = int(repeatRange_.max.x - repeatRange_.min.x);
        int clientHeight = int(repeatRange_.max.y - repeatRange_.min.y);

        int left = topLeft.x;
        int top = topLeft.y;
        int right = left + clientWidth - 1;
        int bottom = top + clientHeight - 1;

        // --- ClipCursor で範囲を制限 ---
    #ifdef _DEBUG
        if(!IsPressKey(DIK_ESCAPE)){
            RECT clipRect = { left, top, right, bottom };
            ClipCursor(&clipRect);
        }
    #else
        RECT clipRect = { left, top, right, bottom };
        ClipCursor(&clipRect);
    #endif


        // --- 端に来たらワープ処理 ---
        bool moved = false;
        if(cursorPos.x <= left){
            cursorPos.x = right - 1;
            moved = true;
        } else if(cursorPos.x >= right - 1){
            cursorPos.x = left + 1;
            moved = true;
        }
        if(cursorPos.y <= top){
            cursorPos.y = bottom - 1;
            moved = true;
        } else if(cursorPos.y >= bottom - 1){
            cursorPos.y = top + 1;
            moved = true;
        }

        if(moved){
            SetCursorPos(cursorPos.x, cursorPos.y);
        }
    }
}