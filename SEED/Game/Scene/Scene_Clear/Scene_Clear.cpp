#include <Game/Scene/Scene_Clear/Scene_Clear.h>
#include <Game/Scene/Scene_Game/Scene_Game.h>
#include <SEED/Source/SEED.h>
#include <SEED/Source/Manager/InputManager/InputManager.h>
#include <SEED/Source/Manager/HttpManager/HttpManager.h>
#include <Game/GameSystem.h>
#include <json.hpp>
#include <algorithm>

Scene_Clear::Scene_Clear()
    : Scene_Base()
    , scoreText_(std::make_shared<SEED::TextBox2D>())
    , rankingText_(std::make_shared<SEED::TextBox2D>())
    , infoText_(std::make_shared<SEED::TextBox2D>())
    , state_(std::make_shared<ClearState>()) {
}

Scene_Clear::~Scene_Clear() {
}

void Scene_Clear::Initialize() {
    Scene_Base::Initialize();

    int finalScore = Scene_Game::GetFinalScore();
    scoreText_->SetFont("");
    scoreText_->text = "Your Score: " + std::to_string(finalScore);
    scoreText_->fontSize = 50.0f;
    scoreText_->size = { 1280.0f, 100.0f };
    scoreText_->transform.translate = { 640.0f, 100.0f }; // Slightly higher
    scoreText_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
    scoreText_->alignX = SEED::TextAlignX::CENTER;
    scoreText_->textBoxVisible = false;

    rankingText_->SetFont("");
    rankingText_->text = "Fetching Rankings...";
    rankingText_->fontSize = 30.0f;
    rankingText_->size = { 1280.0f, 400.0f };
    rankingText_->transform.translate = { 640.0f, 400.0f }; // Moved down to avoid overlap
    rankingText_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
    rankingText_->alignX = SEED::TextAlignX::CENTER;
    rankingText_->alignY = SEED::TextAlignY::TOP; // Ensure it starts from top of its box
    rankingText_->textBoxVisible = false;

    infoText_->SetFont("");
    infoText_->text = "Click to Title";
    infoText_->fontSize = 25.0f;
    infoText_->size = { 1280.0f, 50.0f };
    infoText_->transform.translate = { 640.0f, 650.0f };
    infoText_->alignX = SEED::TextAlignX::CENTER;
    infoText_->color = { 0.7f, 0.7f, 0.7f, 1.0f };
    infoText_->textBoxVisible = false;

    FetchRankings();
}

void Scene_Clear::Update() {
    Scene_Base::Update();

    if (SEED::Input::IsTriggerMouse(SEED::MOUSE_BUTTON::LEFT)) {
        SEED::GameSystem::ChangeScene("Title");
    }
}

void Scene_Clear::Draw() {
    Scene_Base::Draw();
    SEED::Instance::DrawText2D(*scoreText_);
    SEED::Instance::DrawText2D(*rankingText_);
    SEED::Instance::DrawText2D(*infoText_);
}

void Scene_Clear::Finalize() {
    Scene_Base::Finalize();
}

void Scene_Clear::FetchRankings() {
    if (state_->isFetching) return;
    state_->isFetching = true;

    auto s = state_;
    auto rt = rankingText_;

    SEED::HttpManager::GetAsync("http://localhost:3000/scores", [s, rt](bool success, const std::string& body) {
        s->isFetching = false;
        if (!success) {
            rt->text = "Failed to fetch rankings.";
            return;
        }

        try {
            auto json = nlohmann::json::parse(body);
            if (json.is_array()) {
                struct RankingEntry { int score; std::string name; };
                std::vector<RankingEntry> entries;
                for (const auto& item : json) {
                    if (item.contains("score") && item["score"].is_number()) {
                        std::string name = "Unknown";
                        if(item.contains("user") && item["user"].is_object() && item["user"].contains("name")){
                            name = item["user"]["name"];
                        }
                        entries.push_back({ item["score"].get<int>(), name });
                    }
                }

                std::sort(entries.begin(), entries.end(), [](const RankingEntry& a, const RankingEntry& b) {
                    return a.score > b.score;
                });

                std::string rankStr = "=== Top 5 Rankings ===\n\n";
                int count = 0;
                for (const auto& entry : entries) {
                    rankStr += std::to_string(count + 1) + ". " + entry.name + ": " + std::to_string(entry.score) + "\n";
                    if (++count >= 5) break;
                }
                rt->text = rankStr;
            }
        } catch (...) {
            rt->text = "Error parsing rankings.";
        }
    });
}

