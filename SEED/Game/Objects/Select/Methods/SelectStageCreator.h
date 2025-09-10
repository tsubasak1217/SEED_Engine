#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Objects/Select/Drawer/SelectStageDrawer.h>

//============================================================================
//	SelectStageCreator class
//============================================================================
class SelectStageCreator {
public:
    //========================================================================
    //	public Methods
    //========================================================================

    SelectStageCreator() = default;
    ~SelectStageCreator() = default;

    // ステージの構築
    void BuildStage();

    //--------- accessor -----------------------------------------------------

    const std::vector<SelectStageDrawer::Stage>& GetStages() const { return stages_; }
private:
    //========================================================================
    //	private Methods
    //========================================================================

    //--------- structure ----------------------------------------------------

    // 初期化値
    struct Default {

        // 色
        Vector4 frameColor = 1.0f;
        Vector4 backgroundColor = 1.0f;
        Vector4 stageIndexBackColor = 1.0f;
        Vector4 stageIndexTextColor = 1.0f;

        // サイズ
        float stageIndexBackSize = 128.0f;
        float stageIndexTextSize = 128.0f;
        float stageAchievementUISize = 64.0f;

        // フォント情報
        bool isSameFontStageIndex = false;
        std::string stageIndexFontName = "Game/x10y12pxDonguriDuel.ttf";
    };

    //--------- variables ----------------------------------------------------

    // 作成するステージの構造体
    std::vector<SelectStageDrawer::Stage> stages_;

    //--------- functions ----------------------------------------------------

    // helper
    Sprite CreateTileSprite(uint32_t index, const Vector2& translate,
        const Vector2& size, uint32_t warpIndex);
    std::string GetFileNameFromIndex(uint32_t index, uint32_t warpIndex) const;
};