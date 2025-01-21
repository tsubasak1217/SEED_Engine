#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include "Vector3.h"

using RoutineLibrary = std::unordered_map<std::string, std::vector<Vector3>>;

class EnemyRoutineManager{
public:
    EnemyRoutineManager() = default;
    ~EnemyRoutineManager() = default;

    // ルーチンの追加・削除
    void AddRoutine(const std::string& name, const std::vector<Vector3>& points);
    void DeleteRoutine(const std::string& name);

    // ルーチンの取得
    const std::vector<Vector3>* GetRoutinePoints(const std::string& name) const;
    std::vector<std::string> GetRoutineNames() const;

    // 保存・読み込み
    void SaveRoutines(uint32_t stageNum) const;
    void LoadRoutines(uint32_t stageNum);

private:
    RoutineLibrary routines_;
};
