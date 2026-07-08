#include <Game/Scene/Scene_Game/Scene_Game.h>
#include <SEED/Source/SEED.h>
#include <SEED/Source/Manager/InputManager/InputManager.h>
#include <SEED/Source/Manager/ClockManager/ClockManager.h>
#include <SEED/Source/Manager/HttpManager/HttpManager.h>
#include <Game/GameSystem.h>
#include <Game/Manager/UserManager.h>
#include <json.hpp>

int Scene_Game::finalScore_ = 0;

Scene_Game::Scene_Game()
    : Scene_Base()
    , mainText_(std::make_shared<SEED::TextBox2D>())
    , subText_(std::make_shared<SEED::TextBox2D>())
    , userText_(std::make_shared<SEED::TextBox2D>()) {
}

Scene_Game::~Scene_Game() {
}

void Scene_Game::Initialize() {
    Scene_Base::Initialize();

    state_ = GameState::Countdown;
    timer_ = 3.0f;
    score_ = 0;

    mainText_->SetFont("");
    mainText_->fontSize = 80.0f;
    mainText_->size = { 1280.0f, 150.0f };
    mainText_->transform.translate = { 640.0f, 300.0f };
    mainText_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
    mainText_->alignX = SEED::TextAlignX::CENTER;
    mainText_->textBoxVisible = false;

    subText_->SetFont("");
    subText_->fontSize = 40.0f;
    subText_->size = { 1280.0f, 100.0f };
    subText_->transform.translate = { 640.0f, 450.0f };
    subText_->color = { 0.9f, 0.9f, 0.9f, 1.0f };
    subText_->alignX = SEED::TextAlignX::CENTER;
    subText_->textBoxVisible = false;

    userText_->SetFont("");
    userText_->fontSize = 20.0f;
    userText_->size = { 1280.0f, 30.0f };
    userText_->transform.translate = { 20.0f, 20.0f };
    userText_->color = { 0.7f, 0.7f, 1.0f, 1.0f };
    userText_->alignX = SEED::TextAlignX::LEFT;
    userText_->textBoxVisible = false;
}

void Scene_Game::Update() {
    Scene_Base::Update();
    userText_->text = "User: " + UserManager::GetInstance()->GetUserName();

    float dt = SEED::ClockManager::DeltaTime();
    timer_ -= dt;

    switch (state_) {
    case GameState::Countdown:
        if (timer_ <= 0.0f) {
            state_ = GameState::Playing;
            timer_ = 10.0f;
        }
        break;

    case GameState::Playing:
        if (SEED::Input::IsTriggerMouse(SEED::MOUSE_BUTTON::LEFT)) {
            score_++;
        }
        if (timer_ <= 0.0f) {
            state_ = GameState::Finish;
            timer_ = 3.0f;
            finalScore_ = score_;
            SubmitScore();
        }
        break;

    case GameState::Finish:
        if (timer_ <= 0.0f) {
            SEED::GameSystem::ChangeScene("Clear");
        }
        break;
    }
}

void Scene_Game::Draw() {
    Scene_Base::Draw();

    switch (state_) {
    case GameState::Countdown:
        mainText_->text = std::to_string((int)timer_ + 1);
        subText_->text = "Get Ready!";
        break;

    case GameState::Playing:
        mainText_->text = "Score: " + std::to_string(score_);
        subText_->text = "Time: " + std::to_string((int)timer_ + 1);
        break;

    case GameState::Finish:
        mainText_->text = "FINISH!";
        subText_->text = "Your Score: " + std::to_string(score_);
        break;
    }

    SEED::Instance::DrawText2D(*mainText_);
    SEED::Instance::DrawText2D(*subText_);
    SEED::Instance::DrawText2D(*userText_);
}

void Scene_Game::Finalize() {
    Scene_Base::Finalize();
}

void Scene_Game::SubmitScore() {
    nlohmann::json j;
    j["score"] = score_;
    std::string body = j.dump();

    SEED::HttpManager::RequestAsync("POST", "http://localhost:3000/scores", body, []([[maybe_unused]] bool success, [[maybe_unused]] const std::string& responseBody) {
        // Log or handle success/failure if needed
    }, UserManager::GetInstance()->GetToken());
}
