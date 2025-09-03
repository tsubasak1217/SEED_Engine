#pragma once

//============================================================================
//	include
//============================================================================
#include <SEED/Source/Basic/Object/GameObject2D.h>
#include <Game/Objects/Stage/BorderLine/BorderLine.h>

// c++
#include <list>
// front
class Player;

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

    //--------- structure ----------------------------------------------------

    // ステージ進行状態
    enum class State {

        Play,  // プレイ中...
        Clear, // クリア
        Retry  // リトライ
    };

    //--------- variables ----------------------------------------------------

    State currentState_;         // 現在の状態
    uint32_t currentStageIndex_; // 現在のステージ番号

    // jsonパス
    const std::string kJsonPath_ = "GameStage/stageParameter.json";
    
    // オブジェクトのリスト
    std::list<GameObject2D*> objects_;         // 通常
    std::list<GameObject2D*> hologramObjects_; // ホログラム
    // 境界線
    std::unique_ptr<BorderLine> borderLine_;

    // リストから貰って使用する
    Player* player_ = nullptr;

    // パラメータ
    float stageObjectMapTileSize_; // マップ一個分のサイズ

    //--------- functions ----------------------------------------------------

    // json
    void ApplyJson();
    void SaveJson();

    // create
    void BuildStage();

    // update
    void UpdateBorderLine();
    void PutBorderLine();
    void RemoveBorderLine();

    // helper
    void GetListsPlayerPtr();
};