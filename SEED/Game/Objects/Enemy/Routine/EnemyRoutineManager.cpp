#include "EnemyRoutineManager.h"
#include "../adapter/json/JsonCoordinator.h"
#include "../adapter/csv/CsvAdapter.h"
#include <fstream>
#include <iostream>

////////////////////////////////////////////////////////////////////////
// CSV 保存
////////////////////////////////////////////////////////////////////////
void EnemyRoutineManager::AddRoutine(const std::string& name, const std::vector<Vector3>& points){
    routines_[name] = points;
}


////////////////////////////////////////////////////////////////////////
// ルーチンの削除
////////////////////////////////////////////////////////////////////////
void EnemyRoutineManager::DeleteRoutine(const std::string& name){
    routines_.erase(name);
}

////////////////////////////////////////////////////////////////////////
// ルーチンポイントの取得
////////////////////////////////////////////////////////////////////////
const std::vector<Vector3>* EnemyRoutineManager::GetRoutinePoints(const std::string& name) const{
    auto it = routines_.find(name);
    if (it != routines_.end()){
        return &(it->second);
    }
    return nullptr;
}

////////////////////////////////////////////////////////////////////////
// ルーチン名の取得
////////////////////////////////////////////////////////////////////////
std::vector<std::string> EnemyRoutineManager::GetRoutineNames() const{
    std::vector<std::string> names;
    for (const auto& pair : routines_){
        names.push_back(pair.first);
    }
    return names;
}

////////////////////////////////////////////////////////////////////////
// 保存
////////////////////////////////////////////////////////////////////////
void EnemyRoutineManager::SaveRoutines(uint32_t stageNum) const{
    std::vector<std::vector<std::string>> csvData;
    csvData.push_back({"RoutineName", "PointCount", "Points..."});

    for (const auto& kv : routines_){
        const std::string& name = kv.first;
        const auto& points = kv.second;

        std::vector<std::string> row;
        row.push_back(name);
        row.push_back(std::to_string(points.size()));

        for (const auto& point : points){
            row.push_back(std::to_string(point.x));
            row.push_back(std::to_string(point.y));
            row.push_back(std::to_string(point.z));
        }
        csvData.push_back(row);
    }

    //ステージごとに保存
    const std::string fileName = "stage" + std::to_string(stageNum) + "_routineLibrary";

    CsvAdapter::GetInstance()->SaveCsv(fileName, csvData);
}

////////////////////////////////////////////////////////////////////////
// 読み込み
////////////////////////////////////////////////////////////////////////
void EnemyRoutineManager::LoadRoutines(uint32_t stageNum){
    //ステージごとに保存
    const std::string fileName = "stage" + std::to_string(stageNum) + "_routineLibrary";
    auto csvData = CsvAdapter::GetInstance()->LoadCsv(fileName);
    if (csvData.size() <= 1) return;

    routines_.clear();

    for (size_t i = 1; i < csvData.size(); ++i){
        const auto& row = csvData[i];
        if (row.size() < 2) continue;

        std::string name = row[0];
        int pointCount = std::stoi(row[1]);
        std::vector<Vector3> points;
        points.reserve(pointCount);

        size_t idx = 2;
        for (int j = 0; j < pointCount; ++j){
            if (idx + 2 >= row.size()) break;
            Vector3 point;
            point.x = std::stof(row[idx++]);
            point.y = std::stof(row[idx++]);
            point.z = std::stof(row[idx++]);
            points.push_back(point);
        }

        routines_[name] = points;
    }
}