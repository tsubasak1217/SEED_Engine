#pragma once

//============================================================================
//	include
//============================================================================
#include <SEED/Source/Basic/Object/GameObject2D.h>
#include <Game/Objects/Stage/BorderLine/BorderLine.h>
#include <Game/Objects/Stage/Methods/GameStageWarpController.h>
#include <Game/Objects/Stage/Methods/GameStageLaserController.h>
#include <Game/Objects/Stage/Methods/StageCameraAdjuster.h>
#include <Game/Objects/Stage/Methods/StageBackDrawer.h>
#include <SEED/Lib/Structs/TextBox.h>

// c++
#include <list>
#include <deque>
// front
class Player;

//============================================================================
//	GameStage class
//============================================================================
class GameStage {
    friend class GameState_Play;// 期限が近いのでいったんこれで
    friend class GameState_Pause;
    friend class GameState_Clear;
public:
    //========================================================================
    //	public Methods
    //========================================================================

    GameStage() = default;
    ~GameStage() = default;

    // 初期化処理
    void Initialize(int currentStageIndex);

    // 更新処理
    void Update(bool isUpdateBorderLine = true);

    // 描画処理
    void Draw();

    // エディター
    void Edit();

    // ホログラムオブジェクトの削除
    void RemoveBorderLine();

    // 次のステージをリクエスト
    void RequestNextStage();

    //--------- structure ----------------------------------------------------

    // ステージ進行状態
    enum class State {

        Play,   // プレイ中...
        Clear,  // クリア
        Dead,   // プレイヤーがやられた
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
    bool IsClear() const { return isClear_; }
    bool IsFinishedAdjust() const { return cameraAdjuster_.IsFinishedAdjust(); }
    void SetIsClear(bool isClear) { isClear_ = isClear; }

    // オブジェクトリスト関連
    static void AddDisActiveObject(GameObject2D* object) { disActiveObjects_.push_back(object); }
    static void ReActivateDisActiveObjects();

    // StageObjectComponentからプレイヤーがNormalBlockを踏んだ時の座標を受け取る
    void RecordPlayerOnBlock(const Vector2& translate);

    // 何か入力があるかチェック
    bool IsTriggredAnyDevice() const;
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
    bool isPlayerDead_ = false; // プレイヤーが死んだかどうか

    // jsonパス
    const std::string kJsonPath_ = "GameStage/stageParameter.json";

    // オブジェクトのリスト
    std::list<GameObject2D*> objects_;         // 通常
    std::list<GameObject2D*> hologramObjects_; // ホログラム

    // 空白ブロックに重なって一時的に非アクティブになっているオブジェクトのリスト
    static inline std::list<GameObject2D*> disActiveObjects_{};

    // 境界線
    std::unique_ptr<BorderLine> borderLine_;
    // 背景描画
    StageBackDrawer backDrawer_;
    // リストから貰って使用する
    Player* player_ = nullptr;

    // メソッド
    std::unique_ptr<GameStageWarpController> warpController_;   // ワープ管理
    std::unique_ptr<GameStageLaserController> laserController_; // レーザー管理

    // パラメータ
    float stageObjectMapTileSize_; // マップ一個分のサイズ
    float playerSize_;

    // 現在のステージの範囲を格納
    std::optional<Range2D> currentStageRange_;
    StageCameraAdjuster cameraAdjuster_; // カメラ調整

    // プレイヤーが踏んでいたNormalBlockの座標s
    int maxRecordCount_ = 1;
    std::deque<Vector2> onPlayerNormalBlocks_;

    // timer
    Timer removeUITimer_ = Timer(0.25f);
    Sprite removeUI_;
    TextBox2D textBox;

    //--------- functions ----------------------------------------------------

    // json
    void ApplyJson();
    void SaveJson();

    // create
    void BuildStage();

    // update
    /// Play
    void UpdatePlay(bool isUpdateBorderLine = true);
    void UpdateWarp();
    void UpdateLaserLauncher();
    void UpdateBorderLine();
    /// Clear
    void UpdateClear();
    /// Death
    void UpdateDead();
    /// Retry
    void UpdateRetry();
    /// Select
    void UpdateReturnSelect();

    // helper
    void GetListsPlayerPtr();
    void SetListsWarpPtr(StageObjectCommonState state);
    void SetListsLaserLaunchersPtr(StageObjectCommonState state);
    void PutBorderLine();
    void CheckClear();
    //死亡判定
    void CheckPlayerDead();
    //プレイヤーが境界線を越えたかどうかの判定
    void CheckPlayerCrossedBorderLine();
    //カメラ範囲から出たか判定
    void CheckPlayerOutOfCamera();

    // ステージ範囲計算
    void CalculateCurrentStageRange();
    void CloseToPlayer(LR direction, float zoomRate = 2.4f, const Vector2& focus = 0.7f);

    // プレイヤーが踏んでいる最新のブロック位置を記録する
    void RecordPlayerOnBlock();

    // 取り除くUI
    void UpdateRemoveUI();
};