#include "InputManager.h"
#include "SEED.h"
#include "MyMath.h"

// static変数初期化
InputManager* InputManager::instance_ = nullptr;

// デストラクタ
InputManager::~InputManager(){
    if(instance_)
    {
        delete instance_;
        instance_ = nullptr;
    }
}

// 単一のインスタンスを返す関数
const InputManager* InputManager::GetInstance(){
    if(!instance_)
    {
        instance_ = new InputManager();
    }

    return instance_;
}

/////////////////////////////////////////////////////////////////////////////////////

/*                                   初期化関数                                      */

/////////////////////////////////////////////////////////////////////////////////////

void InputManager::Initialize(){
    // 実体がなければ作る
    GetInstance();

    /*=========== 初期化 ===========*/
    // DirectInput
    instance_->InitializeDInput();
    // XInput
    instance_->InitializeXInput();
}


void InputManager::InitializeDInput(){
    HRESULT hr;

    // Inputオブジェクトの生成
    IDirectInput8* directInput = nullptr;
    hr = DirectInput8Create(
        SEED::GetHINSTANCE(), DIRECTINPUT_VERSION, IID_IDirectInput8,
        (void**)&directInput, nullptr
    );
    assert(SUCCEEDED(hr));

    // キーボードデバイスの生成
    hr = directInput->CreateDevice(GUID_SysKeyboard, &keyboard_, NULL);
    assert(SUCCEEDED(hr));

    // 入力形式のセット
    hr = keyboard_->SetDataFormat(&c_dfDIKeyboard);// 標準
    assert(SUCCEEDED(hr));

    // 制御レベルの設定
    hr = keyboard_->SetCooperativeLevel(
        SEED::GetHWND(),
        DISCL_FOREGROUND | DISCL_NOWINKEY | DISCL_NONEXCLUSIVE
    );
    assert(SUCCEEDED(hr));
}

