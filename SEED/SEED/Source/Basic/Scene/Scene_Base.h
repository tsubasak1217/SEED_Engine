#pragma once
#include <memory>
#include <SEED/Source/Basic/Scene/State_Base.h>
#include <SEED/Source/Basic/Scene/EventState_Base.h>
#include <SEED/Source/Manager/Hierarchy/Hierarchy.h>

class GameObject;

class Scene_Base{
public:
    Scene_Base();
    virtual ~Scene_Base(){};
    virtual void Initialize() = 0;
    virtual void Finalize();
    virtual void Update() = 0;
    virtual void Draw() = 0;
    virtual void BeginFrame();
    virtual void EndFrame() = 0;
    virtual void HandOverColliders() = 0;
    virtual void ManageState();

public:
    void ChangeScene(const std::string& nextSceneName);
    void ChangeState(State_Base* nextState);
    void CauseEvent(EventState_Base* nextEventState);
    void EndEvent() { currentEventState_ = nullptr; };
    bool HasEvent(){ return currentEventState_ != nullptr; };
    void RegisterToHierarchy(GameObject* gameObject);
    void RemoveFromHierarchy(GameObject* gameObject);

protected:
    std::unique_ptr<State_Base> currentState_;
    std::unique_ptr<EventState_Base> currentEventState_;
    std::unique_ptr<Hierarchy> hierarchy_; // Hierarchy管理クラス
};