#pragma once

//============================================================================
//	include
//============================================================================
#include <SEED/Source/Basic/Object/GameObject2D.h>
#include <Game/Objects/Stage/BorderLine/BorderLine.h>
#include <Game/Objects/Stage/Methods/GameStageWarpController.h>

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
    void Initialize(int currentStageIndex);

    // 更新処理
    void Update();

    // 描画処理
    void Draw();

    // エディター
    void Edit();

    // ホログラムオブジェクトの削除
    void RemoveBorderLine();

    //--------- structure ----------------------------------------------------

    // ステージ進行状態
    enum class State {

        Play,   // プレイ中...
        Clear,  // クリア
        Death,  // プレイヤーがやられた
        Retry,  // リトライ
        Select, // セレクト画面に戻る
    };

    //--------- accessor -----------------------------------------------------

    // 現在の状態を取得する
    State GetCurrentState() const { return currentState_; }

    // 現在のステージ番号を取得する
    uint32_t GetCurrentStageIndex() const { return currentStageIndex_; }

    // 最大ステージ数を取得する
    uint32_t GetMaxStageCount() const { return maxStageCount_; }

    // アクティブ状態を設定する
    void SetIsActive(bool isActive);

    // クリアしたかどうか
    bool IsClear() const{ return isClear_; }
    void SetIsClear(bool isClear){ isClear_ = isClear; }

    // オブジェクトリスト関連
    static void AddDisActiveObject(GameObject2D* object){disActiveObjects_.push_back(object);}
    static void ReActivateDisActiveObjects();

private:
    //========================================================================
    //	private Methods
    //========================================================================

    //--------- variables ----------------------------------------------------

    State currentState_;         // 現在の状態
    uint32_t currentStageIndex_; // 現在のステージ番号
    uint32_t maxStageCount_;     // 最大ステージ数
    bool isRemoveHologram_;      // ホログラムオブジェクトの削除を行うか
    bool isClear_ = false;       // クリアしたかどうか

    // jsonパス
    const std::string kJsonPath_ = "GameStage/stageParameter.json";
    
    // オブジェクトのリスト
    std::list<GameObject2D*> objects_;         // 通常
    std::list<GameObject2D*> hologramObjects_; // ホログラム

    // 空白ブロックに重なって一時的に非アクティブになっているオブジェクトのリスト
    static inline std::list<GameObject2D*> disActiveObjects_{};

    // 境界線
    std::unique_ptr<BorderLine> borderLine_;
    // リストから貰って使用する
    Player* player_ = nullptr;

    // メソッド
    std::unique_ptr<GameStageWarpController> warpController_; // ワープ管理

    // パラメータ
    float stageObjectMapTileSize_; // マップ一個分のサイズ
    float playerSize_;

    //--------- functions ----------------------------------------------------

    // json
    void ApplyJson();
    void SaveJson();

    // create
    void BuildStage();

    // update
    /// Play
    void UpdatePlay();
    void UpdateWarp();
    void UpdateBorderLine();
    /// Clear
    void UpdateClear();
    /// Death
    void UpdateDeath();
    /// Retry
    void UpdateRetry();
    /// Select
    void UpdateReturnSelect();

    // helper
    void GetListsPlayerPtr();
    void SetListsWarpPtr(StageObjectCommonState state);
    void PutBorderLine();
    void CheckClear();
};