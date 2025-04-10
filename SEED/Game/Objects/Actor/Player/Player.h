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
    void SolveCollision(Block_Base* other) override;
    

private:
    bool Move();
    void DrawHistory();

public:
    static int32_t GetDoppelSteps(){ return doppelSteps_; }
    static void SetDoppelSteps(int32_t steps){ doppelSteps_ = steps; }
    bool GetDoppelCreateOder(){ return steps_ >= doppelSteps_; }
    void SetIsPlaying(bool isPlaying){ isPlaying_ = isPlaying; }
    void SetIsDoppel(bool isDoppel){ isDoppel_ = isDoppel; }
    int32_t GetCharacterNo(){ return characterNo_; }
    bool GetIsDoppel(){ return isDoppel_; }
    bool GetIsCreatedDoppel(){ return isCreatedDoppel_; }
    void SetIsCreatedDoppel(bool isCreatedDoppel){ isCreatedDoppel_ = isCreatedDoppel; }
    bool GetIsFoundDoppel(){ return isFoundDoppel_; }
    void SetIsFoundDoppel(bool isFoundDoppel){ isFoundDoppel_ = isFoundDoppel; }
    Vector2i GetForwardVec();
private:

    static int playerCount_;// プレイヤーの数
    int32_t steps_;// 歩数
    static int32_t doppelSteps_;// ドッペルゲンガーを生むまでの歩数
    int32_t characterNo_;// 何番目のキャラクターか認識するための変数
    bool isCreatedDoppel_ = false;// ドッペルゲンガーを生み出したかどうか
    bool isFoundDoppel_ = false;// ドッペルゲンガーを見つけたかどうか
    bool isDoppel_ = false;// ドッペルゲンガーかどうか
    bool isPlaying_ = true;// プレイヤーが操作中かどうか
    bool isMoveInput_ = false;// 移動の入力があったかどうか
    Vector4 modelColor_ = { 1.0f,0.0f,0.0f,1.0f };// モデルの色

    // 移動履歴
    std::vector<Vector2i> moveHistory_;// 移動の入力履歴
    std::unique_ptr<Model> historyModel_;// 移動履歴のモデル
};