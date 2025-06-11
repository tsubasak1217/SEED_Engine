#pragma once
#include <SEED.h>

class Scene_Base;

class Event_Base{
public:
    Event_Base() = default;
    Event_Base(Scene_Base* parentScene);
    virtual ~Event_Base();
    virtual void Initialize();
    virtual void Update();
    virtual void Draw();

private:
    // イベントを発行しているシーン
    Scene_Base* pScene_;
};