#pragma once
#include <SEED/Source/Basic/Scene/State_Base.h>
#include <json.hpp>

#include <SEED/Lib/Structs/Sprite.h>
#include <SEED/Lib/Structs/TextBox.h>

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

    Sprite MenuBack_[3];
    TextBox2D MenuText_[3];
    int32_t currentMenu_ = 0;

    Vector2 menuPos_[3];
    Vector2 menuSize_;
};