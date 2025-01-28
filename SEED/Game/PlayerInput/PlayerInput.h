#pragma once

///local
//lib
#include "InputManager.h"

//math
#include "MyMath.h"
#include "Vector2.h"

// keyboard か Pad か
//enum class KeyOrPad{
//    KEY,
//    PAD
//};

/// <summary>
/// 各操作ごとの入力をまとめた構造体
/// </summary>
namespace PlayerInput{
    namespace CharacterMove{
        inline  Vector2 GetCharacterMoveDirection(){
            if(Input::IsConnectedPad(0)){
                return Input::GetStickValue(LR::LEFT);
            }
            Vector2 keyDirection = {
                float(Input::IsPressKey(DIK_D)) - float(Input::IsPressKey(DIK_A)), //x
                float(Input::IsPressKey(DIK_W)) - float(Input::IsPressKey(DIK_S))  //z
            };
            return MyMath::Normalize(keyDirection);
        }
        inline bool Jump(){
            if(Input::IsConnectedPad(0)){
                return Input::IsTriggerPadButton(PAD_BUTTON::A);
            }
            return Input::IsTriggerKey(DIK_SPACE);
        }
        inline bool Hovering(){
            if(Input::IsConnectedPad(0)){
                return Input::IsPressPadButton(PAD_BUTTON::A);
            }
            return Input::IsPressKey(DIK_SPACE);
        }

        inline bool FocusEggInput(){
            if(Input::IsConnectedPad(0)){
                return Input::IsTriggerPadButton(PAD_BUTTON::LT);
            }
            return Input::IsTriggerKey(DIK_Q);
        }
        inline bool CancelFocusEgg(bool siwtchIsPress){
            if(siwtchIsPress){
                if(Input::IsConnectedPad(0)){
                    return !Input::IsPressPadButton(PAD_BUTTON::LT);
                }
                return !Input::IsPressKey(DIK_Q);
            }
            if(Input::IsConnectedPad(0)){
                return Input::IsTriggerPadButton(PAD_BUTTON::LT);
            }
            return Input::IsTriggerKey(DIK_Q);
        }

        inline bool ThrowEgg(){
            if(Input::IsConnectedPad(0)){
                return Input::IsTriggerPadButton(PAD_BUTTON::RT);
            }
            return Input::IsTriggerKey(DIK_E);
        }
        inline bool ThrowEggOnFocus(bool siwtchIsPress){
            if(siwtchIsPress){
                if(Input::IsConnectedPad(0)){
                    return Input::IsPressPadButton(PAD_BUTTON::LT) && Input::IsReleasePadButton(PAD_BUTTON::RT);
                }
                return Input::IsPressKey(DIK_Q) && Input::IsPressKey(DIK_E);
            }
            if(Input::IsConnectedPad(0)){
                return Input::IsTriggerPadButton(PAD_BUTTON::RT);
            }
            return  Input::IsTriggerKey(DIK_E);
        }

        inline bool Eat(){
            if(Input::IsConnectedPad(0)){
                return Input::IsTriggerPadButton(PAD_BUTTON::B);
            }
            return Input::IsTriggerKey(DIK_F);
        }

        inline bool FastForwardEggTimer(){
            if(Input::IsConnectedPad(0)){
                return Input::IsTriggerPadButton(PAD_BUTTON::X);
            }
            return Input::IsTriggerKey(DIK_R);
        }

        inline bool GoNextStage(){
            if(Input::IsConnectedPad(0)){
                return Input::IsTriggerPadButton(PAD_BUTTON::A | PAD_BUTTON::B);
            }
            return Input::IsTriggerKey(DIK_SPACE) || Input::IsTriggerKey(DIK_F);

        }
    };

    namespace Pause{
        inline  bool Pause(){
            if(Input::IsConnectedPad(0)){
                return Input::IsTriggerPadButton(PAD_BUTTON::START);
            }
            return Input::IsTriggerKey(DIK_RETURN) || Input::IsTriggerKey(DIK_ESCAPE);
        }
        inline  bool Exit(){
            if(Input::IsConnectedPad(0)){
                return Input::IsTriggerPadButton(PAD_BUTTON::START);
            }
            return Input::IsTriggerKey(DIK_RETURN) || Input::IsTriggerKey(DIK_ESCAPE);
        }

