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
    void Finalize()override;
    void Update()override;
    void Draw()override;
    void BeginFrame()override;
    void EndFrame()override;
    void HandOverColliders()override;

private:

};