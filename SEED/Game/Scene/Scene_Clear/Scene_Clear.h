#pragma once

//host
#include <Game/Scene/Base/Scene_Base.h>

///stl
#include <memory>

///local
//light
#include <SEED/Lib/Structs/DirectionalLight.h>
//objects
#include <SEED/Lib/Structs/Model.h>
#include <Game/Objects/Base/BaseObject.h>

class Scene_Clear
    : public Scene_Base{
public:
    Scene_Clear();
    ~Scene_Clear()override;
    void Initialize()override;
    void Finalize()override;
    void Update()override;
    void Draw()override;
    void BeginFrame()override;
    void EndFrame()override;
    void HandOverColliders()override;
};