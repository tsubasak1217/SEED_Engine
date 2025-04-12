#include <SEED/Source/Manager/InputManager/InputManager.h>
#include <SEED/Source/SEED.h>
#include <SEED/Lib/Functions/MyFunc/MyMath.h>
#include <SEED/Source/Manager/ImGuiManager/ImGuiManager.h>

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


// デストラクタ
Input::~Input(){
    if(instance_){
        delete instance_;
        instance_ = nullptr;
    }
}

// 単一のインスタンスを返す関数
const Input* Input::GetInstance(){
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
    // DirectInput
    instance_->InitializeDInput();
    // XInput
    instance_->InitializeXInput();
}


void Input::InitializeDInput(){
    HRESULT hr;

    // Inputオブジェクトの生成
    IDirectInput8* directInput = nullptr;
    hr = DirectInput8Create(
        WindowManager::GetHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8,
        (void**)&directInput, nullptr
    );
    assert(SUCCEEDED(hr));

    /*======================== キーボード ========================*/

    // キーボードデバイスの生成
    hr = directInput->CreateDevice(GUID_SysKeyboard, &keyboard_, NULL);
    assert(SUCCEEDED(hr));

    // 入力形式のセット
    hr = keyboard_->SetDataFormat(&c_dfDIKeyboard);// 標準
    assert(SUCCEEDED(hr));

    // 制御レベルの設定
    hr = keyboard_->SetCooperativeLevel(
        WindowManager::GetHWND(SEED::GetWindowTitle()),
        DISCL_FOREGROUND | DISCL_NOWINKEY | DISCL_NONEXCLUSIVE
    );
    assert(SUCCEEDED(hr));

    /*========================== マウス ==========================*/

    // マウスデバイスの生成
    hr = directInput->CreateDevice(GUID_SysMouse, &mouse_, NULL);
    assert(SUCCEEDED(hr));

    // 入力形式のセット
    hr = mouse_->SetDataFormat(&c_dfDIMouse);
    assert(SUCCEEDED(hr));

    // 制御レベルの設定
    hr = mouse_->SetCooperativeLevel(
        WindowManager::GetHWND(SEED::GetWindowTitle()),
        DISCL_FOREGROUND | DISCL_NONEXCLUSIVE
    );
}

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
    // DirectInput
    instance_->GetDInputState();

    // XInput
    instance_->GetXInputState();
}


void Input::GetDInputState(){

    // DirectInputのキーボード情報取得開始
    keyboard_->Acquire();
    // 前のフレームのキー情報を保存
    std::memcpy(preKeys_, keys_, sizeof(BYTE) * kMaxKey_);
    // 現在の全キーの状態を取得する
    keyboard_->GetDeviceState(sizeof(keys_), keys_);


    // マウスの情報取得開始
    mouse_->Acquire();
    // 前のフレームのマウス情報を保存
    std::memcpy(&preMouseState_, &mouseState_, sizeof(DIMOUSESTATE));
    // 現在のマウス状態を取得
    mouse_->GetDeviceState(sizeof(DIMOUSESTATE), &mouseState_);

}

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

/*                         指定したキーの入力の状態を返す関数                            */

/////////////////////////////////////////////////////////////////////////////////////


/********************* 外部から使えるやつ *******************/


//--------------------------------- キーボード -----------------------------------------//

bool Input::IsPressKey(uint8_t key){
    return instance_->keys_[key];
}

bool Input::IsPressKey(const std::initializer_list<uint8_t>& keys){
    for(auto& key : keys){
        if(instance_->keys_[key]){ return true; }
    }

    return false;
}

bool Input::IsPressAnyKey(){
    for(int i = 0; i < kMaxKey_; i++){
        if(instance_->keys_[i]){ return true; }
    }

    return false;
}

bool Input::IsTriggerKey(uint8_t key){
    return instance_->keys_[key] && !instance_->preKeys_[key];
}

bool Input::IsTriggerKey(const std::initializer_list<uint8_t>& keys){
    for(auto& key : keys){
        if(instance_->keys_[key] && !instance_->preKeys_[key]){ return true; }
    }

    return false;
}

bool Input::IsTriggerAnyKey(){
    for(int i = 0; i < kMaxKey_; i++){
        if(instance_->keys_[i] && !instance_->preKeys_[i]){ return true; }
    }

    return false;
}

bool Input::IsReleaseKey(uint8_t key){
    return !instance_->keys_[key] && instance_->preKeys_[key];
}

