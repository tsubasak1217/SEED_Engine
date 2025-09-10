#pragma once

//============================================================================
//	include
//============================================================================
#include <SEED/Lib/Structs/Sprite.h>
#include <SEED/Lib/Structs/Timer.h>
#include <SEED/Lib/Structs/TextBox.h>
#include <SEED/Lib/Input/InputMapper.h>
#include <Game/Objects/Select/Input/SelectInputEnum.h>

// c++
#include <vector>

//============================================================================
//	SelectStageDrawer structure
//============================================================================

// 選択ステージ情報
struct SelectStageInfo {

    bool isNextScene;     // 次のシーンに進めるかどうか
    uint32_t decideStage; // 決定したステージ番号
};

//============================================================================
//	SelectStageDrawer class
//============================================================================
class SelectStageDrawer {
public:
    //========================================================================
    //	public Methods
    //========================================================================

    //--------- structure -----------------------------------------------------

    // ステージ描画
    struct Stage {

        uint32_t index;              // ステージ番号
        std::vector<Sprite> objects; // オブジェクト番号に対応するスプライト
        Sprite frame;                // ステージを囲うフレーム
        Sprite background;           // ステージとフレームの背景
        Sprite achievementUI;        // クリア済みかどうかの表示
        TextBox2D stageIndexText;    // ステージ番号
        Sprite stageIndexBack;       // ステージ番号背景
        std::string stageName;       // ステージの名前
        int32_t difficulty;          // 難易度の星の数

        // 全体の表示
        Vector2 translate; // 中心座標
        Vector2 size;      // サイズ

        // スプライトの位置情報
        int rows = 0;
        int cols = 0;
        std::vector<uint32_t> objectIds; // 各スプライトのid
        std::vector<Vector2> objectUVs;  // フレーム内の位置

        bool isClear = false; // クリア済みかどうか
    };
public:
    //========================================================================
    //	public Methods
    //========================================================================

    SelectStageDrawer() = default;
    ~SelectStageDrawer() = default;

    // 初期化
    void Initialize(uint32_t firstFocusStage, const std::vector<Stage>& stageData);

    // 更新処理
    void Update();

    // 描画処理
    void Draw();

    // エディター
    void Edit();

    //--------- accessor -----------------------------------------------------

    // フォーカスするステージを設定
    void SetNextFocus();
    void SetPrevFocus();
    // ステージの決定入力
    void SetEndFocus();

    // 決定入力が確定したかどうか
    bool IsDecideStage() const { return currentState_ == State::End; }
    // 選択ステージ情報
    const SelectStageInfo& GetStageInfo() const { return stageInfo_; }
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

    // End時のフェーズ
    enum class EndPhase {

        Decide, // 決定
        Zoom    // ズーム
    };

    //--------- variables ----------------------------------------------------

    // 現在の状態
    State currentState_;
    EndPhase endPhase_;
    SelectStageInfo stageInfo_;
    // 最大ステージ数
    uint32_t maxStageCount_;

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
    // 色
    Vector4 frameColor_;          // フレームの色
    Vector4 backgroundColor_;     // 背景の色
    Vector4 stageIndexBackColor_; // ステージ番号背景の色
    Vector4 stageIndexTextColor_; // ステージ番号の色
    Vector4 stageNameTextColor_;  // ステージ名前の色
    // 座標
    Vector2 centerTranslate_;      // 真ん中のステージ表示(フォーカス位置)
    Vector2 leftTranslate_;        // 左のステージ表示位置
    Vector2 rightTranslate_;       // 右のステージ表示位置
    //オフセット
    float stageIndexBackOffsetY_;  // ステージ番号インデックス背景のオフセットY
    float stageIndexTextOffsetY_;  // ステージ番号インデックスのオフセットY
    float stageAchievementUIOffsetY_; // ステージクリア済みUIのオフセットY
    Vector2 stageNameTextTranslate_; // ステージ名前インデックスのオフセットY
    // サイズ
    Vector2 focusSize_; // フォーカス時のサイズ
    Vector2 outSize_;   // フォーカスされていないときのサイズ
    float tileScale_;   // フレーム内のブロックサイズのスケーリング
    float stageIndexBackSize_; // ステージ番号背景サイズ
    float stageIndexTextSize_; // ステージ番号サイズ
    float stageNameTextSize_;  // ステージ名前サイズ
    float stageAchievementUISize_; // ステージクリア済みUIサイズ

    // 中央フレームアニメーション
    Sprite focusAnimFrame_; // 中央だけに被せるフレーム
    Timer focusAnimTimer_;  // ループ用タイマー
    Easing::Type focusAnimEasing_;
    float focusAnimFrom_;   // 開始スケール
    float focusAnimTo_;     // 目標スケール
    bool focusAnimForward_; // 補間先の設定
    // ステージ番号背景アニメーション
    Sprite stageIndexBackAnim_;

    // 決定後のズームアニメーション
    Timer endZoomTimer_;         // ズーム時間
    Easing::Type endZoomEasing_;
    float focusAnimBaseDuration_; // フォーカス速度の保存用
    float endZoomToScale_;        // 最後のズームスケール倍率

    // 下の矢印
    Sprite leftArrow_;  // 左
    Sprite rightArrow_; // 右
    Timer arrowAnimTimer_;  // 矢印アニメーション時間
    float arrowSpacing_;    // 矢印の間の間隔
    float arrowAmplitude_;  // 振幅
    float arrowTranslateY_; // 矢印のY座標
    Easing::Type arrowEasing_;
    Timer leftArrowReactTimer_;   // 左リアクション
    Timer rightArrowReactTimer_;  // 右リアクション
    float reactRate_;
    bool leftArrowReacting_ = false;
    bool rightArrowReacting_ = false;
    // ステージの名前表示
    TextBox2D stageNameText_;
    // ステージの名前s
    std::vector<std::string> stageNames_;
    bool isSameFontStageIndex_;
    // ステージの難易度s
    std::vector<int32_t> stageDifficulties_;
    std::array<Sprite,5> difficultyStars_; // 難易度の星
    Vector2 difficultyStarBasePos_;
    float starDrawRangeX_ = 300.0f;

    //--------- functions ----------------------------------------------------

    // json
    void ApplyJson();
    void SaveJson();

    // init
    void SetStageParam();

    // update
    void UpdateFocusAnim();
    void UpdateArrow();

    // draw
    void DrawFocusAnim();
    void DrawEndZoom();

    // helper
    void StartMoveToNext(uint32_t next);
    void ApplyPoseToStage(Stage& stage, const Vector2& center, const Vector2& size);
    void PoseFromOffset(float offset, Vector2& outPos, Vector2& outSize);
    void DrawActivate(float f);
    void TriggerLeftArrowReact();
    void TriggerRightArrowReact();
    void UpdateDifficultyStar();
};