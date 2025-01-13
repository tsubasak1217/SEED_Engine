#pragma once
#include <memory>
#include <SceneManager.h>
#include <State_Base.h>

class Scene_Base{
public:
    Scene_Base() = default;
    Scene_Base(SceneManager* pSceneManager);
    virtual ~Scene_Base(){};
    virtual void Initialize() = 0;
    virtual void Finalize() = 0;
    virtual void Update() = 0;
    virtual void Draw() = 0;
    virtual void BeginFrame() = 0;
    virtual void EndFrame() = 0;
    virtual void HandOverColliders() = 0;

public:
    void ChangeScene(Scene_Base* nextScene);
    void ChangeState(State_Base* nextState);

public:
    SceneManager* pSceneManager_;

protected:
    std::unique_ptr<State_Base> currentState_;
};