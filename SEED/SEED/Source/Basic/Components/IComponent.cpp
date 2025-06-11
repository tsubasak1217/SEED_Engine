#include "IComponent.h"

IComponent::IComponent(GameObject* pOwner, const std::string& tagName){
    owner_ = pOwner;
    componentTag_ = tagName;
}
