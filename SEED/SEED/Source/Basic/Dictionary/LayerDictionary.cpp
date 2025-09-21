#include "LayerDictionary.h"

void LayerDictionary::Initialize(){
    dict.clear();

    // jsonファイルの読み込み
    nlohmann::json j = MyFunc::GetJson(jsonFilePath_,true);

    if(!j.empty()){
        for(auto& element : j.items()){
            std::string layerName = element.key();
            int layerValue = element.value();
            dict[layerName] = layerValue;
        }
    }
}