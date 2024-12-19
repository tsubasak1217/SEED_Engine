#pragma once
#include "ICommand.h"

class MoveRightCommand : public ICommand{
public:
    void Execute(Player* player)override;
};

class MoveLeftCommand : public ICommand{
public:
    void Execute(Player* player)override;
};