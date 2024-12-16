#pragma once
#include "Scene_Base.h"
#include <memory>

class Scene_Title : public IScene{
public:
    Scene_Title();
    ~Scene_Title()override;
    void Initialize()override;
    void Finalize()override;
    void Update()override;
    void Draw()override;

private:

};