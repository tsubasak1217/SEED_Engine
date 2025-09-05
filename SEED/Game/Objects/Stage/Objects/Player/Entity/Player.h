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
    void Initialize() override;

    // 更新処理
    void Update() override;

    // 描画処理
    void Draw() override;

    // エディター
    void Edit() override;

    //--------- accessor -----------------------------------------------------

    void SetTranslate(const Vector2& translate) override { body_.translate = translate; }
    void SetSize(const Vector2& size) override;
    // プレイヤーをワープ状態にする
    void SetWarpState(const Vector2& start, const Vector2& target);

    const Sprite& GetSprite() const { return body_; }
    LR GetMoveDirection() const { return moveDirection_; }

    // クリア判定
    void IncreaseGoalTouchTime(){ goalTouchTime_ += ClockManager::DeltaTime(); }
    void ResetGoalTouchTime(){ goalTouchTime_ = 0.0f; }
    bool IsClearStage() const{ return goalTouchTime_ >= requiredGoalTime_; }

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

    // 画像ハンドルのマップ
    static inline std::unordered_map<std::string, int32_t> imageMap_;

    // 描画情報
    Sprite body_;
    Sprite legs_[2];
    static const int32_t baseLayer_ = 10;
    bool isMove_ = false; // 動き始めた瞬間か
    bool isHologram_ = false; // ホログラム状態か
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
    void SpriteMotion();
};