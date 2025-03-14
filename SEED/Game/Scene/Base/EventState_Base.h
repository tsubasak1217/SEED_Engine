#pragma once
#include <SEED/Source/SEED.h>
#include <SEED/Lib/Structs/Sprite.h>
class Scene_Base;

class EventState_Base {
public:
    EventState_Base() = default;
    EventState_Base(Scene_Base* parentState);
    virtual ~EventState_Base();
    virtual void Initialize();
    virtual void Update();
    virtual void Draw();

private:
    // イベントを発行しているステート
    Scene_Base* pScene_;
};