bool Input::IsReleaseKey(const std::initializer_list<uint8_t>& keys){
    for(auto& key : keys){
        if(!instance_->keys_[key] && instance_->preKeys_[key]){ return true; }
    }

    return false;
}

bool Input::IsReleaseAnyKey(){
    for(int i = 0; i < kMaxKey_; i++){
        if(!instance_->keys_[i] && instance_->preKeys_[i]){ return true; }
    }

    return false;
}


//----------------------------------- マウス -------------------------------------------//

// マウスのボタンが押されているかどうか
bool Input::IsPressMouse(MOUSE_BUTTON button){
    switch(button){
    case MOUSE_BUTTON::LEFT:
        return instance_->mouseState_.rgbButtons[0] & 0x80;
    case MOUSE_BUTTON::RIGHT:
        return instance_->mouseState_.rgbButtons[1] & 0x80;
    case MOUSE_BUTTON::MIDDLE:
        return instance_->mouseState_.rgbButtons[2] & 0x80;
    default:
        return false;
    }
}

// マウスのボタンが押された瞬間
bool Input::IsTriggerMouse(MOUSE_BUTTON button){
    switch(button){
    case MOUSE_BUTTON::LEFT:
        return (instance_->mouseState_.rgbButtons[0] & 0x80) && !(instance_->preMouseState_.rgbButtons[0] & 0x80);
    case MOUSE_BUTTON::RIGHT:
        return (instance_->mouseState_.rgbButtons[1] & 0x80) && !(instance_->preMouseState_.rgbButtons[1] & 0x80);
    case MOUSE_BUTTON::MIDDLE:
        return (instance_->mouseState_.rgbButtons[2] & 0x80) && !(instance_->preMouseState_.rgbButtons[2] & 0x80);
    default:
        return false;
    }

}

// マウスのボタンが離された瞬間かどうか
bool Input::IsReleaseMouse(MOUSE_BUTTON button){
    switch(button){
    case MOUSE_BUTTON::LEFT:
        return !(instance_->mouseState_.rgbButtons[0] & 0x80) && (instance_->preMouseState_.rgbButtons[0] & 0x80);
    case MOUSE_BUTTON::RIGHT:
        return !(instance_->mouseState_.rgbButtons[1] & 0x80) && (instance_->preMouseState_.rgbButtons[1] & 0x80);
    case MOUSE_BUTTON::MIDDLE:
        return !(instance_->mouseState_.rgbButtons[2] & 0x80) && (instance_->preMouseState_.rgbButtons[2] & 0x80);
    default:
        return false;
    }
}

// マウスのホイールの回転量を取得
int32_t Input::GetMouseWheel(){
    return instance_->mouseState_.lZ / 120;
}

// マウスの移動量を取得
Vector2 Input::GetMouseVector(INPUT_STATE inputState){
    return {
        inputState == INPUT_STATE::CURRENT ? (float)instance_->mouseState_.lX : (float)instance_->preMouseState_.lX,
        inputState == INPUT_STATE::CURRENT ? (float)instance_->mouseState_.lY : (float)instance_->preMouseState_.lY
    };
}

// マウスの方向を取得
Vector2 Input::GetMouseDirection(INPUT_STATE inputState){
    Vector2 vec = GetMouseVector(inputState);
    if(MyMath::Length(vec)){
        return MyMath::Normalize(vec);
    }

    return Vector2();
}

// マウスの座標を取得
Vector2 Input::GetMousePosition(INPUT_STATE inputState){

    if(inputState == INPUT_STATE::CURRENT){
        // マウスの座標を取得
        POINT point;
        GetCursorPos(&point);
        ScreenToClient(WindowManager::GetHWND(SEED::GetWindowTitle()), &point);
        Vector2 result = { (float)point.x,(float)point.y };

        // 比率をかけてもとに戻す
        Vector2 adjustScale = Vector2(1.0f, 1.0f) / WindowManager::GetWindowScale(SEED::GetWindowTitle());

        return result * adjustScale;
    } else{
        return instance_->GetMousePosition() - instance_->GetMouseVector();
    }
}


//---------------------------------- ゲームパッド --------------------------------------//

bool Input::IsPressPadButton(PAD_BUTTON button, uint8_t padNumber){

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
                result += GetLRTriggerValue(LR::LEFT) >= 0.9f;
            } else{
                result += GetLRTriggerValue(LR::RIGHT) >= 0.9f;
            }
        }
    }

    // 1以上ならtrue
    return result > 0;
}


