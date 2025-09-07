#include "SelectStageDrawer.h"

//============================================================================
//	include
//============================================================================
#include <Game/Objects/Stage/Enum/StageObjectType.h> 
#include <Game/Objects/Select/Methods/SelectStageBuilder.h>

// imgui
#include <SEED/Source/Manager/ImGuiManager/ImGuiManager.h>

//============================================================================
//	SelectStageDrawer classMethods
//============================================================================

void SelectStageDrawer::Initialize(uint32_t firstFocusStage) {

    firstFocusStage;

    centerTranslate_ = { 640.0f, 360.0f };
    leftTranslate_ = { 120.0f, 360.0f }; // 0番の左基準
    rightTranslate_ = { 0.0f,   0.0f }; // 使わない
    focusSize_ = { 300.0f, 300.0f };
    outSize_ = { 320.0f, 180.0f };

    // すべてのステージを構築する
    BuildAllStage();

    // 初期化値設定
    currentState_ = State::Select;
}

void SelectStageDrawer::Update() {
}

void SelectStageDrawer::Draw() {

    // 各ステージを描画する
    for (auto& stage : stages_) {

        stage.frame.Draw();
        for (auto& sprite : stage.objects) {

            sprite.Draw();
        }
    }
}

void SelectStageDrawer::Edit() {

    ImFunc::CustomBegin("SelectStageDrawer", MoveOnly_TitleBar);
    {

        ImGui::End();
    }
}

void SelectStageDrawer::BuildAllStage() {

    // 全てのステージをCSVファイルを基に構築する
    stages_.clear();

    // ファイル名をすべて取得する
    auto csvFiles = SelectStageBuilder::CollectStageCSV("Resources/Stage");

    // 1番左の表示
    float cursorX = leftTranslate_.x;

    for (uint32_t index = 0; index < csvFiles.size(); ++index) {

        // CSVファイルを読み込む
        auto grid = SelectStageBuilder::LoadCSV(csvFiles[index]);
        if (grid.empty()) {
            continue;
        }

        const int rows = static_cast<int>(grid.size());
        const int cols = static_cast<int>(grid.front().size());

        // このステージ描画情報
        Stage stage;
        stage.index = index;
        // 全体のサイズ
        stage.size = outSize_;
        stage.translate = Vector2(cursorX + stage.size.x * 0.5f, leftTranslate_.y);

        // フレーム描画初期化
        // フレームは全体のサイズで設定する
        stage.frame = Sprite("Scene_Select/selectStageRect.png");
        stage.frame.size = stage.size;
        stage.frame.anchorPoint = 0.5f;
        stage.frame.translate = stage.translate;

        // グリッドを描画サイズでぴったり埋まるようにタイルサイズを設定する
        Vector2 tileSize = Vector2(stage.size.x / static_cast<float>(cols),
            stage.size.y / static_cast<float>(rows));
        // 左上原点座標
        const float left = stage.translate.x - stage.size.x * 0.5f;
        const float top = stage.translate.y - stage.size.y * 0.5f;
        for (int r = 0; r < rows; ++r) {

            const int colsThis = static_cast<int>(grid[r].size());
            for (int c = 0; c < colsThis; ++c) {

                // CSVのインデックスからスプライトを作成する
                const int id = grid[r][c];
                // Noneは処理しない
                if (id == 0) {
                    continue;
                }
                // 表示座標を設定
                Vector2 translate = Vector2(left + (c + 0.5f) * tileSize.x,
                    top + (r + 0.5f) * tileSize.y);
                stage.objects.emplace_back(CreateTileSprite(id, translate, tileSize));
            }
        }
        // 配列に追加
        stages_.push_back(std::move(stage));
        // 次のステージの開始座標を設定
        cursorX += outSize_.x + 32.0f;
    }
}

Sprite SelectStageDrawer::CreateTileSprite(uint32_t index,
    const Vector2& translate, const Vector2& size) {

    // スプライトをパスから作成
    Sprite sprite = Sprite(GetFileNameFromIndex(index));
    // 設定
    sprite.anchorPoint = 0.5f;
    sprite.translate = translate;
    sprite.size = size;

    // 個別のサイズ設定
    if (static_cast<StageObjectType>(index) == StageObjectType::Player) {

        sprite.size *= 0.8f;
    }
    return sprite;
}

std::string SelectStageDrawer::GetFileNameFromIndex(uint32_t index) const {

    // 番号からスプライト画像のパスを取得する
    switch (index) {
    case 1:  return "Scene_Game/StageObject/normalBlock.png";       // NormalBlock
    case 2:  return "Scene_Game/StageObject/crown.png";             // Goal
    case 3:  return "Scene_Game/StageObject/Player/PlayerBody.png"; // Player
    case 4:  return "Scene_Game/StageObject/frameRect.png";         // Warp
    case 5:  return "Scene_Game/StageObject/dottedLine.png";        // EmptyBlock
    case 6:  return "Scene_Game/StageObject/normalBlock.png";       // LaserLauncher
    }
    return "";
}