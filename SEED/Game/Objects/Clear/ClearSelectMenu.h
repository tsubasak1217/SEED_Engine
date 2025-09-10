#pragma once

//============================================================================
//	include
//============================================================================
#include <SEED/Lib/Structs/Sprite.h>
#include <SEED/Lib/Structs/TextBox.h>
#include <SEED/Lib/Structs/Timer.h>
#include <SEED/Lib/Input/InputMapper.h>
#include <Game/Scene/Input/PauseMenuInputActionEnum.h>

// json
#include <json.hpp>

//============================================================================
//	ClearSelectMenu structure
//============================================================================

// 入力結果
struct SelectMenuResult {

    bool isNextStage;  // 次のステージ
    bool returnSelect; // セレクトに戻る
};

//============================================================================
//	ClearSelectMenu class
//============================================================================
class ClearSelectMenu {
public:
    //========================================================================
    //	public Methods
    //========================================================================

    ClearSelectMenu() = default;
    ~ClearSelectMenu() = default;

    // 初期化処理
    void Initialize(uint32_t currentStageIndex,bool isLastStage);

    // 更新処理
    void Update();

    // 描画処理
    void Draw();

    // エディター
    void SelectEdit();

    //--------- accessor -----------------------------------------------------

    const SelectMenuResult& GetResult() const { return result_; }
private:
    //========================================================================
    //	private Methods
    //========================================================================

    //--------- structure ----------------------------------------------------

    // メニューアイテム
    struct Item {

        //--------- variables ------------------------------------------------

        Transform2D start; // 開始地点
        Transform2D end;   // 目標地点

        Sprite backSprite; // 背景
        TextBox2D text;    // 入力文字

        Vector2 backSpriteSize; // 背景サイズ
        Vector4 backColor;      // 背景色

        Timer selectTime = Timer(0.2f);
        static inline float kSelectTimeMax = 0.2f;

        //--------- functions ------------------------------------------------

        // json
        nlohmann::json ToJson() const;
        void FromJson(const nlohmann::json& data);

        // エディター
        void Edit();
    };

    //--------- variables ----------------------------------------------------

    // アイテム数
    uint32_t kItemCount_; // メニュー最大数
    uint32_t currentMenu_ = 0;      // 現在選択しているメニュー
    SelectMenuResult result_;       // 入力結果

    // 入力処理
    std::unique_ptr<InputMapper<PauseMenuInputAction>> inputMapper_;
    bool isExit_ = false;
    bool isLastStage_ = false;

    // アイテムs
    std::vector<Item> items_;
    // メニュー移動時間
    Timer menuTimer_ = Timer(0.8f);

    // ステージ番号を表示する
    Sprite stageIndexBack_;       // ステージ番号背景
    TextBox2D stageIndexText_;    // ステージ番号
    Vector2 stageIndexBackTranslate_; // 座標
    Vector2 stageIndexTextTranslate_; // 座標

    //--------- functions ----------------------------------------------------

    // helper
    void MenuItemsToJson();
};