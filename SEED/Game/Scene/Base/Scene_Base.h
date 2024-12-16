#pragma once
#include <State_Base.h>
#include <memory>

class IScene{
public:
    IScene() = default;
    virtual ~IScene(){};
    virtual void Initialize() = 0;
    virtual void Finalize() = 0;
    virtual void Update();
    virtual void Draw();

public:
    void ChangeState(State_Base* nextState);
    IScene* GetNextScene()const{ return nextScene_; }
    void SetNextScene(IScene* nextScene){ nextScene_ = nextScene; }

protected:
    IScene* nextScene_ = nullptr;
    std::unique_ptr<State_Base> currentState_;
};