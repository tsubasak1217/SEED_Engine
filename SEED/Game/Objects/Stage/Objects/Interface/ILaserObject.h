#pragma once

//============================================================================
//	include
//============================================================================
#include <SEED/Lib/enums/Direction.h>
#include <SEED/Lib/Structs/Sprite.h>
#include <SEED/Lib/Tensor/Vector2.h>
#include <Game/Objects/Stage/Enum/StageObjectType.h>
#include <SEED/Source/Basic/Object/GameObject2D.h>

// c++
#include <string>

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
    virtual void Initialize(const std::string& filename) = 0;

    // 更新処理
    virtual void Update() = 0;

    // 描画処理
    virtual void Draw() = 0;

    // エディター
    virtual void Edit() = 0;

    //--------- accessor -----------------------------------------------------

    void SetTranslate(const Vector2& translate) { sprite_.translate = translate; }
    void SetSize(const Vector2& size) { sprite_.size = size; }

    void SetCommonState(StageObjectCommonState state) { commonState_ = state; }
    virtual void SetDirection(DIRECTION4 direction) = 0;

    const Vector2& GetTranslate() const { return sprite_.translate; }
    StageObjectCommonState GetCommonState() const { return commonState_; }

    GameObject2D* GetOwner() const { return owner_; }
protected:
    //========================================================================
    //	protected Methods
    //========================================================================

    //--------- variables ----------------------------------------------------

    Sprite sprite_;                      // 描画情報
    StageObjectCommonState commonState_; // オブジェクトの状態
    DIRECTION4 direction_;               // レーザーの向き
    GameObject2D* owner_ = nullptr;      // 所有者
};