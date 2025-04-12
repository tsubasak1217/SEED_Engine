#include "PlayerInput.h"
#include <SEED/Lib/Functions/MyFunc/MyMath.h>
#include <SEED/Lib/Functions/MyFunc/MatrixFunc.h>

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
    // 左右フリックの取得
    ///////////////////////////////////////////////////////
    {
        sideFlick_.Value = [&]{
            float horizontalVal = Input::GetMouseVector().x;
            if(horizontalVal > flickDeadZone_){ return LR::RIGHT; }
            if(horizontalVal < -flickDeadZone_){ return LR::LEFT; }
            return LR::NONE;
        };

        sideFlick_.PreValue = [&]{
            float horizontalVal = Input::GetMouseVector(INPUT_STATE::BEFORE).x;
            if(horizontalVal > flickDeadZone_){ return LR::RIGHT; }
            if(horizontalVal < -flickDeadZone_){ return LR::LEFT; }
            return LR::NONE;
        };

        sideFlick_.Trigger = [&]{
            return (sideFlick_.Value() != LR::NONE) && (sideFlick_.PreValue() == LR::NONE);
        };
    }

    ///////////////////////////////////////////////////////
    // 矩形フリックの取得
    ///////////////////////////////////////////////////////
    {
        rectFlick_.Value = [&]{
            // LT,RT,LB,RB方向のマウスフリック判定を取得
            Vector2 flickVec = Input::GetMouseVector();

            // 長さがデッドゾーンより小さければ無視
            if(MyMath::Length(flickVec) < flickDeadZone_){ return DIRECTION8::NONE; }

            // 方向を取得
            if(flickVec.x > 0.0f && flickVec.y <= 0.0f){ return DIRECTION8::RIGHTTOP; }
            if(flickVec.x > 0.0f && flickVec.y > 0.0f){ return DIRECTION8::RIGHTBOTTOM; }
            if(flickVec.x <= 0.0f && flickVec.y > 0.0f){ return DIRECTION8::LEFTBOTTOM; }
            if(flickVec.x <= 0.0f && flickVec.y <= 0.0f){ return DIRECTION8::LEFTTOP; }

            // どの方向にも当てはまらなければ無視
            return DIRECTION8::NONE;
        };

        rectFlick_.PreValue = [&]{
            // LT,RT,LB,RB方向のマウスフリック判定を取得
            Vector2 flickVec = Input::GetMouseVector(INPUT_STATE::BEFORE);

            // 長さがデッドゾーンより小さければ無視
            if(MyMath::Length(flickVec) < flickDeadZone_){ return DIRECTION8::NONE; }

            // 方向を取得
            if(flickVec.x > 0.0f && flickVec.y <= 0.0f){ return DIRECTION8::RIGHTTOP; }
            if(flickVec.x > 0.0f && flickVec.y > 0.0f){ return DIRECTION8::RIGHTBOTTOM; }
            if(flickVec.x <= 0.0f && flickVec.y > 0.0f){ return DIRECTION8::LEFTBOTTOM; }
            if(flickVec.x <= 0.0f && flickVec.y <= 0.0f){ return DIRECTION8::LEFTTOP; }

            // どの方向にも当てはまらなければ無視
            return DIRECTION8::NONE;
        };

        rectFlick_.Trigger = [&]{
            return (rectFlick_.Value() != DIRECTION8::NONE) && (rectFlick_.PreValue() == DIRECTION8::NONE);
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// デストラクタ
/////////////////////////////////////////////////////////////////////////////////////////////////////////
PlayerInput::~PlayerInput(){
}
