#include "SelectStageCreator.h"

//============================================================================
//	include
//============================================================================
#include <SEED/Lib/JsonAdapter/JsonAdapter.h>
#include <Game/GameSystem.h>
#include <Game/Objects/Stage/Enum/StageObjectType.h>
#include <Game/Objects/Select/Methods/SelectStageBuilder.h>

//============================================================================
//	SelectStageCreator classMethods
//============================================================================

nlohmann::json SelectStageCreator::GetJsonData() const {

    nlohmann::json data;
    if (!JsonAdapter::LoadCheck("SelectScene/selectStageDrawer.json", data)) {
        return nlohmann::json{};
    }
    return data;
}

nlohmann::json SelectStageCreator::GetProgressJsonData() const {
    
    nlohmann::json data;
    if (!JsonAdapter::LoadCheck("Progress/StageProgress.json", data)) {
        return nlohmann::json{};
    }
    return data;
}

void SelectStageCreator::BuildStage() {

    // 作成済みの場合は処理しない
    if (!stages_.empty()) {
        return;
    }

    // jsonデータを取得する
    nlohmann::json data = GetJsonData();
    nlohmann::json progressData = GetProgressJsonData();
    const Vector4 frameColor = data.value("frameColor_", Vector4(1));
    const Vector4 backgroundColor = data.value("backgroundColor_", Vector4(1));
    const Vector4 stageIndexBackColor = data.value("stageIndexBackColor_", Vector4(1));
    const Vector4 stageIndexTextColor = data.value("stageIndexTextColor_", Vector4(1));
    const float   stageIndexBackSize = data.value("stageIndexBackSize_", 128.0f);
    const float   stageIndexTextSize = data.value("stageIndexTextSize_", 128.0f);

    const bool    isSameFontStageIndex = data.value("isSameFontStageIndex_", false);
    const std::string stageIndexFontName =
        data.contains("stageNameText_.fontName") ? data["stageNameText_.fontName"].get<std::string>()
        : "Game/x10y12pxDonguriDuel.ttf";
    // ステージ名/難易度の初期化
    std::vector<std::string> stageNames;
    if (data.contains("stageNames") && data["stageNames"].is_array()) {
        for (auto& n : data["stageNames"]) if (n.is_string()) stageNames.push_back(n.get<std::string>());
    }
    std::vector<int32_t> stageDiffs;
    if (data.contains("stageDifficulties") && data["stageDifficulties"].is_array()) {
        for (auto& d : data["stageDifficulties"]) if (d.is_number_integer()) stageDiffs.push_back(d.get<int32_t>());
    }

    drawerParams_ = data;

    // ファイル名をすべて取得する
    auto csvFiles = SelectStageBuilder::CollectStageCSV("Resources/Stage");
    // ステージ数分メモリ確保
    stages_.reserve(csvFiles.size());

    for (uint32_t index = 0; index < csvFiles.size(); ++index) {

        // CSVファイルを読み込む
        auto grid = SelectStageBuilder::LoadCSV(csvFiles[index]);
        if (grid.empty()) {
            continue;
        }

        const int rows = static_cast<int>(grid.size());
        const int cols = static_cast<int>(grid.front().size());

        // このステージ描画情報
        SelectStageDrawer::Stage stage;
        stage.index = index;
        stage.rows = rows;
        stage.cols = cols;

        // フレーム
        stage.frame = Sprite("Scene_Select/selectStageRect.png");
        stage.frame.anchorPoint = 0.5f;
        stage.frame.isApplyViewMat = false;
        // 背景
        stage.background = Sprite("Scene_Select/bgCheckerboard.png");
        stage.background.anchorPoint = 0.5f;
        stage.background.uvTransform.scale = Vector2(80.0f, 45.0f);
        stage.background.isApplyViewMat = false;
        // ステージ番号背景
        stage.stageIndexBack = Sprite("Scene_Select/hexagonDesign.png");
        stage.stageIndexBack.anchorPoint = 0.5f;
        stage.stageIndexBack.isApplyViewMat = false;
        // ステージ番号
        stage.stageIndexText = TextBox2D(std::to_string(index + 1));
        stage.stageIndexText.anchorPos = Vector2(0.5f, 0.0f);
        stage.stageIndexText.glyphSpacing = 0.0f;
        stage.stageIndexText.size = 256.0f;
        stage.stageIndexText.textBoxVisible = false;
        stage.stageIndexText.isApplyViewMat = false;

        // ステージクリア済みUI
        stage.achievementUI = Sprite("Scene_Select/stageGoal.png");
        stage.achievementUI.anchorPoint = 0.5f;
        stage.achievementUI.isApplyViewMat = false;

        // 色
        stage.frame.color = frameColor;
        stage.background.color = backgroundColor;
        stage.stageIndexBack.color = stageIndexBackColor;
        stage.stageIndexText.color = stageIndexTextColor;

        // サイズ
        stage.stageIndexBack.size = stageIndexBackSize;
        stage.stageIndexText.fontSize = stageIndexTextSize;

        // フォント
        if (isSameFontStageIndex) {
            stage.stageIndexText.SetFont(stageIndexFontName);
        } else {
            stage.stageIndexText.SetFont("Game/x10y12pxDonguriDuel.ttf");
        }

        // ステージ名
        if (index < stageNames.size()) {

            stage.stageName = stageNames[index];
        } else {

            stage.stageName = "Stage " + std::to_string(static_cast<int>(index) + 1);
        }

        // 難易度
        stage.difficulty = (index < stageDiffs.size()) ? stageDiffs[index] : 1;

        // タイル
        uint32_t warpIndex = 0;
        for (int r = 0; r < rows; ++r) {
            const int colsThis = static_cast<int>(grid[r].size());
            for (int c = 0; c < colsThis; ++c) {

                // CSVのインデックスからスプライトを作成する
                const int id = grid[r][c];
                // Noneは処理しない
                if (static_cast<StageObjectType>(id) == StageObjectType::None) {
                    continue;
                }

                // ステージ位置を記録
                stage.objectIds.push_back(id);
                stage.objectUVs.push_back(Vector2((c + 0.5f) / static_cast<float>(cols),
                    (r + 0.5f) / static_cast<float>(rows)));
                stage.objects.emplace_back(CreateTileSprite(id, Vector2(0.0f), Vector2(1.0f), warpIndex));

                // インデックスを進める
                if (static_cast<StageObjectType>(id) == StageObjectType::Warp) {
                    ++warpIndex;
                }
            }
        }

        // クリア済みかどうか
        if (GameSystem::GetInstance()->GetStageProgressCollector()) {
            stage.isClear = GameSystem::GetInstance()->GetStageProgressCollector()->IsStageClear(index);
        } else {
            stage.isClear = false;
        }
        stages_.push_back(std::move(stage));
    }
}

