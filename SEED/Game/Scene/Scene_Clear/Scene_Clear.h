#pragma once
#include "Scene_Base.h"
#include <memory>

class Scene_Clear : public Scene_Base{
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