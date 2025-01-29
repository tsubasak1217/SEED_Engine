#pragma once
#include <memory>
#include <State_Base.h>
#include <Event_Base.h>

class Scene_Base{
public:
    Scene_Base();
    virtual ~Scene_Base(){};
    virtual void Initialize() = 0;
    virtual void Finalize() = 0;
    virtual void Update() = 0;
    virtual void Draw() = 0;
    virtual void BeginFrame();
    virtual void EndFrame() = 0;
    virtual void HandOverColliders() = 0;
    virtual void ManageState();

public:
    void ChangeScene(const std::string& nextSceneName);
    void ChangeState(State_Base* nextState);
    void CauseEvent(Event_Base* nextEventState);
    void EndEvent() { currentEvent_ = nullptr; };

protected:
    std::unique_ptr<State_Base> currentState_;
    std::unique_ptr<Event_Base> currentEvent_;
    bool isStateChanged_ = false;
};