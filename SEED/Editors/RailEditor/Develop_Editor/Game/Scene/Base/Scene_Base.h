#pragma once
#include <State_Base.h>
#include <memory>

class SceneManager;

class Scene_Base{
public:
    Scene_Base() = default;
    Scene_Base(SceneManager* pSceneManager);
    virtual ~Scene_Base(){};
    virtual void Initialize() = 0;
    virtual void Finalize() = 0;
    virtual void Update();
    virtual void Draw();

public:
    void ChangeScene(Scene_Base* nextScene);
    void ChangeState(State_Base* nextState);

public:
    SceneManager* pSceneManager_;

protected:
    std::unique_ptr<State_Base> currentState_;
};