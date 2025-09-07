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

//============================================================================
//	SelectStageBuilder class
//============================================================================
class SelectStageBuilder {
public:
	//========================================================================
	//	public Methods
	//========================================================================

    SelectStageBuilder() = default;
	~SelectStageBuilder() = default;

    // CSVファイルをすべて取得する
    static std::vector<std::string> CollectStageCSV(const std::string& directoryPath);
    // CSVからグリッド配置を取得する
    static std::vector<std::vector<uint32_t>> LoadCSV(const std::string& filePath);
};