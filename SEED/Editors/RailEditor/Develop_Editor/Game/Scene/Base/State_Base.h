#pragma once
#include <string>

class Scene_Base;

// ゲームの基底ステート
class State_Base{
public:
    State_Base() = default;
    State_Base(Scene_Base* pScene) : pScene_(pScene){};
    virtual ~State_Base(){};
    virtual void Update() = 0;
    virtual void Draw() = 0;
    void ShiftState(State_Base* nextState);

protected:
    Scene_Base* pScene_ = nullptr;
};
