#pragma once

//============================================================================
//	include
//============================================================================
#include <SEED/Source/Basic/Object/GameObject.h>
#include <Game/Objects/Player/Entity/Player.h>
#include <Game/Objects/Stage/BorderLine/BorderLine.h>

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
    
    // プレイヤー
    std::unique_ptr<Player> player_;
    // ブロックのリスト
    std::list<GameObject*> blocks_;
    // 境界線
    std::unique_ptr<BorderLine> borderLine_;

    // エディター用


    //--------- functions ----------------------------------------------------

    // json
    void ApplyJson();
    void SaveJson();

    // init
    void InitializeBlock(BlockType blockType, uint32_t index);
};