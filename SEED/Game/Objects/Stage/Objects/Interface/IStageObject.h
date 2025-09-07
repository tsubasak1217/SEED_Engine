#pragma once

//============================================================================
//	include
//============================================================================
#include <SEED/Lib/Structs/Sprite.h>
#include <SEED/Lib/Tensor/Vector2.h>
#include <Game/Objects/Stage/Enum/StageObjectType.h>
#include <SEED/Source/Basic/Object/GameObject2D.h>

// c++
#include <string>

//============================================================================
//	IStageObject class
//============================================================================
class IStageObject {
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

    // 描画処理
    virtual void Draw() = 0;

    // エディター
    virtual void Edit() = 0;

    //--------- accessor -----------------------------------------------------

    virtual void SetTranslate(const Vector2& translate) { sprite_.translate = translate; }
    virtual void SetSize(const Vector2& size) { sprite_.size = size; }
    void SetScale(float scale) { sprite_.scale = scale; }
    virtual void SetCommonState(StageObjectCommonState state) { commonState_ = state; }

    virtual const Vector2& GetTranslate() const { return sprite_.translate; }
    StageObjectCommonState GetCommonState() const { return commonState_; }

    GameObject2D* GetOwner() const { return owner_; }

    // Collision
    virtual void OnCollisionEnter([[maybe_unused]] GameObject2D* other) {}
    virtual void OnCollisionStay([[maybe_unused]] GameObject2D* other) {}
    virtual void OnCollisionExit([[maybe_unused]] GameObject2D* other) {}


protected:
    //========================================================================
    //	protected Methods
    //========================================================================

    //--------- variables ----------------------------------------------------

    Sprite sprite_;                      // 描画情報
    StageObjectCommonState commonState_; // オブジェクトの状態
    GameObject2D* owner_ = nullptr;            // 所有者

    // 色
    static inline const Vector4 normalColor_ = MyMath::FloatColor(255, 198, 57, 255);
    static inline const Vector4 hologramColor_ = MyMath::FloatColor(255, 43, 245, 255);
    // サイズ
    static inline const Vector2 defaultTileSize_ = Vector2(46.0f, 46.0f);
};