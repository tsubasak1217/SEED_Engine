#pragma once

// lib
#include <string>
#include <unordered_map>
#include <vector>
#include "Vector3.h"
#include "../adapter/csv/CsvAdapter.h"  // 汎用CSVクラスを使う

using RoutineLibrary = std::unordered_map<std::string, std::vector<Vector3>>;

class EnemyRoutineManager{
public:
    EnemyRoutineManager() = default;
    ~EnemyRoutineManager() = default;

    // CSV 保存
    void Save(const std::string& fileName, const RoutineLibrary& library);

    // CSV 読み込み
    void Load(const std::string& fileName, RoutineLibrary& library);

    // CsvAdapter を外部から注入する場合
    void SetCsvAdapter(CsvAdapter* adapter){ pCsvAdapter_ = adapter; }

private:
    CsvAdapter* pCsvAdapter_ = nullptr;
};