#pragma once

//============================================================================
//	include
//============================================================================
#include <SEED/Lib/Structs/Sprite.h>
#include <SEED/Lib/Structs/Timer.h>
#include <SEED/Lib/Tensor/Vector2.h>
#include <Game/Objects/Stage/Enum/StageObjectType.h>
#include <SEED/Source/Basic/Object/GameObject2D.h>

// c++
#include <string>

//============================================================================
//	IStageObject class
//============================================================================
class IStageObject {
    friend class StageObjectComponent;
    friend class GameStageBuilder;
public:
    //========================================================================
    //	public Methods
    //========================================================================

    IStageObject(GameObject2D* owner) : owner_(owner) {}
    IStageObject() = default;
    virtual ~IStageObject() = default;

    // 初期化処理
    virtual void Initialize() = 0;

    // 更新処理
    virtual void Update() = 0;
    // ホログラム発生時のアニメーション
    virtual void AppearanceUpdateAnimation(float baseDuration, float spacing, Easing::Type easing, uint32_t colum);
    virtual void MasterScaleUpdate(float timeScale = 1.0f);

    // 描画処理
    virtual void Draw() = 0;

    // エディター
    virtual void Edit() = 0;

    //--------- accessor -----------------------------------------------------

    virtual void SetTranslate(const Vector2& translate) { sprite_.transform.translate = translate; }
    virtual void SetSize(const Vector2& size) { sprite_.size = size; }
    virtual void SetScale(float scale) { sprite_.transform.scale = scale; }
    virtual void SetCommonState(StageObjectCommonState state) { commonState_ = state; }
    void SetStopAppearance(bool isSetStopAppearance) { isSetStopAppearance_ = isSetStopAppearance; }

    virtual const Vector2& GetTranslate() const { return sprite_.transform.translate; }
    StageObjectCommonState GetCommonState() const { return commonState_; }

    GameObject2D* GetOwner() const { return owner_; }
    const Timer& GetMasterScaleTimer() const { return masterScaleTimer_; }
    bool IsStopAppearance() const { return isSetStopAppearance_; }

    // Collision
    virtual void OnCollisionEnter([[maybe_unused]] GameObject2D* other) {}
    virtual void OnCollisionStay([[maybe_unused]] GameObject2D* other) {}
    virtual void OnCollisionExit([[maybe_unused]] GameObject2D* other) {}
protected:
    //========================================================================
    //	protected Methods
    //========================================================================

    //--------- variables ----------------------------------------------------

    Sprite sprite_;                       // 描画情報
    StageObjectCommonState commonState_;  // オブジェクトの状態
    GameObject2D* owner_ = nullptr;       // 所有者
    Timer appearanceTimer_;               // 出現アニメーション
    Timer appearanceWaitTimer_;           // 出現待機時間
    float commonScale_;                   // 共通スケーリング
    Timer masterScaleTimer_ = Timer(0.5f, 0.5f);// マスター倍率用タイマー
    bool masterScaleOrder_ = true; // マスター倍率更新中かどうか
    bool isSetStopAppearance_ = false;

    // 色
    static inline const Vector4 normalColor_ = MyMath::FloatColor(255, 198, 57, 255);
    static inline const Vector4 hologramColor_ = MyMath::FloatColor(255, 43, 245, 255);
    // サイズ
    static inline const Vector2 defaultTileSize_ = Vector2(46.0f, 46.0f);
};