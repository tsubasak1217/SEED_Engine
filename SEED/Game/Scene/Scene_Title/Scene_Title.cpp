#include <Game/Scene/Scene_Title/Scene_Title.h>
#include <SEED/Source/SEED.h>
#include <SEED/Source/Manager/InputManager/InputManager.h>
#include <Game/GameSystem.h>

Scene_Title::Scene_Title()
    : Scene_Base()
    , titleText_(std::make_unique<SEED::TextBox2D>())
    , infoText_(std::make_unique<SEED::TextBox2D>()) {
}

Scene_Title::~Scene_Title() {
}

void Scene_Title::Initialize() {
    Scene_Base::Initialize();

    titleText_->SetFont("");
    titleText_->text = "Clicker Game 10s";
    titleText_->transform.translate = { 640.0f, 200.0f };
    titleText_->fontSize = 60.0f;
    titleText_->size = { 1280.0f, 100.0f };
    titleText_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
    titleText_->alignX = SEED::TextAlignX::CENTER;
    titleText_->textBoxVisible = false;

    infoText_->SetFont("");
    infoText_->text = "Click as many times as you can in 10 seconds!\n\nClick to Start";
    infoText_->transform.translate = { 640.0f, 450.0f };
    infoText_->fontSize = 30.0f;
    infoText_->size = { 1280.0f, 300.0f };
    infoText_->color = { 0.8f, 0.8f, 0.8f, 1.0f };
    infoText_->alignX = SEED::TextAlignX::CENTER;
    infoText_->textBoxVisible = false;
}

void Scene_Title::Update() {
    Scene_Base::Update();

    if (SEED::Input::IsTriggerMouse(SEED::MOUSE_BUTTON::LEFT)) {
        SEED::GameSystem::ChangeScene("Game");
    }
}

void Scene_Title::Draw() {
    Scene_Base::Draw();
    SEED::Instance::DrawText2D(*titleText_);
    SEED::Instance::DrawText2D(*infoText_);
}

void Scene_Title::Finalize() {
    Scene_Base::Finalize();
}
