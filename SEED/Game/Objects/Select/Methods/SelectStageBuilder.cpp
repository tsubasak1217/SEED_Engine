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
    auto getIndex = [](const std::string& path) -> int {
        // 末尾の番号を抽出（例: stage_12.csv -> 12）
        std::smatch m;
        static const std::regex re(R"(.*?stage_(\d+)\.csv$)");
        if (std::regex_match(path, m, re)) {
            try { return std::stoi(m[1].str()); }
            catch (...) { /* fallthrough */ }
        }
        // マッチしないものは最後へ
        return std::numeric_limits<int>::max(); };

    std::sort(files.begin(), files.end(),
        [&](const std::string& a, const std::string& b) {
            int ia = getIndex(a);
            int ib = getIndex(b);
            // 数値で比較
            if (ia != ib) return ia < ib;
            // 同点時は辞書順
            return a < b;
        });
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