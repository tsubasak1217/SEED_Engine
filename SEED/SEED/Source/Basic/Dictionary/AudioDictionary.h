#pragma once
#include <string>
#include <unordered_map>
#include <SEED/Lib/Functions/FileFunc.h>

/// <summary>
/// 音声情報を辞書登録して管理するクラス
/// </summary>
class AudioDictionary{
public:
    static void Initialize();
    static const std::string& Get(const std::string& key) {

        // 一度のみ初期化
        static bool initialized = false;
        if(!initialized){
            Initialize();
            initialized = true;
        }

        auto it = dict.find(key);
        if(it != dict.end()){
            return it->second;
        }
        static const std::string emptyString = "";
        return emptyString;
    }
private:
    static inline std::unordered_map<std::string, std::string> dict{};
    static inline std::string jsonFilePath_ = "Resources/Jsons/Dictionary/AudioDictionary.json";
};