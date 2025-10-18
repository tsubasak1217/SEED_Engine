#pragma once
#include <string>
#include <cstdint>
#include <unordered_map>
#include <SEED/Lib/Functions/MyFunc.h>

// 描画レイヤーの索引辞書
class LayerDictionary{
public:
    static void Initialize();
    static int32_t Get(const std::string& key){

        // 一度のみ初期化
        static bool isInit = false;
        if(!isInit){
            Initialize();
            isInit = true;
        }

        auto it = dict.find(key);
        if(it != dict.end()){
            return it->second;
        }
        return -1;
    }
private:
    static inline std::unordered_map<std::string,int32_t> dict{};
    static inline std::string jsonFilePath_ = "Resources/Jsons/Dictionary/LayerDictionary.json";
};