#pragma once
#include "Scene_Base.h"
#include <memory>

class Scene_Clear : public IScene{
public:
    Scene_Clear();
    ~Scene_Clear()override;
    void Initialize()override;
    void Finalize()override;
    void Update()override;
    void Draw()override;

private:

};