        inline  bool addItemIndex(){
            if(Input::IsConnectedPad(0)){
                return Input::IsTriggerStick(LR::LEFT,DIRECTION::UP) || Input::IsTriggerPadButton(PAD_BUTTON::UP | PAD_BUTTON::RB | PAD_BUTTON::RT);
            }
            return Input::IsTriggerKey(DIK_W) || Input::IsTriggerKey(DIK_RIGHT) || Input::IsTriggerKey(DIK_UP);
        }
        inline  bool subItemIndex(){
            if(Input::IsConnectedPad(0)){
                return Input::IsTriggerStick(LR::LEFT,DIRECTION::DOWN) || Input::IsTriggerPadButton(PAD_BUTTON::DOWN | PAD_BUTTON::LB | PAD_BUTTON::LT);
            }
            return Input::IsTriggerKey(DIK_S) || Input::IsTriggerKey(DIK_LEFT) || Input::IsTriggerKey(DIK_DOWN);
        }

        inline  bool ExecuteItem(){
            if(Input::IsConnectedPad(0)){
                return Input::IsTriggerPadButton(PAD_BUTTON::A | PAD_BUTTON::B);
            }
            return Input::IsTriggerKey(DIK_SPACE);
        }
    }

    namespace StageSelect{
        inline  bool addStageNum(){
            if(Input::IsConnectedPad(0)){
                return Input::IsTriggerStick(LR::LEFT,DIRECTION::RIGHT) || Input::IsTriggerPadButton(PAD_BUTTON::RIGHT | PAD_BUTTON::RB | PAD_BUTTON::RT);
            }
            return Input::IsTriggerKey(DIK_D) || Input::IsTriggerKey(DIK_RIGHT) || Input::IsTriggerKey(DIK_E);
        }
        inline  bool subStageNum(){
            if(Input::IsConnectedPad(0)){
                return Input::IsTriggerStick(LR::LEFT,DIRECTION::LEFT) || Input::IsTriggerPadButton(PAD_BUTTON::LEFT | PAD_BUTTON::LB | PAD_BUTTON::LT);
            }
            return Input::IsTriggerKey(DIK_A) || Input::IsTriggerKey(DIK_LEFT) || Input::IsTriggerKey(DIK_Q);
        }
        inline  bool DecideStage(){
            if(Input::IsConnectedPad(0)){
                return Input::IsPressPadButton(PAD_BUTTON::A | PAD_BUTTON::B);
            }
            return Input::IsTriggerKey(DIK_SPACE) || Input::IsTriggerKey(DIK_RETURN);
        }

    };

    namespace TitleScene{
        inline bool IsStartGame(){
            if(Input::IsConnectedPad(0)){
                return Input::IsPressAnyPadButton();
            }
            return Input::IsPressAnyKey();
        }
    };

    namespace Camera{
        inline Vector2 GetCameraDirection(){
            if(Input::IsConnectedPad(0)){
                return Input::GetStickValue(LR::RIGHT);
            }
            Vector2 keyDirection = {
                float(Input::IsPressKey(DIK_RIGHT)) - float(Input::IsPressKey(DIK_LEFT)), //x
                float(Input::IsPressKey(DIK_UP)) - float(Input::IsPressKey(DIK_DOWN))  //z
            };
            return MyMath::Normalize(keyDirection);
        }
        inline float GetCameraDistance(){
            if(Input::IsConnectedPad(0)){
                return Input::GetLRTriggerValue(LR::LEFT) - Input::GetLRTriggerValue(LR::RIGHT);
            }
            return 0.5f;
        }
    };

    namespace Tutorial{
        inline  bool StepText(){
            if(Input::IsConnectedPad(0)){
                return Input::IsPressPadButton(PAD_BUTTON::A | PAD_BUTTON::B);
            }
            return Input::IsTriggerKey(DIK_SPACE) || Input::IsTriggerKey(DIK_RETURN);
        }
    }
};