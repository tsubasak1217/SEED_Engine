#pragma once
#include "Scene_Base.h"

///stl
//pointer
#include <memory>

///local
//state
#include "State/ITitleState.h"
//object
#include "../UI/UI.h"

class Scene_Title : public Scene_Base{
public:
    Scene_Title(SceneManager* pSceneManager);
    ~Scene_Title() override;

    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Finalize() override;

    void BeginFrame() override;
    void EndFrame() override;
    void HandOverColliders() override;

private:
    //============= state =============//
    std::unique_ptr<ITitleState> currentState_ = nullptr;

    //============= UI =============//
    std::unique_ptr<UI> titleLogo_    = nullptr;
    std::unique_ptr<UI> toNextButton_ = nullptr;
    std::unique_ptr<UI> toExitButton_ = nullptr;
};