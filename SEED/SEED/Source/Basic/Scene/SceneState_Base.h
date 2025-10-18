#pragma once
#include <string>
#include <cstdint>
#include <SEED/Source/Manager/PostEffectSystem/PostEffectSystem.h>

class Scene_Base;

// ゲームの基底ステート
class SceneState_Base {
public:
    SceneState_Base() = default;
    SceneState_Base(Scene_Base* pScene) : pScene_(pScene) {};
    virtual ~SceneState_Base() {};
    virtual void Initialize() {};
    virtual void Finalize() {};
    virtual void Update() = 0;
    virtual void Draw() = 0;
    virtual void BeginFrame() = 0;
    virtual void EndFrame() = 0;
    virtual void HandOverColliders() = 0;
    virtual void ManageState() = 0;

protected:
    Scene_Base* pScene_ = nullptr;
};
