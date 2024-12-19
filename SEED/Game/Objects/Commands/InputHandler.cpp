#include "InputHandler.h"
#include "InputManager.h"

ICommand* InputHandler::HandleInput(){

    if(Input::IsPressKey(DIK_A)){
        return pressA_;
    } else if(Input::IsPressKey(DIK_D)){
        return pressD_;
    }

    return nullptr;
}

void InputHandler::AssignMoveLeftCommandToPressA(){
    ICommand* command = new MoveLeftCommand();
    pressA_ = command;
}

void InputHandler::AssignMoveRightCommandToPressD(){
    ICommand* command = new MoveRightCommand();
    pressD_ = command;
}
