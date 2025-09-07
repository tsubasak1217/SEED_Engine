#pragma once

//============================================================================
//	include
//============================================================================
#include <SEED/Lib/Functions/myFunc/Easing.h>
#include <SEED/Lib/Structs/Timer.h>
#include <Game/Objects/Stage/Objects/Interface/IStageObject.h>

//============================================================================
//	Warp class
//============================================================================
class Warp :
    public IStageObject {
public:
    //========================================================================
    //	public Methods
    //========================================================================

    Warp(GameObject2D* owner) : IStageObject(owner) {}
    ~Warp() = default;

    void Initialize() override;

    void Update() override;

    void Draw() override;

    void Edit() override;

    //--------- accessor -----------------------------------------------------

    void SetTranslate(const Vector2& translate) override;
    void SetSize(const Vector2& size) override;

    // 自身のワープインデックスを作成時に設定
    void SetWarpIndex(uint32_t warpIndex);
    // 通知を行わせる
    void SetNotification();
    // ワープ中にする
    void SetWarping() { currentState_ = State::Warping; }
    void SetWarpNotPossible() { currentState_ = State::WarpNotPossible; }
    void SetWarpEmpty() { currentState_ = State::WarpEmpty; }
    // 通常更新状態にする
    void SetNone();
    void ResetAnimation();

    uint32_t GetWarpIndex() const { return warpIndex_; }
    bool IsStateNotification()const { return currentState_ == State::Notification; }
    bool IsStateNone()const { return currentState_ == State::None; }
    bool IsStateWarpNotPossible()const { return currentState_ == State::WarpNotPossible; }
    bool IsStateWarpEmpty()const { return currentState_ == State::WarpEmpty; }
private:
    //========================================================================
    //	private Methods
    //========================================================================

    //--------- structure ----------------------------------------------------

    // 状態
    enum class State {

        None,            // 通常更新中(ワープ可能)
        Notification,    // ワープ通知
        Warping,         // ワープ中
        WarpNotPossible, // ワープ不可
        WarpEmpty,       // 空白に埋もれている状態
    };

    //--------- variables ----------------------------------------------------

    // 現在の状態
    State currentState_;
    // 自身のワープインデックス
    uint32_t warpIndex_;

    // 表示スプライト
    std::vector<Sprite> frameSprites_;   // 外側から内側に小さくするフレーム
    std::vector<Timer> frameAnimTimers_; // アニメーションタイマー

    // パラメータ
    // NoneState
    Timer frameAnimTimer_;       // アニメーションタイマー
    int frameSpriteCount_;       // 使用するフレームスプライト数
    float innerScalingSpacing_;  // フレームを小さくする間の間隔
    float initScale_;            // 初期スケール
    Easing::Type scalineEasing_; // 内側にスケーリング処理を行う際のイージング

    //--------- functions ----------------------------------------------------

    // init
    void InitializeFrameSprites();

    // update
    void UpdateState();
    void UpdateNone();
    void UpdateAlways();
};