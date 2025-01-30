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
#include "Sprite.h"
#include "Model.h"

class Scene_Title
    : public Scene_Base{
    friend class LoadNextSceneThread;
public:
    Scene_Title();
    ~Scene_Title() override;

    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Finalize() override;

    void BeginFrame() override;
    void EndFrame() override;
    void HandOverColliders() override;

private:
    //============= PlayerModel =============//
    std::unique_ptr<Model> playerModel_ = nullptr;

    //============= UI =============//
    std::unique_ptr<Sprite> titleLogo_ = nullptr;
    std::unique_ptr<Sprite> howToStartUI_ = nullptr; // "Press Any Button" 的なやつ

public:
};