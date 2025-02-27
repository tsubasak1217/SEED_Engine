#include "PlayerInput.h"

PlayerInput::PlayerInput(){

    pPlayField_ = PlayField::GetInstance();

    ///////////////////////////////////////////////////////
    // タップの取得
    ///////////////////////////////////////////////////////
    {
        tap_.Value = []{
            return Input::IsPressKey(DIK_A) or Input::IsPressKey(DIK_S) or Input::IsPressKey(DIK_D) or Input::IsPressMouse(MOUSE_BUTTON::LEFT);
        };
        tap_.Trigger = [&]{
            if(Input::IsTriggerMouse(MOUSE_BUTTON::LEFT)){
                return true;
            } else{
                if(lane_.Value() != lane_.PreValue()){
                    return true;
                } else{
                    return Input::IsTriggerKey(DIK_A) or Input::IsTriggerKey(DIK_S) or Input::IsTriggerKey(DIK_D) or Input::IsTriggerMouse(MOUSE_BUTTON::LEFT);
                }
            }
        };
        tap_.Release = []{
            return Input::IsReleaseKey(DIK_A) or Input::IsReleaseKey(DIK_S) or Input::IsReleaseKey(DIK_D) or Input::IsReleaseMouse(MOUSE_BUTTON::LEFT);
        };
        tap_.Press = [&]{ return tap_.Value(); };
    }

    ///////////////////////////////////////////////////////
    // フリックの取得
    ///////////////////////////////////////////////////////
    {
        flick_.Value = [&]{
            float horizontalVal = Input::GetMouseVector().x;
            if(horizontalVal > flickDeadZone_){ return LR::RIGHT; }
            if(horizontalVal < -flickDeadZone_){ return LR::LEFT; }
            return LR::NONE;
        };

        flick_.PreValue = [&]{
            float horizontalVal = Input::GetMouseVector(INPUT_STATE::BEFORE).x;
            if(horizontalVal > flickDeadZone_){ return LR::RIGHT; }
            if(horizontalVal < -flickDeadZone_){ return LR::LEFT; }
            return LR::NONE;
        };

        flick_.Trigger = [&]{
            return (flick_.Value() != LR::NONE) && (flick_.PreValue() == LR::NONE);
        };
    }

    ///////////////////////////////////////////////////////
    // ホールドの取得
    ///////////////////////////////////////////////////////
    {
        hold_.Value = [&]{
            std::unordered_set<int32_t> holdLane;
            if(Input::IsPressKey(DIK_A)){ holdLane.insert(0); }
            if(Input::IsPressKey(DIK_S)){ holdLane.insert(1); }
            if(Input::IsPressKey(DIK_D)){ holdLane.insert(2); }
            if(Input::IsPressKey(DIK_F)){ holdLane.insert(3); }
            if(Input::IsPressMouse(MOUSE_BUTTON::LEFT)){ holdLane.insert(lane_.Value()); }
            return holdLane;

        };
        hold_.Trigger = [&]{
            return tap_.Trigger();
        };
        hold_.Release = [&]{
            return tap_.Release();
        };
        hold_.Press = [&]{
            return tap_.Value();
        };
    }

    ///////////////////////////////////////////////////////
    // レーンの取得
    ///////////////////////////////////////////////////////
    {
        lane_.Value = []{
            static float basePosX = -pPlayField_->kPlayFieldSizeX_ * 0.5f;
            float cursorDif = instance_->cursorPos_ - basePosX;
            return std::clamp(int(cursorDif / pPlayField_->kKeyWidth_), 0, pPlayField_->kKeyCount_ - 1);
        };

        lane_.PreValue = []{
            static float basePosX = -pPlayField_->kPlayFieldSizeX_ * 0.5f;
            float cursorDif = instance_->preCursorPos_ - basePosX;
            return std::clamp(int(cursorDif / pPlayField_->kKeyWidth_), 0, pPlayField_->kKeyCount_ - 1);
        };
    }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// インスタンスの取得
/////////////////////////////////////////////////////////////////////////////////////////////////////////
PlayerInput* PlayerInput::GetInstance(){
    if(!instance_){
        instance_ = new PlayerInput();
    }
    return instance_;
}
