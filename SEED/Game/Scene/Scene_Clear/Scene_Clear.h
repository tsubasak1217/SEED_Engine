#pragma once

//host
#include <SEED/Source/Basic/Scene/Scene_Base.h>

///stl
#include <memory>

///local
//light
#include <SEED/Lib/Structs/DirectionalLight.h>
//objects
#include <SEED/Lib/Structs/Model.h>
#include <SEED/Source/Basic/Object/GameObject.h>
#include <SEED/Lib/Structs/Timer.h>

class Scene_Clear
    : public Scene_Base{
public:
    Scene_Clear();
    ~Scene_Clear()override;
    void Initialize()override;
    void Update()override;
    void Draw()override;
    void BeginFrame()override;
    void EndFrame()override;

private:
    void CheckStep();

private:
    int32_t step_ = 0;
    int32_t kMaxStep_ = 4;// 最大ステップ数
    Timer stepTimer_ = Timer(0.7f);

    bool sceneChangeOrder = false; // シーン変更のオーダーが出ているかどうか
};