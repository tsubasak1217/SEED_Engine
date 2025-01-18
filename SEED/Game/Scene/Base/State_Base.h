#pragma once
#include <string>

class Scene_Base;

// ゲームの基底ステート
class State_Base{
public:
    State_Base() = default;
    State_Base(Scene_Base* pScene) : pScene_(pScene){};
    virtual ~State_Base(){};
    virtual void Initialize() = 0;
    virtual void Finalize() = 0;
    virtual void Update() = 0;
    virtual void Draw() = 0;
    virtual void BeginFrame() = 0;
    virtual void EndFrame() = 0;
    virtual void HandOverColliders() = 0;
    virtual void ManageState() = 0;

protected:
    Scene_Base* pScene_ = nullptr;
};
