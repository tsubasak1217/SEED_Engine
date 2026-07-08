#pragma once
#include <SEED/Source/Basic/Scene/Scene_Base.h>
#include <SEED/Lib/Structs/TextBox.h>
#include <memory>
#include <vector>
#include <string>

struct ClearState {
    bool isFetching = false;
};

class Scene_Clear : public SEED::Scene_Base {
public:
    Scene_Clear();
    ~Scene_Clear() override;

    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Finalize() override;

private:
    std::shared_ptr<SEED::TextBox2D> scoreText_;
    std::shared_ptr<SEED::TextBox2D> rankingText_;
    std::shared_ptr<SEED::TextBox2D> infoText_;
    std::shared_ptr<ClearState> state_;

    void FetchRankings();
};
