#pragma once
#include <string>

class IScene;

// ゲームの基底ステート
class State_Base{
public:
    State_Base() = default;
    State_Base(IScene* pScene) : pScene_(pScene){};
    virtual ~State_Base(){};
    virtual void Update() = 0;
    virtual void Draw() = 0;
    void ShiftState(State_Base* nextState);

protected:
    IScene* pScene_ = nullptr;
};
