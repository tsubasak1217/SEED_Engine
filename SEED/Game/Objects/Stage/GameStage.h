#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Objects/Stage/Block/Interface/IBlock.h>

// c++
#include <list>

//============================================================================
//	GameStage class
//============================================================================
class GameStage {
public:
    //========================================================================
    //	public Methods
    //========================================================================

    GameStage() = default;
    ~GameStage() = default;

    // 初期化処理
    void Initialize();

    // 更新処理
    void Update();

    // 描画処理
    void Draw();

    // エディター
    void Edit();

    //--------- accessor -----------------------------------------------------

private:
    //========================================================================
    //	private Methods
    //========================================================================

    //--------- variables ----------------------------------------------------

    // jsonパス
    const std::string kJsonPath_ = "GameStage/stageParameter.json";

    // ブロックのリスト
    std::list<std::unique_ptr<IBlock>> blocks_;

    // エディター用


    //--------- functions ----------------------------------------------------

    // json
    void ApplyJson();
    void SaveJson();

    // init
    void InitializeBlock(BlockType blockType, uint32_t index);

    // helper
    std::string GetBlockTextureName(BlockType blockType) const;
};