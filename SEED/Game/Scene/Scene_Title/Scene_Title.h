#pragma once
#include "Scene_Base.h"
#include <memory>

#include "../UI/GraphicalUserInterface.h"

class Scene_Title : public Scene_Base{
public:
    Scene_Title(SceneManager* pSceneManager);
    ~Scene_Title()override;
    void Initialize()override;
    void Finalize()override;
    void Update()override;
    void Draw()override;
    void BeginFrame()override;
    void EndFrame()override;
    void HandOverColliders()override;

private:
    std::unique_ptr<GUI> titleLogo_;
    std::unique_ptr<GUI> toNextButton;
    std::unique_ptr<GUI> toExitButton;
};