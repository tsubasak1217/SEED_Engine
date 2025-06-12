#include "IComponent.h"

IComponent::IComponent(GameObject* pOwner, const std::string& tagName){
    owner_ = pOwner;
    componentTag_ = tagName;
    componentID_ = nextComponentID_++;
}

nlohmann::json IComponent::GetJsonData() const{
    nlohmann::json j;
    j["componentID"] = componentID_;
    j["componentTag"] = componentTag_;
    return j;
}

void IComponent::LoadFromJson(const nlohmann::json& jsonData){
    // コンポーネントのIDとタグを読み込む
    componentID_ = jsonData["componentID"];
    componentTag_ = jsonData["componentTag"];
}
