#include "IComponent.h"

IComponent::IComponent(std::variant<GameObject*, GameObject2D*> pOwner, const std::string& tagName){

    // 所有者の設定
    if(std::holds_alternative<GameObject*>(pOwner)) {
        owner_.owner3D = std::get<GameObject*>(pOwner);

    } else if (std::holds_alternative<GameObject2D*>(pOwner)) {
        owner_.is2D = true;
        owner_.owner2D = std::get<GameObject2D*>(pOwner);

    } else {
        assert(false);
    }

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
