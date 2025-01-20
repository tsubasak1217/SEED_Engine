#include "CsvAdapter.h"

//lib
#include <fstream>
#include <filesystem>
#include <sstream>
#include <iostream>

CsvAdapter* CsvAdapter::GetInstance(){
    CsvAdapter* instance = new CsvAdapter();
    return instance;
}

void CsvAdapter::SaveCsv(const std::string& fileName,
                         const std::vector<std::vector<std::string>>& data){
    std::string path = directoryPath_ + fileName + ".csv";
    // ディレクトリが存在しなければ作成
    std::filesystem::path dir(directoryPath_);
    if (!std::filesystem::exists(dir)){
        std::filesystem::create_directories(dir);
    }
    std::ofstream ofs(path);
    if (!ofs){
        std::cerr << "[CsvAdapter] Failed to open file for writing: " << path << std::endl;
        return;
    }

    // data: [行][列]
    for (const auto& row : data){
        bool first = true;
        for (const auto& col : row){
            if (!first){
                ofs << ",";
            }
            ofs << col;
            first = false;
        }
        ofs << "\n";
    }

    ofs.close();
    std::cout << "[CsvAdapter] Saved CSV: " << path << std::endl;
}

std::vector<std::vector<std::string>> CsvAdapter::LoadCsv(const std::string& fileName){
    std::vector<std::vector<std::string>> result;

    std::string path = directoryPath_ + fileName + ".csv";
    std::ifstream ifs(path);
    if (!ifs){
        std::cerr << "[CsvAdapter] Failed to open file for reading: " << path << std::endl;
        return result;
    }

    std::string line;
    while (std::getline(ifs, line)){
        std::stringstream ss(line);
        std::vector<std::string> tokens;
        std::string cell;
        while (std::getline(ss, cell, ',')){
            tokens.push_back(cell);
        }
        result.push_back(tokens);
    }

    ifs.close();
    std::cout << "[CsvAdapter] Loaded CSV: " << path << std::endl;
    return result;
}