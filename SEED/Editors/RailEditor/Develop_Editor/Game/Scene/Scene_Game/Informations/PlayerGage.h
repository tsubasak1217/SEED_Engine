#pragma once
#include "Player.h"

class PlayerGage{
public:
    PlayerGage();
    ~PlayerGage();

    void Initialize();
    void Update();
    void Draw();

    void SetPlayer(Player* player){ pPlayer_ = player; }

private:

    void CalcScoreGage();
    void StretchShotGage();
    void EditByImGui();
    void OutputToJson();
    void LoadFromJson();

private:
    Player* pPlayer_ = nullptr;

    // ゲージ関連
    std::unique_ptr<Sprite>shotGage_ = nullptr;
    std::unique_ptr<Sprite>shotGageFrame_ = nullptr;
    std::unique_ptr<Sprite>scoreGageBack_ = nullptr;
    std::vector<std::unique_ptr<Sprite>>score_;
    Vector2 scoreBasePos_ = { 0.0f,0.0f };
    Vector2 scoreBaseScale_ = { 0.0f,0.0f };
    Vector2 scoreOffset_ = { 0.0f,0.0f };
    float numberGraphWidth_ = 42.0f;
};