#pragma once

//============================================================================
//	include
//============================================================================
#include <SEED/Lib/Structs/Sprite.h>
#include <SEED/Lib/enums/Direction.h>
#include <SEED/Lib/Functions/myFunc/Easing.h>

//============================================================================
//	BorderLine class
//============================================================================
class BorderLine {
public:
    //========================================================================
    //	public Methods
    //========================================================================

    BorderLine() = default;
    ~BorderLine() = default;

    // 初期化処理
    void Initialize();

    // 更新処理
    void Update(const Vector2& translate, float sizeY, float tileSize);

    // 描画処理
    void Draw();

    // エディター
    void Edit(const Vector2& playerTranslate, float tileSize);

    // json
    void FromJson(const nlohmann::json& data);
    void ToJson(nlohmann::json& data);

    void ShakeBorderLine();

    //--------- accessor -----------------------------------------------------

    // アクティブ状態にする
    void SetActivate();
    // 非アクティブ状態にする
    void SetDeactivate();
    // 向きを設定
    void SetDirection(LR direction) { direction_ = direction; }

    const Sprite& GetSprite() const { return sprite_; }
    // 向きを取得
    LR GetDirection() const { return direction_; }
    // 現在アクティブ状態かどうか(アクティブ == 置かれている状態)
    bool IsActive() const { return currentState_ == State::Active; }
    // 非アクティブ状態に遷移可能かどうか
    bool CanTransitionDisable(const Vector2& playerTranslate, float tileSize) const;
    //シェイク中か
    bool IsShaking() const { return isShaking_; }
    //シェイクフラグ設定
    void SetIsShaking(bool flag) { isShaking_ = flag; }

    void SetTargetX(float x);

    void SetPaused(bool isPaused) { isPaused_ = isPaused; }

private:
    //========================================================================
    //	private Methods
    //========================================================================

    //--------- structure ----------------------------------------------------

    // 現在の状態
    enum class State {

        Disable, // 非アクティブ状態
        Active,  // アクティブ状態
    };

    // 補間構造体
    struct LerpParam {

        float startX;   // 開始X座標
        float targetX;  // 補間先X座標
        float elapsed;  // 経過時間
        float duration; // 補間時間
        Easing::Type easing; // x座標の補間処理
        bool running; // 補間中か
    };

    //--------- variables ----------------------------------------------------

    // 現在の状態
    State currentState_;

    // 描画情報
    Sprite sprite_;
    uint32_t transparentTextureGH_; // 非アクティブ状態のテクスチャ
    uint32_t opaqueTextureGH_;      // アクティブ状態のテクスチャ

    // parameters
    float playerToDistance_;   // プレイヤーとの判定を取るまで距離
    float offsetTranslateY_;   // プレイヤーのY座標からのオフセット
    LerpParam lerpXParam_;
    Vector2 initialSize_;// 初期サイズ
    float scrollValue_ = 0.0f;// スクロール値
    LR direction_; // 向き

    // 揺れ情報
    float shakeDuration_ = 0.2f;
    float shakeAmount_ = 5.0f;
    float elapsedTime_ = 0.0f;
    float shakeStartPosX_ = 0.0f;
    bool isShaking_ = false;

    //ポーズ中かどうか
    bool isPaused_ = false;

    //--------- functions ----------------------------------------------------

    // update
    void UpdateSprite(const Vector2& translate, float sizeY, float tileSize);

    // helper
    // プレイヤーと一定距離近づいたか
    bool CheckPlayerToDistance(const Vector2& playerTranslate, float tileSize) const;
};