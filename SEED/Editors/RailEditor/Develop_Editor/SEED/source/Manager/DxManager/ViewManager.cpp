#include "ViewManager.h"
#include "DescriptoeHeap_SRV_CBV_UAV.h"
#include "DescriptoeHeap_RTV.h"
#include "DescriptoeHeap_DSV.h"

void ViewManager::Initialize(){

    // 各ヒープの作成
    DescriptorHeaps_["SRV_CBV_UAV"] = std::make_unique<DescriptorHeap_SRV_CBV_UAV>();
    DescriptorHeaps_["RTV"] = std::make_unique<DescriptorHeap_RTV>();
    DescriptorHeaps_["DSV"] = std::make_unique<DescriptorHeap_DSV>();
}
