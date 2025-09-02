#include "JsonAdapter.h"

//============================================================================
//	JsonAdapter classMethods
//============================================================================

const std::string& JsonAdapter::baseDirectoryFilePath_ = "./Resources/Jsons/";

void JsonAdapter::Save(const std::string& saveDirectoryFilePath, const nlohmann::json& jsonData) {

    // フォルダのフルパス取得
    std::string fullPath = baseDirectoryFilePath_ + saveDirectoryFilePath;
    std::filesystem::path path(fullPath);

    if (path.extension() != ".json") {

        path.replace_extension(".json");
        fullPath = path.string();
    }

    if (!std::filesystem::exists(path.parent_path())) {
        std::filesystem::create_directories(path.parent_path());
    }

    std::ofstream file(fullPath);

    if (!file.is_open()) {

        assert(false && "failed to open file for saving json");
        return;
    }

    file << jsonData.dump(4);
    file.close();
}

bool JsonAdapter::LoadCheck(const std::string& loadDirectoryFilePath, nlohmann::json& data) {

    std::string fullPath = baseDirectoryFilePath_ + loadDirectoryFilePath;
    std::ifstream file(fullPath);

    if (!file.is_open()) {
        return false;
    }

    file >> data;
    file.close();

    return true;
}