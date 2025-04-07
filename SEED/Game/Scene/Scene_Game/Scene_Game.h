#pragma once
#include "Scene_Base.h"

// external
#include <list>
#include <memory>
#include <string>
#include <vector>

// local
#include <Model.h>
#include <Sprite.h>
#include <Triangle.h>

// camera
#include "FollowCamera.h"

//editor
#include "../Game/Editor/FieldEditor.h"

// objects
#include "../Game/Objects/Assets/Actor/SampleCharacter/SampleCharacter.h"

// manager
#include "../Game/Manager/StageManager.h"

class Scene_Game
    : public Scene_Base{

public:
    Scene_Game();
    ~Scene_Game() override;
    void Initialize() override;
    void Finalize() override;
    void Update() override;
    void Draw() override;
    void BeginFrame() override;
    void EndFrame() override;
    void HandOverColliders() override;

public:
    StageManager* Get_pStageManager(){ return stageManager_; }
    StageManager& Get_StageManager(){ return *stageManager_; }
    FollowCamera* Get_pCamera(){ return followCamera_.get(); }
    BaseCharacter* Get_pPlayer(){ return player_.get(); }

    // 禁忌
    FieldEditor* GetFieldEditor();

private:

    // EngineObjects
    std::unique_ptr<DirectionalLight> directionalLight_ = nullptr;
    std::vector<std::unique_ptr<PointLight>> pointLights_;
    std::vector<std::unique_ptr<SpotLight>> spotLights_;
    std::unique_ptr<FollowCamera> followCamera_ = nullptr;

    // GameObjects
    std::unique_ptr<SampleCharacter> player_ = nullptr;

    // Manager
    StageManager* stageManager_;

    // Sprite
    std::unique_ptr<Sprite> backSprite_ = nullptr;

};