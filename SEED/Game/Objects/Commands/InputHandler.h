#pragma once
#include "ICommand.h"
#include "MoveCommand.h"

class InputHandler{
public:
    ICommand* HandleInput();

    void AssignMoveLeftCommandToPressA();
    void AssignMoveRightCommandToPressD();

private:
    ICommand* pressA_;
    ICommand* pressD_;
};