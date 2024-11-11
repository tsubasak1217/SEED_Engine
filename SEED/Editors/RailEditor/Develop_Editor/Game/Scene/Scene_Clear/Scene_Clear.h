#pragma once
#include "Scene_Base.h"
#include <memory>

class Scene_Clear : public Scene_Base{
public:
    Scene_Clear(SceneManager* pSceneManager);
    ~Scene_Clear()override;
    void Initialize()override;
    void Finalize()override;
    void Update()override;
    void Draw()override;

private:

};