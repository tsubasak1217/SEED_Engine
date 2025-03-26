#pragma once

#include <Game/Objects/Stage/Blocks/Block_Base.h>

// 簡易版のためプレイヤーもブロックとして扱う
class Player : public Block_Base{
public:
    Player();
    ~Player()override;

public:
    void Initialize() override;
    void BeginFrame()override;
    void Update() override;
    void Draw() override;

    Player(const Player&) = delete;
    Player& operator=(const Player&) = delete;
    Player(Player&&) = default;
    Player& operator=(Player&&) = default;
    void Load(const nlohmann::json& json) override;
    

private:
    bool Move();
    void DrawHistory();

public:
    bool GetDoppelCreateOder(){ return steps_ >= doppelSteps_; }
    void SetIsPlaying(bool isPlaying){ isPlaying_ = isPlaying; }
    void SetIsDoppel(bool isDoppel){ isDoppel_ = isDoppel; }

private:

    static int playerCount_;// プレイヤーの数
    int32_t steps_;// 歩数
    int32_t doppelSteps_ = 5;// ドッペルゲンガーを生むまでの歩数
    bool isCreatedDoppel_ = false;// ドッペルゲンガーを生み出したかどうか
    bool isDoppel_ = false;// ドッペルゲンガーかどうか
    bool isPlaying_ = true;// プレイヤーが操作中かどうか
    Vector4 modelColor_ = { 1.0f,0.0f,0.0f,1.0f };// モデルの色

    // 移動履歴
    std::vector<Vector2i> moveHistory_;// 移動の入力履歴
    std::unique_ptr<Model> historyModel_;// 移動履歴のモデル
};