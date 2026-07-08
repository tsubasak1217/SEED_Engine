#pragma once
#include <SEED/Source/Basic/Scene/Scene_Base.h>
#include <SEED/Lib/Structs/TextBox.h>
#include <memory>

enum class GameState {
    Countdown,
    Playing,
    Finish
};

class Scene_Game : public SEED::Scene_Base {
public:
    Scene_Game();
    ~Scene_Game() override;

    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Finalize() override;

    static int GetFinalScore() { return finalScore_; }

private:
    GameState state_ = GameState::Countdown;
    float timer_ = 3.0f;
    int score_ = 0;
    static int finalScore_;

    std::shared_ptr<SEED::TextBox2D> mainText_;
    std::shared_ptr<SEED::TextBox2D> subText_;
    std::shared_ptr<SEED::TextBox2D> userText_;

    void SubmitScore();
};


