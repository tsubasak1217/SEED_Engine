#include "AudioDictionary.h"

void AudioDictionary::Initialize() {
    dict.clear();

    // jsonファイルの読み込み
    nlohmann::json j = MyFunc::GetJson(jsonFilePath_, true);

    if(!j.empty()){
        for(auto& element : j.items()){
            std::string layerName = element.key();
            std::string layerValue = element.value();
            dict[layerName] = layerValue;
        }
    }
}