Sprite SelectStageCreator::CreateTileSprite(uint32_t index,
    const Vector2& translate, const Vector2& size, uint32_t warpIndex) {

    // スプライトをパスから作成
    Sprite sprite = Sprite(GetFileNameFromIndex(index, warpIndex));
    // 設定
    sprite.anchorPoint = 0.5f;
    sprite.transform.translate = translate;
    sprite.size = size;
    sprite.isApplyViewMat = false;

    // 個別のサイズ設定
    if (static_cast<StageObjectType>(index) == StageObjectType::Player) {

        sprite.size *= 0.8f;
    }
    return sprite;
}

std::string SelectStageCreator::GetFileNameFromIndex(uint32_t index, uint32_t warpIndex) const {

    // 番号からスプライト画像のパスを取得する
    switch (index) {
    case 1:  return "Scene_Select/stageBlockNormal.png";   // NormalBlock
    case 2:  return "Scene_Select/stageGoal.png";          // Goal
    case 3:  return "Scene_Select/stagePlayer.png";        // Player
    case 5:  return "Scene_Select/stageEmptyBlock.png";    // EmptyBlock
    case 6:  return "Scene_Select/stageLaserLauncher.png"; // LaserLauncher
    case 4: {

        // Warpはインデックスに応じて使用する画像を変更する
        std::string path = "Scene_Select/stageWarp" + std::to_string(warpIndex) + ".png";
        return path;
    }
    }
    return "";
}