#pragma once
#include "Scene_Base.h"

///stl
//pointer
#include <memory>

///local
//state
#include "State/ITitleState.h"
//Scene
class Scene_Game;
//object
#include "../UI/UI.h"
#include "Model.h"
//lib
#include "../lib/thread/Thread.h"

class Scene_Title 
    : public Scene_Base{
    friend class LoadNextSceneThread;
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
    //============= UI =============//
    std::unique_ptr<UI> titleLogo_    = nullptr;
    std::unique_ptr<UI> toNextButton_ = nullptr;

    //============= PlayerModel =============//
    std::unique_ptr<Model> playerModel_ = nullptr;

    //============= nextScene =============//
    std::unique_ptr<Scene_Game> nextScene_ = nullptr;
    
public:
    void ToNextScene();

    UI* GetTitleLogo() const{ return titleLogo_.get(); }
    UI* GetToNextButton() const{ return toNextButton_.get(); }
};