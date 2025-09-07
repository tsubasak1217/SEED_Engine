#include "SelectStageBuilder.h"

//============================================================================
//	SelectStageBuilder classMethods
//============================================================================

std::vector<std::string> SelectStageBuilder::CollectStageCSV(const std::string& directoryPath) {

    std::vector<std::string> files;
    for (auto& p : std::filesystem::directory_iterator(directoryPath)) {
        if (p.is_regular_file() && p.path().extension() == ".csv") {
            files.emplace_back(p.path().string());
        }
    }
    // 名前順でソートする
    std::sort(files.begin(), files.end());
    return files;
}

std::vector<std::vector<uint32_t>> SelectStageBuilder::LoadCSV(const std::string& filePath) {

    std::ifstream ifs(filePath);
    if (!ifs) {
        return {};
    }

    std::vector<std::vector<uint32_t>> grid;
    std::string line;
    while (std::getline(ifs, line)) {

        std::vector<uint32_t> row;
        std::stringstream ss(line);
        std::string cell;
        while (std::getline(ss, cell, ',')) {

            // 空セル対策
            if (cell.empty()) {

                cell = "0";
            }
            row.push_back(std::stoi(cell));
        }
        if (!row.empty()) {
            grid.push_back(move(row));
        }
    }
    return grid;
}