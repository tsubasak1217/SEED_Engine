#pragma once

//============================================================================
//	include
//============================================================================

// json
#include <SEED/External/nlohmann/json.hpp>
// c++
#include <string>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <vector>
#include <array>
#include <cassert>

//============================================================================
//	JsonAdapter class
//============================================================================
class JsonAdapter {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	JsonAdapter() = default;
	~JsonAdapter() = default;

    static void Save(const std::string& saveDirectoryFilePath, const nlohmann::json& jsonData);
    static bool LoadCheck(const std::string& loadDirectoryFilePath, nlohmann::json& data);
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

    static const std::string& baseDirectoryFilePath_;
};