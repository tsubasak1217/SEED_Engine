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
#include <SEED/Source/Basic/Object/GameObject2D.h>
#include <SEED/Lib/Structs/Timer.h>


/// <summary>
/// クリアシーンクラス
/// </summary>
class Scene_Clear
    : public SEED::Scene_Base{
public:
    Scene_Clear();
    ~Scene_Clear()override;
    void Initialize()override;
    void Update()override;
    void Draw()override;
    void BeginFrame()override;
    void EndFrame()override;

};