#include "GameStageBuilder.h"

//============================================================================
//	include
//============================================================================
#include <Environment/Environment.h>
#include <SEED/Source/Basic/Object/GameObject2D.h>
#include <Game/GameSystem.h>
#include <Game/Objects/Stage/Enum/StageObjectType.h>

//============================================================================
//	GameStageBuilder classMethods
//============================================================================

std::list<GameObject2D*> GameStageBuilder::Create(const std::string& fileName) {

    // CSVデータ解析
    std::vector<std::vector<int>> grid = GetCSVData(fileName);

    // オブジェクトの幅
    const float tile = 32.0f;
    const int rows = static_cast<int>(grid.size());
    const int cols = static_cast<int>(grid.front().size());

    // ウィンドウの中心になるようにする
    const float windowW = kWindowSize.x;
    const float centerX = windowW * 0.5f;
    const float startX = std::round((centerX - (cols * tile) * 0.5f) / tile) * tile;

    const float startY = 360.0f;
    std::list<GameObject2D*> objectList{};

    for (int r = 0; r < rows; ++r) {

        // 行ごとの列数がズレていたら安全側で合わせる
        const int colsThisRow = static_cast<int>(grid[r].size());
        for (int c = 0; c < colsThisRow; ++c) {

            int id = grid[r][c];

            // Emptyは作成しない
            if (id == 0) {
                continue;
            }

            // タイル左上を原点とした配置
            const float x = startX + c * tile;
            const float y = startY + r * tile;

            // オブジェクトを作成
            GameObject2D* object = new GameObject2D(GameSystem::GetScene());
            StageObjectComponent* component = object->AddComponent<StageObjectComponent>();
            component->Initialize(static_cast<StageObjectType>(id), Vector2(x, y));
            objectList.push_back(object);
        }
    }
    return objectList;
}

std::vector<std::vector<int>> GameStageBuilder::GetCSVData(const std::string& fileName) {

    // 読み込めなければエラー
    std::ifstream ifs(kCSVPath_ + fileName);
    if (!ifs) {
        assert(false && "Failed to open CSV");
    }

    std::vector<std::vector<int>> grid;
    std::string line;
    while (std::getline(ifs, line)) {

        std::vector<int> row;
        std::stringstream ss(line);
        std::string cell;
        while (std::getline(ss, cell, ',')) {

            // 空セル対策
            if (cell.empty()) cell = "0";
            row.push_back(std::stoi(cell));
        }
        if (!row.empty()) {
            grid.push_back(std::move(row));
        }
    }
    return grid;
}