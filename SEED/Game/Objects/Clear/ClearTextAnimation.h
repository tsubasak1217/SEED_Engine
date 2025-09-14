#pragma once

//============================================================================
//	include
//============================================================================
#include <SEED/Lib/Structs/Sprite.h>
#include <SEED/Lib/Structs/Timer.h>

//============================================================================
//	ClearTextAnimation class
//============================================================================
class ClearTextAnimation {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	ClearTextAnimation() = default;
	~ClearTextAnimation() = default;

    // 初期化処理
    void Initialize();

    // 更新処理
    void Update();

    // 描画処理
    void Draw();

    // エディター
    void Edit();

	//--------- accessor -----------------------------------------------------

    // アニメーションの遷移指示
    void StartIn();
    void StartOut();
    void StopAnim();

    // アニメーションが終了したかどうか
    bool IsFinishedInAnim() const;
    bool IsFinishedOutAnim() const;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- structure ----------------------------------------------------

    // アニメーション状態
    enum class AnimState {

        None,  // 何もしない
        In,    // 登場
        Out,   // 退場
    };

	//--------- variables ----------------------------------------------------

    // 現在の状態
    AnimState currentState_;

    // SE
    bool isPlayedSe_ = false;
    Timer seTimer_;

    // 文字表示
    Sprite topText_;    // 上
    Sprite bottomText_; // 下
    Timer animTimer_;   // 時間管理
    Vector2 targetTranslate_;         // 目標座標
    float textAngle_;                 // 回転角
    float textDistance_;              // 目標座標からの距離
    float textScale_;            // 文字スケール
    Easing::Type inAnimEasing_;  // 登場
    Easing::Type outAnimEasing_; // 退場

	//--------- functions ----------------------------------------------------

    // json
    void ApplyJson();
    void SaveJson();

    // update
    /// In
    void UpdateInAnim();
    /// Out
    void UpdateOutAnim();

    // helper
    float DegreeToRadian(float degree);
};