bool Input::IsTriggerPadButton(PAD_BUTTON button, uint8_t padNumber){

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
                    GetLRTriggerValue(LR::LEFT, padNumber, INPUT_STATE::CURRENT) >= 0.9f &&
                    GetLRTriggerValue(LR::LEFT, padNumber, INPUT_STATE::BEFORE) < 0.9f;
            } else{
                result +=
                    GetLRTriggerValue(LR::RIGHT, padNumber, INPUT_STATE::CURRENT) >= 0.9f &&
                    GetLRTriggerValue(LR::RIGHT, padNumber, INPUT_STATE::BEFORE) < 0.9f;
            }
        }
    }

    // 1以上ならtrue
    return result > 0;
}

bool Input::IsReleasePadButton(PAD_BUTTON button, uint8_t padNumber){

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
                    GetLRTriggerValue(LR::LEFT, padNumber, INPUT_STATE::CURRENT) < 0.9f &&
                    GetLRTriggerValue(LR::LEFT, padNumber, INPUT_STATE::BEFORE) >= 0.9f;
            } else{
                result +=
                    GetLRTriggerValue(LR::RIGHT, padNumber, INPUT_STATE::CURRENT) < 0.9f &&
                    GetLRTriggerValue(LR::RIGHT, padNumber, INPUT_STATE::BEFORE) >= 0.9f;
            }
        }
    }

    // 1以上ならtrue
    return result > 0;
}


bool Input::IsPressAnyPadButton(uint8_t padNumber){

    for(auto& [key, value] : instance_->buttonMap_){
        if(instance_->IsPressPadButton(padNumber, key, INPUT_STATE::CURRENT)){ return true; }
    }

    return false;
}

bool Input::IsTriggerAnyPadButton(uint8_t padNumber){
    for(auto& [key, value] : instance_->buttonMap_){
        if(instance_->IsTriggerPadButton(key, padNumber)){ return true; }
    }

    return false;
}

bool Input::IsConnectedPad(uint8_t padNumber){
    return instance_->connected_[padNumber];
}

float Input::GetLRTriggerValue(LR LEFTorRIGHT, uint8_t padNumber, INPUT_STATE padState){

    float triggerValue;

    // 対応するパッドの状態取得
    XINPUT_GAMEPAD* pad;
    if(padState == INPUT_STATE::CURRENT){// 現在の状態
        pad = &instance_->xInputState_[padNumber].Gamepad;
    } else{// 前フレームの状態
        pad = &instance_->preXInputState_[padNumber].Gamepad;
    }

    // 方向を取得
    triggerValue = (float)(LEFTorRIGHT == LR::LEFT ? pad->bLeftTrigger : pad->bRightTrigger);
    triggerValue /= 255.0f;

    return triggerValue;
}

Vector2 Input::GetStickValue(LR stick, uint8_t padNumber, INPUT_STATE padState){
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
        (float)(stick == LR::LEFT ? pad->sThumbLX : pad->sThumbRX),
        (float)(stick == LR::LEFT ? pad->sThumbLY : pad->sThumbRY)
    };

    // SHORT型の最大値で割って-1 ~ 1にして返す
    value /= float(SHRT_MAX);
    // デッドゾーン未満なら0に
    if(MyMath::Length(value) < instance_->deadZone_){ value = { 0.0f,0.0f }; }

    // 範囲内におさめてreturn
    return { std::clamp(value.x,-1.0f,1.0f),std::clamp(value.y,-1.0f,1.0f) };
}


bool Input::IsTriggerStick(LR stick_LorR, DIRECTION4 direction, float border, uint8_t padNumber){
    Vector2 stickValue[2] = {
        GetStickValue(stick_LorR,padNumber,INPUT_STATE::CURRENT),
        GetStickValue(stick_LorR,padNumber,INPUT_STATE::BEFORE)
    };

    // 方向によって判定
    switch(direction){
    case DIRECTION4::UP:
        return stickValue[0].y >= border && stickValue[1].y < border;
    case DIRECTION4::DOWN:
        return stickValue[0].y <= -border && stickValue[1].y > -border;
    case DIRECTION4::LEFT:
        return stickValue[0].x <= -border && stickValue[1].x > -border;
    case DIRECTION4::RIGHT:
        return stickValue[0].x >= border && stickValue[1].x < border;
    default:
        return false;
    }
}




/*********************** 内部で使うやつ *********************/

bool Input::IsPressPadButton(uint8_t padNumber, PAD_BUTTON button, INPUT_STATE padState){
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
