#include "GameStageBuilder.h"

//============================================================================
//	include
//============================================================================
#include <Environment/Environment.h>
#include <SEED/Source/Basic/Object/GameObject2D.h>
#include <SEED/Lib/MagicEnumAdapter/EnumAdapter.h>
#include <Game/GameSystem.h>
#include <Game/Objects/Stage/Methods/GameStageHelper.h>
#include <Game/Components/StageObjectComponent.h>

// objects
#include <Game/Objects/Stage/Objects/Block/BlockNormal.h>
#include <Game/Objects/Stage/Objects/Goal/Goal.h>
#include <Game/Objects/Stage/Objects/Warp/Warp.h>
#include <Game/Objects/Stage/Objects/Player/Entity/Player.h>

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

    // オブジェクトのインデックス
    std::array<uint32_t, EnumAdapter<StageObjectType>::GetEnumCount()> objectIndex{};
    for (int r = 0; r < rows; ++r) {

        // 行ごとの列数がズレていたら安全側で合わせる
        const int colsThisRow = static_cast<int>(grid[r].size());
        for (int c = 0; c < colsThisRow; ++c) {

            int id = grid[r][c];

            // Emptyは作成しない
            if (id == static_cast<int>(StageObjectType::None)) {
                continue;
            }

            // タイル左上を原点とした配置
            const float x = startX + c * tileSize;
            const float y = startY + r * tileSize;

            // オブジェクトを作成
            GameObject2D* object = new GameObject2D(GameSystem::GetScene());
            object->SetWorldTranslate({ x, y });
            StageObjectComponent* component = object->AddComponent<StageObjectComponent>();

            if (id == static_cast<int>(StageObjectType::Player)) {
                // プレイヤーはサイズを少し小さくする
                float playerTileSize = tileSize * 0.8f;
                component->Initialize(static_cast<StageObjectType>(id), Vector2(0, 0), playerTileSize);
            } else {
                component->Initialize(static_cast<StageObjectType>(id), Vector2(0, 0), tileSize);
            }


            // オブジェクトごとの個別処理
            IndividualSetting(*component, objectIndex[id]);

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
        StageObjectComponent* sourceComponent = object->GetComponent<StageObjectComponent>();
        if (GameStageHelper::IsSkipHologramObject(sourceComponent)) {
            continue;
        }

        // コピー対象のオブジェクト情報
        const Vector2 sourcePos = object->GetWorldTranslate();
        const StageObjectType sourceType = sourceComponent->GetStageObjectType();
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

        // オブジェクトごとの個別処理
        IndividualSetting(*newComponent, *sourceComponent);

        objectList.push_back(std::move(newBlock));
    }
    return objectList;
}

void GameStageBuilder::IndividualSetting(StageObjectComponent& component, uint32_t& objectIndex) {

    StageObjectType objectType = component.GetStageObjectType();
    switch (objectType) {
    case StageObjectType::None:
    {

        break;
    }
    case StageObjectType::NormalBlock:
    {

        ++objectIndex;
        break;
    }
    case StageObjectType::Goal:
    {

        ++objectIndex;
        break;
    }
    case StageObjectType::Player:
    {

        ++objectIndex;
        break;
    }
    case StageObjectType::Warp:
    {

        Warp* warp = component.GetStageObject<Warp>();
        warp->SetWarpIndex(objectIndex);

        ++objectIndex;
        break;
    }
    case StageObjectType::EmptyBlock:
    {
        ++objectIndex;
        break;
    }
    }
}

void GameStageBuilder::IndividualSetting(StageObjectComponent& dstComponent, const StageObjectComponent& sourceComponent) {

    StageObjectType objectType = sourceComponent.GetStageObjectType();
    switch (objectType) {
    case StageObjectType::NormalBlock: {
        break;
    }
    case StageObjectType::Goal: {
        break;
    }
    case StageObjectType::Player: {
        break;
    }
    case StageObjectType::Warp: {

        // ワープのインデックスを共有する
        Warp* dstWarp = dstComponent.GetStageObject<Warp>();
        Warp* sourceWarp = sourceComponent.GetStageObject<Warp>();
        dstWarp->SetWarpIndex(sourceWarp->GetWarpIndex());
        break;
    }
    case StageObjectType::EmptyBlock:
    {
        break;
    }
    }
}

void GameStageBuilder::CreateColliders(std::list<GameObject2D*>& objects, float tileSize) {

    for (GameObject2D* object : objects) {
        if (StageObjectComponent* component = object->GetComponent<StageObjectComponent>()) {

            StageObjectType type = component->GetStageObjectType();

            // Collisionの追加
            Collision2DComponent* collision = object->AddComponent<Collision2DComponent>();
            Collider_AABB2D* aabb = new Collider_AABB2D();
            aabb->SetParentMatrix(object->GetWorldMatPtr());

            switch (type) {
            case StageObjectType::None:
            {
                aabb->SetSize({ tileSize,tileSize });
                aabb->isMovable_ = false;
                object->SetObjectType(ObjectType::Field);
                break;
            }
            case StageObjectType::NormalBlock:
            {
                aabb->SetSize({ tileSize,tileSize });
                aabb->isMovable_ = false;
                object->SetObjectType(ObjectType::Field);
                break;
            }
            case StageObjectType::Goal:
            {
                aabb->SetSize({ tileSize,tileSize });
                aabb->isMovable_ = false;
                aabb->isGhost_ = true;
                object->SetObjectType(ObjectType::Goal);
                break;
            }
            case StageObjectType::Player:
            {
                float playerSize = tileSize * 0.8f;
                aabb->SetSize({ playerSize,playerSize });
                aabb->isMovable_ = true;
                object->SetObjectType(ObjectType::Player);
                break;
            }
            case StageObjectType::Warp:
            {
                aabb->SetSize({ tileSize * 0.8f,tileSize * 0.8f });
                aabb->isMovable_ = false;
                aabb->isGhost_ = true;
                object->SetObjectType(ObjectType::Warp);
                break;
            }
            case StageObjectType::EmptyBlock:
            {
                aabb->SetSize({ tileSize,tileSize });
                aabb->isMovable_ = false;
                aabb->isGhost_ = true; // 当たり判定を無効にする
                object->SetObjectType(ObjectType::Field);
                break;
            }
            default:
                break;
            }

            aabb->UpdateMatrix();
            aabb->SetOwnerObject(object);
            collision->AddCollider(aabb);
        }
    }
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