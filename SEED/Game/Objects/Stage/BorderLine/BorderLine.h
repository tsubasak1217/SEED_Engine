#pragma once

//============================================================================
//	include
//============================================================================
#include <SEED/Lib/Structs/Sprite.h>
#include <SEED/Lib/enums/Direction.h>

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
    void Update(const Vector2& translate, float sizeY);

    // 描画処理
    void Draw();

    // エディター
    void Edit(const Sprite& playerSprite);

    // json
    void FromJson(const nlohmann::json& data);
    void ToJson(nlohmann::json& data);

    //--------- accessor -----------------------------------------------------

    // アクティブ状態にする
    void SetActivate();
    // 非アクティブ状態にする
    void SetDeactivate();

    const Sprite& GetSprite() const { return sprite_; }
    // 現在アクティブ状態かどうか(アクティブ == 置かれている状態)
    bool IsActive() const { return currentState_ == State::Active; }
    // 非アクティブ状態に遷移可能かどうか
    bool CanTransitionDisable(float playerX) const;
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

    //--------- variables ----------------------------------------------------

    // 現在の状態
    State currentState_;

    // 描画情報
    Sprite sprite_;
    uint32_t transparentTextureGH_; // 非アクティブ状態のテクスチャ
    uint32_t opaqueTextureGH_;      // アクティブ状態のテクスチャ

    // parameters
    float playerToDistance_; // プレイヤーとの判定を取るまで距離
    float offsetTranslateY_; // プレイヤーのY座標からのオフセット

    //--------- functions ----------------------------------------------------

    // helper
    // プレイヤーと一定距離近づいたか
    bool CheckPlayerToDistance(float playerX) const;
};