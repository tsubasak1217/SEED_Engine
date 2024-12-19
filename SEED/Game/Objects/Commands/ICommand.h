#pragma once

class Player;

class ICommand{
public:
    virtual ~ICommand(){}
    virtual void Execute(Player* player) = 0;
};