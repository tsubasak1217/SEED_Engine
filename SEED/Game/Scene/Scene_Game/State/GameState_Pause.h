#pragma once
#include <SEED/Source/Basic/Scene/State_Base.h>
#include <json.hpp>
#include <memory>

#include <SEED/Lib/Input/InputMapper.h>
#include <SEED/Lib/Structs/Sprite.h>
#include <SEED/Lib/Structs/TextBox.h>
#include <SEED/Lib/Structs/Timer.h>
#include <Game/Scene/Input/PauseMenuInputActionEnum.h>

// transition
#include <SEED/Source/Manager/SceneTransitionDrawer/SceneTransitionDrawer.h>
#include <SEED/Source/Basic/SceneTransition/HexagonTransition.h>

struct PauseItem{
    Transform2D start;
    Transform2D end;
    Sprite backSprite;
    TextBox2D text;
    Vector2 backSpriteSize;
    Vector4 backColor;
    Timer selectTime = Timer(0.2f);
    static inline float kSelectTimeMax = 0.2f;
    nlohmann::json ToJson() const;
    void FromJson(const nlohmann::json& data);
    void Edit();
};

class GameState_Pause : public State_Base {
public:
    GameState_Pause() = default;
    GameState_Pause(Scene_Base* pScene);
    ~GameState_Pause() override;


public:
    void Initialize() override;
    void Finalize() override;
    void Update() override;
    void Draw() override;
    void BeginFrame() override;
    void EndFrame() override;
    void HandOverColliders() override;
    void ManageState() override;

private:

    std::unique_ptr<InputMapper<PauseMenuInputAction>> inputMapper_;
    bool isExit_ = false;

    std::vector<PauseItem> pauseItems_;
    PauseItem* currentItem_ = nullptr;
    int32_t currentMenu_ = 0;
    int32_t kMenuCount = 3;
    Timer menuTimer_ = Timer(1.0f);
    const float kBGMVolume_ = 0.16f;

    // transition
    Timer nextStageTimer_ = Timer(0.5f);
    Vector4 color_ = MyMath::FloatColor(238, 106, 2, 255, false);
    float nextStageTime_ = 0.6f;
    bool isResetStage_ = false;
    // parameters
    float stripHeight_ = 40.0f;
    float appearEndTimeT_ = 0.32f;

    // 現在のシーンの終わり時間と次のシーンの始まり時間(遷移用)
    bool isExitScene_ = false;
    Timer sceneChangeTimer_ = Timer(1.0f);
    float nextSceneEnterTime_ = 0.75f;

    //hexagon遷移用
    std::vector<Vector4> hexagonColors_;
    float hexagonSize_ = 80.0f;

private:
    void MenuItemsToJson();
};