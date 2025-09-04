#include "GameStageBuilder.h"

//============================================================================
//	include
//============================================================================
#include <Environment/Environment.h>
#include <SEED/Source/Basic/Object/GameObject2D.h>
#include <Game/GameSystem.h>
#include <Game/Objects/Stage/Enum/StageObjectType.h>
#include <Game/Objects/Stage/Methods/GameStageHelper.h>
#include <Game/Components/StageObjectComponent.h>

//============================================================================
//	GameStageBuilder classMethods
//============================================================================

std::list<GameObject2D*> GameStageBuilder::CreateFromCSVFile(
    const std::string& fileName, float tileSize) {

    // CSVデータ解析
    std::vector<std::vector<int>> grid = GetCSVData(fileName);

    // オブジェクトの幅
    const int rows = static_cast<int>(grid.size());
    const int cols = static_cast<int>(grid.front().size());

    // ウィンドウの中心になるようにする
    const float windowW = kWindowSize.x;
    const float centerX = windowW * 0.5f;
    const float startX = std::round((centerX - (cols *
        tileSize) * 0.5f) / tileSize) * tileSize;

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
            const float x = startX + c * tileSize;
            const float y = startY + r * tileSize;

            // オブジェクトを作成
            GameObject2D* object = new GameObject2D(GameSystem::GetScene());
            object->SetWorldTranslate({ x, y });
            StageObjectComponent* component = object->AddComponent<StageObjectComponent>();
            component->Initialize(static_cast<StageObjectType>(id), Vector2(0, 0), tileSize);
            objectList.push_back(object);
        }
    }
    return objectList;
}

std::list<GameObject2D*> GameStageBuilder::CreateFromBorderLine(std::list<GameObject2D*> objects,
    float axisX, float playerY, int direction, float tileSize) {

    // タイルに合うように切り上げた座標を設定する
    auto roundToTile = [tileSize](float pos) {return std::round(pos / tileSize) * tileSize; };

    std::list<GameObject2D*> objectList{};
    // オブジェクト内で範囲内のオブジェクトをホログラムとして作成する
    for (const auto& object : objects) {

        // オブジェクトコンポーネントを取得
        StageObjectComponent* component = object->GetComponent<StageObjectComponent>();
        if (GameStageHelper::IsSkipHologramObject(component)) {
            continue;
        }

        // コピー対象のオブジェクト情報
        const Vector2 sourcePos = object->GetWorldTranslate();
        const StageObjectType sourceType = component->GetStageObjectType();
        // プレイヤーのY座標より下のオブジェクトは作成しない
        if (!(playerY >= sourcePos.y)) {
            continue;
        }

        // プレイヤーの向いている方向の逆のオブジェクトをホログラム作成対象にする
        const bool isOppositeSide = (direction > 0) ? (sourcePos.x < axisX) : (sourcePos.x > axisX);
        if (!isOppositeSide) {
            continue;
        }

        // 境界線Xを軸に左右対称に反転した座標を設定する
        Vector2 dstPos{};
        dstPos.x = roundToTile(2.0f * axisX - sourcePos.x);
        dstPos.y = sourcePos.y;

        // 元のタイプでオブジェクトを作成
        GameObject2D* newBlock = new GameObject2D(GameSystem::GetScene());
        newBlock->SetWorldTranslate(dstPos);
        StageObjectComponent* newComponent = newBlock->AddComponent<StageObjectComponent>();
        newComponent->Initialize(sourceType, Vector2(0.0f, 0.0f), tileSize);
        newComponent->SetObjectCommonState(StageObjectCommonState::Hologram);
        objectList.push_back(std::move(newBlock));
    }
    return objectList;
}

std::vector<std::vector<int>> GameStageBuilder::GetCSVData(const std::string& fileName) {

    // 読み込めなければエラー
    std::ifstream ifs("Resources/Stage/" + fileName);
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