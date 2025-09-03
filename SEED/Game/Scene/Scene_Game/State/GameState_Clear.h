#pragma once
#include <SEED/Source/Basic/Scene/State_Base.h>
#include <json.hpp>
#include <memory>

#include <SEED/Lib/Input/InputMapper.h>
#include <SEED/Lib/Structs/Sprite.h>
#include <SEED/Lib/Structs/TextBox.h>
#include <Game/Scene/Input/PauseMenuInputActionEnum.h>

class GameState_Clear : public State_Base {
public:
    GameState_Clear() = default;
    GameState_Clear(Scene_Base* pScene);
    ~GameState_Clear() override;

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

    // メニュー項目のスプライトとテキスト
    std::array<Sprite, 2> MenuBack_;
    std::array<TextBox2D,2> MenuText_;
    int32_t currentMenu_ = 0;
    Vector2 menuPos_[2];
    Vector2 menuSize_;

    //クリアテキスト
    TextBox2D ClearText_;

    std::unique_ptr<InputMapper<PauseMenuInputAction>> inputMapper_;
    bool changeStateRequest_ = false;
};