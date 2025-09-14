#pragma once
#include <SEED/Source/Basic/Scene/State_Base.h>
#include <json.hpp>
#include <memory>

#include <SEED/Lib/Input/InputMapper.h>
#include <Game/Scene/Input/PauseMenuInputActionEnum.h>
#include <Game/Objects/Transition/NextStageTransition.h>

class GameState_Play : public State_Base {
public:
    GameState_Play() = default;
    GameState_Play(Scene_Base* pScene);
    ~GameState_Play() override;

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

    std::unique_ptr<InputMapper<PauseMenuInputAction>> menuBarInputMapper_;

    // Audios
    const float kNormalBGMVolume_ = 0.32f;
    const float kHologramBGMVolume_ = 0.4f;
    // 音切り替えタイマーを
    Timer audioChangeTimer_ = Timer(0.48f);
    bool isAudioFading_ = false;
    bool isTargetHologram_ = false;
    bool isStartFade_ = false;

    // 現在ホログラム状態か
    bool isCurrentHologram_;
    bool isSameScene_;

    // transition
    Timer nextStageTimer_ = Timer(0.5f);
    Vector4 color_ = MyMath::FloatColor(238, 106, 2, 255, false);
    float nextStageTime_ = 0.6f;
    bool isResetStage_ = false;
    // parameters
    float stripHeight_ = 40.0f;
    float appearEndTimeT_ = 0.32f;
};