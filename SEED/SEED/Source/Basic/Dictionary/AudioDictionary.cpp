#include "AudioDictionary.h"

void AudioDictionary::Initialize() {
    dict.clear();

    // jsonファイルの読み込み
    nlohmann::json j = MyFunc::GetJson(jsonFilePath_, true);

    if(!j.empty()){
        for(auto& element : j.items()){
            std::string audioName = element.key();
            std::string audioPath = element.value();
            dict[audioName] = audioPath;
        }
    }
}