#pragma once

//============================================================================
//	include
//============================================================================
#include <SEED/Lib/Structs/Sprite.h>
#include <SEED/Lib/Structs/Timer.h>
#include <SEED/Lib/Input/InputMapper.h>
#include <Game/Objects/Select/Input/SelectInputEnum.h>

// c++
#include <vector>

//============================================================================
//	SelectStageDrawer class
//============================================================================
class SelectStageDrawer {
public:
    //========================================================================
    //	public Methods
    //========================================================================

    SelectStageDrawer() = default;
    ~SelectStageDrawer() = default;

    // 初期化
    void Initialize(uint32_t firstFocusStage);

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

    // 現在の状態
    enum class State {

        Start,  // 開始アニメーション
        Select, // 選択画面(この時にのみステージを選択できる)
        Move,   // 移動中
        End,    // 選択アニメーション
    };

    // ステージ描画
    struct Stage {

        uint32_t index;              // ステージ番号
        std::vector<Sprite> objects; // オブジェクト番号に対応するスプライト
        Sprite frame;                // ステージを囲うフレーム

        // 全体の表示
        Vector2 translate; // 中心座標
        Vector2 size;      // サイズ

        // スプライトの位置情報
        int rows = 0;
        int cols = 0;
        std::vector<uint32_t> objectIds; // 各スプライトのid
        std::vector<Vector2> objectUVs;  // フレーム内の位置
    };

    //--------- variables ----------------------------------------------------

    // 現在の状態
    State currentState_;

    // ステージを描画する用の配列
    std::vector<Stage> stages_;
    // ステージ番号表示
    Sprite indexSprite;

    // パラメータ
    Timer moveTimer_;     // 移動にかかる時間
    Easing::Type moveEasing_;
    uint32_t focusIndex_; // 目標フォーカスインデックス
    float animFrom_;      // 補間開始の連続フォーカス
    float animTo_;        // 補間終了の連続フォーカス
    // 座標
    Vector2 centerTranslate_; // 真ん中のステージ表示(フォーカス位置)
    Vector2 leftTranslate_;   // 左のステージ表示位置
    Vector2 rightTranslate_;  // 右のステージ表示位置
    // サイズ
    Vector2 focusSize_; // フォーカス時のサイズ
    Vector2 outSize_;   // フォーカスされていないときのサイズ

    //--------- functions ----------------------------------------------------

    // json
    void ApplyJson();
    void SaveJson();

    // init
    void BuildAllStage();
    Sprite CreateTileSprite(uint32_t index, const Vector2& translate, const Vector2& size);

    // helper
    void StartMoveToNext(uint32_t next);
    void ApplyPoseToStage(Stage& stage, const Vector2& center, const Vector2& size);
    void PoseFromOffset(float offset, Vector2& outPos, Vector2& outSize);
    std::string GetFileNameFromIndex(uint32_t index) const;
};