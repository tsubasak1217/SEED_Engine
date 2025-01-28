#include "PlayerInput.h"


//bool PlayerInput::TitleScene::IsStartGame(){
//    if(Input::IsConnectedPad(0)){
//        return Input::IsPressAnyPadButton();
//    }
//    return Input::IsPressAnyKey();
//}
//
//Vector2 PlayerInput::CharacterMove::GetCharacterMoveDirection(){
//    if(Input::IsConnectedPad(0)){
//        return Input::GetStickValue(LR::LEFT);
//    }
//    Vector2 keyDirection = {
//        float(Input::IsPressKey(DIK_D)) - float(Input::IsPressKey(DIK_A)), //x
//        float(Input::IsPressKey(DIK_W)) - float(Input::IsPressKey(DIK_S))  //z
//    };
//    return MyMath::Normalize(keyDirection);
//}
//
//bool PlayerInput::CharacterMove::FocusEggInput(){
//    if(Input::IsConnectedPad(0)){
//        return Input::IsTriggerPadButton(PAD_BUTTON::LT);
//    }
//    return Input::IsTriggerKey(DIK_Q);
//}
//
//bool PlayerInput::CharacterMove::CancelFocusEgg(bool siwtchIsPress){
//    if(siwtchIsPress){
//        if(Input::IsConnectedPad(0)){
//            return !Input::IsPressPadButton(PAD_BUTTON::LT);
//        }
//        return !Input::IsPressKey(DIK_Q);
//    }
//    if(Input::IsConnectedPad(0)){
//        return Input::IsTriggerPadButton(PAD_BUTTON::LT);
//    }
//    return Input::IsTriggerKey(DIK_Q);
//}
//
//bool PlayerInput::CharacterMove::ThrowEgg(){
//    if(Input::IsConnectedPad(0)){
//        return Input::IsTriggerPadButton(PAD_BUTTON::RT);
//    }
//    return Input::IsTriggerKey(DIK_E);
//}
//
//bool PlayerInput::CharacterMove::ThrowEggOnFocus(bool siwtchIsPress){
//    if(siwtchIsPress){
//        if(Input::IsConnectedPad(0)){
//            return Input::IsPressPadButton(PAD_BUTTON::LT) && Input::IsReleasePadButton(PAD_BUTTON::RT);
//        }
//        return Input::IsPressKey(DIK_Q) && Input::IsPressKey(DIK_E);
//    }
//    if(Input::IsConnectedPad(0)){
//        return Input::IsTriggerPadButton(PAD_BUTTON::RT);
//    }
//    return  Input::IsTriggerKey(DIK_E);
//}
//
//bool PlayerInput::CharacterMove::Eat(){
//    if(Input::IsConnectedPad(0)){
//        return Input::IsTriggerPadButton(PAD_BUTTON::B);
//    }
//    return Input::IsTriggerKey(DIK_F);
//}
//
//bool PlayerInput::CharacterMove::FastForwardEggTimer(){
//    if(Input::IsConnectedPad(0)){
//        return Input::IsTriggerPadButton(PAD_BUTTON::X);
//    }
//    return Input::IsTriggerKey(DIK_R);
//}
//
//bool PlayerInput::CharacterMove::GoNextStage(){
//    if(Input::IsConnectedPad(0)){
//        return Input::IsTriggerPadButton(PAD_BUTTON::A | PAD_BUTTON::B);
//    }
//    return Input::IsTriggerKey(DIK_SPACE) || Input::IsTriggerKey(DIK_F);
//
//}
//
//bool PlayerInput::CharacterMove::Jump(){
//    if(Input::IsConnectedPad(0)){
//        return Input::IsTriggerPadButton(PAD_BUTTON::A);
//    }
//    return Input::IsTriggerKey(DIK_SPACE);
//}
//
//bool PlayerInput::CharacterMove::Hovering(){
//    if(Input::IsConnectedPad(0)){
//        return Input::IsPressPadButton(PAD_BUTTON::A);
//    }
//    return Input::IsPressKey(DIK_SPACE);
//}
//
//bool PlayerInput::StageSelect::addStageNum(){
//    if(Input::IsConnectedPad(0)){
//        return Input::IsTriggerStick(LR::LEFT,DIRECTION::RIGHT) || Input::IsTriggerPadButton(PAD_BUTTON::RIGHT | PAD_BUTTON::RB | PAD_BUTTON::RT);
//    }
//    return Input::IsTriggerKey(DIK_D) || Input::IsTriggerKey(DIK_RIGHT) || Input::IsTriggerKey(DIK_E);
//}
//
//bool PlayerInput::StageSelect::subStageNum(){
//    if(Input::IsConnectedPad(0)){
//        return Input::IsTriggerStick(LR::LEFT,DIRECTION::RIGHT) || Input::IsTriggerPadButton(PAD_BUTTON::LEFT | PAD_BUTTON::LB | PAD_BUTTON::LT);
//    }
//    return Input::IsTriggerKey(DIK_A) || Input::IsTriggerKey(DIK_LEFT) || Input::IsTriggerKey(DIK_Q);
//}
//
//bool PlayerInput::StageSelect::DecideStage(){
//    if(Input::IsConnectedPad(0)){
//        return Input::IsPressPadButton(PAD_BUTTON::A);
//    }
//    return Input::IsTriggerKey(DIK_SPACE) || Input::IsTriggerKey(DIK_RETURN);
//}
//
//bool PlayerInput::Pause::Pause(){
//    if(Input::IsConnectedPad(0)){
//        return Input::IsTriggerPadButton(PAD_BUTTON::START);
//    }
//    return Input::IsTriggerKey(DIK_RETURN) || Input::IsTriggerKey(DIK_ESCAPE);
//}
//
//bool PlayerInput::Pause::Exit(){
//    if(Input::IsConnectedPad(0)){
//        return Input::IsTriggerPadButton(PAD_BUTTON::START);
//    }
//    return Input::IsTriggerKey(DIK_RETURN) || Input::IsTriggerKey(DIK_ESCAPE);
//}
//
//bool PlayerInput::Pause::addItemIndex(){
//    if(Input::IsConnectedPad(0)){
//        return Input::IsTriggerStick(LR::LEFT,DIRECTION::UP) || Input::IsTriggerPadButton(PAD_BUTTON::UP | PAD_BUTTON::RB | PAD_BUTTON::RT);
//    }
//    return Input::IsTriggerKey(DIK_W) || Input::IsTriggerKey(DIK_RIGHT) || Input::IsTriggerKey(DIK_UP);
//}
//
//bool PlayerInput::Pause::subItemIndex(){
//    if(Input::IsConnectedPad(0)){
//        return Input::IsTriggerStick(LR::LEFT,DIRECTION::DOWN) || Input::IsTriggerPadButton(PAD_BUTTON::DOWN | PAD_BUTTON::LB | PAD_BUTTON::LT);
//    }
//    return Input::IsTriggerKey(DIK_S) || Input::IsTriggerKey(DIK_LEFT) || Input::IsTriggerKey(DIK_DOWN);
//}
//
//bool PlayerInput::Pause::ExecuteItem(){
//    if(Input::IsConnectedPad(0)){
//        return Input::IsTriggerPadButton(PAD_BUTTON::A | PAD_BUTTON::B);
//    }
//    return Input::IsTriggerKey(DIK_SPACE);
//}