void InputManager::InitializeXInput(){
    for(DWORD i = 0; i < XUSER_MAX_COUNT; i++) {
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

void InputManager::GetAllInput(){
    // DirectInput
    instance_->GetDInputState();

    // XInput
    instance_->GetXInputState();
}


void InputManager::GetDInputState(){
    // DirectInputのキーボード情報取得開始
    keyboard_->Acquire();

    // 前のフレームのキー情報を保存
    std::memcpy(preKeys_, keys_, sizeof(BYTE) * kMaxKey_);

    // 現在の全キーの状態を取得する
    keyboard_->GetDeviceState(sizeof(keys_), keys_);
}

void InputManager::GetXInputState(){
    for(DWORD i = 0; i < XUSER_MAX_COUNT; i++) {

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

//------- キーボード -------//

bool InputManager::IsPressKey(uint8_t key){
    return instance_->keys_[key];
}

bool InputManager::IsTriggerKey(uint8_t key){
    return instance_->keys_[key] && !instance_->preKeys_[key];
}

bool InputManager::IsReleaseKey(uint8_t key){
    return !instance_->keys_[key] && instance_->preKeys_[key];
}

//------ ゲームパッド ------//

bool InputManager::IsPressPadButton(PAD_BUTTON button, uint8_t padNumber){

    if(button != PAD_BUTTON::LT && button != PAD_BUTTON::RT){
        return instance_->IsPressPadButton(padNumber, button, PAD_STATE::CURRENT);

    } else{

        if(button == PAD_BUTTON::LT){
            return GetLRTriggerValue(PAD_TRIGGER::LEFT) >= 0.9f;
        } else{
            return GetLRTriggerValue(PAD_TRIGGER::RIGHT) >= 0.9f;
        }

    }
}

bool InputManager::IsTriggerPadButton(PAD_BUTTON button, uint8_t padNumber){

    if(button != PAD_BUTTON::LT && button != PAD_BUTTON::RT){

        return
            instance_->IsPressPadButton(padNumber, button, PAD_STATE::CURRENT) &&
            !instance_->IsPressPadButton(padNumber, button, PAD_STATE::BEFORE);

    } else{
        if(button == PAD_BUTTON::LT){
            return
                GetLRTriggerValue(PAD_TRIGGER::LEFT, padNumber, PAD_STATE::CURRENT) >= 0.9f &&
                GetLRTriggerValue(PAD_TRIGGER::LEFT, padNumber, PAD_STATE::BEFORE) < 0.9f;
        } else{
            return
                GetLRTriggerValue(PAD_TRIGGER::RIGHT, padNumber, PAD_STATE::CURRENT) >= 0.9f &&
                GetLRTriggerValue(PAD_TRIGGER::RIGHT, padNumber, PAD_STATE::BEFORE) < 0.9f;
        }
    }
}

bool InputManager::IsReleasePadButton(PAD_BUTTON button, uint8_t padNumber){

    if(button != PAD_BUTTON::LT && button != PAD_BUTTON::RT){

        return
            !instance_->IsPressPadButton(padNumber, button, PAD_STATE::CURRENT) &&
            instance_->IsPressPadButton(padNumber, button, PAD_STATE::BEFORE);

    } else{
        if(button == PAD_BUTTON::LT){
            return
                GetLRTriggerValue(PAD_TRIGGER::LEFT, padNumber, PAD_STATE::CURRENT) < 0.9f &&
                GetLRTriggerValue(PAD_TRIGGER::LEFT, padNumber, PAD_STATE::BEFORE) >= 0.9f;
        } else{
            return
                GetLRTriggerValue(PAD_TRIGGER::RIGHT, padNumber, PAD_STATE::CURRENT) < 0.9f &&
                GetLRTriggerValue(PAD_TRIGGER::RIGHT, padNumber, PAD_STATE::BEFORE) >= 0.9f;
        }
    }
}

bool InputManager::IsPressAnyPadButton(PAD_BUTTON padNumber){
    padNumber;
    return false;
}

float InputManager::GetLRTriggerValue(PAD_TRIGGER LEFTorRIGHT, uint8_t padNumber, PAD_STATE padState){

    float triggerValue;

    // 対応するパッドの状態取得
    XINPUT_GAMEPAD* pad;
    if(padState == PAD_STATE::CURRENT){// 現在の状態
        pad = &instance_->xInputState_[padNumber].Gamepad;
    } else{// 前フレームの状態
        pad = &instance_->preXInputState_[padNumber].Gamepad;
    }

    // 方向を取得
    triggerValue = (float)(LEFTorRIGHT == PAD_TRIGGER::LEFT ? pad->bLeftTrigger : pad->bRightTrigger);
    triggerValue /= 255.0f;

    return triggerValue;
}

Vector2 InputManager::GetStickValue(PAD_STICK stick, uint8_t padNumber, PAD_STATE padState){
    // パッド番号の範囲外の場合アサート
    if(padNumber >= XUSER_MAX_COUNT){ assert(0); }
    // 番号に対応するパッドが存在しなければreturn
    if(!instance_->connected_[padNumber]){ return Vector2(); }

    // 対応するパッドの状態取得
    XINPUT_GAMEPAD* pad;
    if(padState == PAD_STATE::CURRENT){// 現在の状態
        pad = &instance_->xInputState_[padNumber].Gamepad;
    } else{// 前フレームの状態
        pad = &instance_->preXInputState_[padNumber].Gamepad;
    }

    // 方向を取得
    Vector2 value = {
    (float)(stick == PAD_STICK::LEFT ? pad->sThumbLX : pad->sThumbRX),
    (float)(stick == PAD_STICK::LEFT ? pad->sThumbLY : pad->sThumbRY)
    };

    // SHORT型の最大値で割って-1 ~ 1にして返す
    value /= float(SHRT_MAX);
    // デッドゾーン未満なら0に
    if(MyMath::Length(value) < instance_->deadZone_){ value = { 0.0f,0.0f }; }

    // 範囲内におさめてreturn
    return { std::clamp(value.x,-1.0f,1.0f),std::clamp(value.y,-1.0f,1.0f) };
}

Vector2 InputManager::GetStickDirection(PAD_STICK stick, uint8_t padNumber, PAD_STATE padState){
    return MyMath::Normalize(GetStickValue(stick, padNumber, padState));
}



/*********************** 内部で使うやつ *********************/

bool InputManager::IsPressPadButton(uint8_t padNumber, PAD_BUTTON button, PAD_STATE padState){
    // パッド番号の範囲外の場合アサート
    if(padNumber >= XUSER_MAX_COUNT){ assert(0); }
    // 番号に対応するパッドが存在しなければreturn
    if(!instance_->connected_[padNumber]){ return false; }


    // 対応するパッドの状態取得
    XINPUT_GAMEPAD* pad;
    if(padState == PAD_STATE::CURRENT){// 現在の状態
        pad = &instance_->xInputState_[padNumber].Gamepad;
    } else{// 前フレームの状態
        pad = &instance_->preXInputState_[padNumber].Gamepad;
    }


    if(button == PAD_BUTTON::LT){// 左トリガー(押し切った時)

        return pad->bLeftTrigger == 255;

    } else if(button == PAD_BUTTON::LT){// 右トリガー(押し切った時)

        return pad->bRightTrigger == 255;

    } else {// トリガーボタン以外のボタン

        return pad->wButtons & (uint32_t)button;
    }
}
