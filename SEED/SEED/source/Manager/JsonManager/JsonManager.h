#pragma once
#include <string>
#include <unordered_map>

class JsonManager{
private:
    // プライベートコンストラクタ
    JsonManager() = default;
    // コピー禁止
    JsonManager(const JsonManager&) = delete;
    void operator=(const JsonManager&) = delete;

public:
    ~JsonManager() = default;
    static JsonManager* GetInstance();
    static void Initialize();

private:
    static JsonManager* instance_;

private:
    std::unordered_map<std::string, std::string> jsonMap_;// ファイル名 : ディレクトリパス

};