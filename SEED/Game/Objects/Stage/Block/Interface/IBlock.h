#pragma once

//============================================================================
//	include
//============================================================================
#include <SEED/Lib/Structs/Sprite.h>
#include <SEED/Lib/Tensor/Vector2.h>
#include <Game/Objects/Stage/Block/Enum/BlockType.h>

// c++
#include <string>

//============================================================================
//	IBlock class
//============================================================================
class IBlock {
public:
    //========================================================================
    //	public Methods
    //========================================================================

    IBlock() = default;
    virtual ~IBlock() = default;

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
    void SetCommonState(BlockCommonState state) { commonState_ = state; }

    const Vector2& GetTranslate() const { return sprite_.translate; }
    BlockCommonState GetCommonState() const { return commonState_; }
protected:
    //========================================================================
    //	protected Methods
    //========================================================================

    //--------- variables ----------------------------------------------------

    Sprite sprite_;                // 描画情報
    BlockCommonState commonState_; // ブロックの状態
};