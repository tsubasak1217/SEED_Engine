#pragma once

//============================================================================
//	include
//============================================================================

// c++
#include <vector>
#include <string>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <regex>
#include <unordered_map>

//============================================================================
//	StageAchievementCollector class
//============================================================================
class StageProgressCollector {
public:

    //========================================================================
    //	public Methods
    //========================================================================

    StageProgressCollector() = default;
    ~StageProgressCollector();

    void Initialize();

    void SetStageClear(int stageID, bool isClear);

    bool IsStageClear(int stageID) const;

    const std::unordered_map<int, bool>& GetStageClearMap() const { return stageClearMap_; }

    void ClearAllStages() { stageClearMap_.clear(); }

    void Reset() { stageClearMap_.clear(); }

    void Edit();

private:

    //--------- variables ----------------------------------------------------

    std::unordered_map<int, bool> stageClearMap_;

private:

    //========================================================================
    //	private Methods
    //========================================================================

    //json
    void SaveJson();
    void ApplyJson();
};