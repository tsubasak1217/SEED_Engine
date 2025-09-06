#pragma once

//============================================================================
//	include
//============================================================================
#include <SEED/Lib/enums/Direction.h>
#include <SEED/Lib/Structs/Sprite.h>
#include <SEED/Lib/Structs/Timer.h>
#include <SEED/Lib/Tensor/Vector2.h>
#include <Game/Objects/Stage/Enum/StageObjectType.h>
#include <SEED/Source/Basic/Object/GameObject2D.h>

// c++
#include <string>

//============================================================================
//	Laser structure
//============================================================================

// ワープするレーザー処理
struct WarpLaserParam {

    bool isHit;                             // ワープと衝突したかどうか
    uint32_t warpIndex;                     // 衝突したワープのインデックス
    StageObjectCommonState warpCommonState; // 通常かホログラムか
};

//============================================================================
//	ILaserObject class
//============================================================================
class ILaserObject {
public:
    //========================================================================
    //	public Methods
    //========================================================================

    ILaserObject(GameObject2D* owner) : owner_(owner) {}
    ILaserObject() = default;
    virtual ~ILaserObject() = default;

    // 初期化処理
    virtual void Initialize() = 0;

    // 更新処理
    virtual void Update() = 0;

    // 描画処理
    virtual void Draw() = 0;

    // エディター
    virtual void Edit() = 0;

    //--------- accessor -----------------------------------------------------

    void SetTranslate(const Vector2& translate) { sprite_.translate = translate; }
    void SetSize(const Vector2& size) { initSizeY_ = size.y; sprite_.size = size; }

    // 状態の設定
    // 伸び始める
    virtual void ReExtend() = 0;
    // 伸びるのを終了
    virtual void StopExtend() = 0;
    void SetCommonState(StageObjectCommonState state) { commonState_ = state; }
    virtual void SetDirection(DIRECTION4 direction) = 0;
    // ヒット先のワープの情報を設定する
    virtual void SetHitWarpParam(const WarpLaserParam& param) = 0;
    void SetFamilyId(uint64_t id) { familyId_ = id; }

    const Vector2& GetTranslate() const { return sprite_.translate; }
    const Vector2& GetSize() const { return sprite_.size; }
    const Vector2& GetAnchorPoint() const { return sprite_.anchorPoint; }
    StageObjectCommonState GetCommonState() const { return commonState_; }
    DIRECTION4 GetDirection() const { return direction_; }

    // ヒット先のワープの情報を取得する
    const WarpLaserParam& GetWarpParam() const { return warpParam_; }
    uint64_t GetFamilyId() const { return familyId_; }

    GameObject2D* GetOwner() const { return owner_; }
protected:
    //========================================================================
    //	protected Methods
    //========================================================================

    //--------- structure ----------------------------------------------------

    // 現在の状態
    enum class State {

        Extend, // 伸びてる最中
        Stop,   // 止まった
    };

    //--------- variables ----------------------------------------------------

    Sprite sprite_;                      // 描画情報
    State currentState_;                 // 現在の状態
    StageObjectCommonState commonState_; // オブジェクトの共通状態
    DIRECTION4 direction_;               // レーザーの向き
    GameObject2D* owner_ = nullptr;      // 所有者

    // パラメータ
    float initSizeY_;       // 初期化時のサイズY
    float sizeExtendSpeed_; // レーザーが伸びる速度
    // ワープするレーザー処理に使用する
    WarpLaserParam warpParam_;
    //  発射系統を識別するID
    uint64_t familyId_ = 0;
};