#include "EnemyRoutineManager.h"
#include <iostream>

void EnemyRoutineManager::Save(const std::string& fileName, const RoutineLibrary& library){
    if (!pCsvAdapter_){
        std::cerr << "[EnemyRoutineManager] CsvAdapter is null!" << std::endl;
        return;
    }

    // CSVに書き込むための 2次元文字列配列を作る
    std::vector<std::vector<std::string>> csvData;

    // 1行目: ヘッダ (任意)
    csvData.push_back({"RoutineName", "PointCount"});

    // ライブラリの各要素を1行に
    for (auto& kv : library){
        const std::string& routineName = kv.first;
        const auto& points = kv.second;

        // 1行分の文字列を格納するベクタ
        std::vector<std::string> row;
        row.push_back(routineName);
        row.push_back(std::to_string(points.size()));

        // points から (x,y,z) を続けて文字列化
        for (auto& p : points){
            row.push_back(std::to_string(p.x));
            row.push_back(std::to_string(p.y));
            row.push_back(std::to_string(p.z));
        }

        csvData.push_back(row);
    }

    // CsvAdapter で保存
    pCsvAdapter_->SaveCsv(fileName, csvData);
}

void EnemyRoutineManager::Load(const std::string& fileName, RoutineLibrary& library){
    if (!pCsvAdapter_){
        std::cerr << "[RoutineLibraryManager] CsvAdapter is null!" << std::endl;
        return;
    }

    // CSV -> 2次元文字列配列
    auto csvData = pCsvAdapter_->LoadCsv(fileName);
    if (csvData.empty()){
        return; // 何も無い
    }

    // 1行目はヘッダと想定 => 2行目以降を処理
    bool isHeaderSkipped = false;
    for (auto& row : csvData){
        // 先頭行をスキップ
        if (!isHeaderSkipped){
            isHeaderSkipped = true;
            continue;
        }
        if (row.size() < 2){
            // RoutineName, PointCount が無い
            continue;
        }

        // 1列目: ルーチン名
        std::string routineName = row[0];
        // 2列目: ポイント数
        int pointCount = std::stoi(row[1]);

        // pointCount が 0 なら空
        // それ以上なら (x,y,z) * pointCount = 3 * pointCount 個必要
        size_t requiredSize = 2 + (3 * ( size_t ) pointCount);
        if (row.size() < requiredSize){
            // データ不足
            continue;
        }

        std::vector<Vector3> points;
        points.reserve(pointCount);

        // row[2] 以降に x,y,z が並んでいる
        size_t idx = 2;
        for (int i = 0; i < pointCount; ++i){
            Vector3 p;
            p.x = std::stof(row[idx++]);
            p.y = std::stof(row[idx++]);
            p.z = std::stof(row[idx++]);
            points.push_back(p);
        }

        // ライブラリに上書き(または新規追加)
        library[routineName] = points;
    }
}