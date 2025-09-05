#pragma once

//============================================================================
//	include
//============================================================================
#include <SEED/Lib/enums/Direction.h>
#include <Game/Objects/Stage/Objects/Interface/IStageObject.h>
#include <Game/Objects/Stage/Objects/Player/State/PlayerStateController.h>

//============================================================================
//	Player class
//============================================================================
class Player :
    public IStageObject {
public:
    //========================================================================
    //	public Methods
    //========================================================================

    Player(GameObject2D* owner) : IStageObject(owner) {}
    Player() = default;
    ~Player() = default;

    // 初期化処理
    void Initialize(const std::string& filename) override;

    // 更新処理
    void Update() override;

    // 描画処理
    void Draw() override;

    // エディター
    void Edit() override;

    //--------- accessor -----------------------------------------------------

    void SetTranslate(const Vector2& translate) override { sprite_.translate = translate; }
    void SetSize(const Vector2& size) override { sprite_.size = size; }
    // プレイヤーをワープ状態にする
    void SetWarpState(const Vector2& start, const Vector2& target);

    const Sprite& GetSprite() const { return sprite_; }
    LR GetMoveDirection() const { return moveDirection_; }

    // クリア判定
    void IncreaseGoalTouchTime(){ goalTouchTime_ += ClockManager::DeltaTime(); }
    void ResetGoalTouchTime(){ goalTouchTime_ = 0.0f; }
    bool IsClearStage() const{ return goalTouchTime_ >= requiredGoalTime_; }
    // 死亡判定
    bool IsDead() const;
   

    // 入力検知
    bool IsFinishedWarp() const;
    bool IsPutBorder() const;
    bool IsRemoveBorder() const;
    bool IsJumpInput() const;


    // 状態通知
    void OnGroundTrigger();
    void OnCeilingTrigger();

private:
    //========================================================================
    //	private Methods
    //========================================================================

    //--------- variables ----------------------------------------------------

     // jsonパス
    const std::string kJsonPath_ = "Player/playerParameter.json";

    // 描画情報
    Sprite sprite_;
    // 向いている方向
    LR moveDirection_;

    // 入力管理
    std::unique_ptr<InputMapper<PlayerInputAction>> inputMapper_;
    // 状態管理
    std::unique_ptr<PlayerStateController> stateController_;

    // ゴールに関わる変数
    float goalTouchTime_ = 0.0f; // ゴールに触れてからの時間
    float requiredGoalTime_ = 2.0f; // ゴールに触れてからクリアになるまでの時間

    //--------- functions ----------------------------------------------------

    // json
    void ApplyJson();
    void SaveJson();

    // update
    void UpdateMoveDirection();
};