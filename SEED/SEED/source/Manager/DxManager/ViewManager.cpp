#include "ViewManager.h"
#include "DescriptoeHeap_SRV_CBV_UAV.h"
#include "DescriptoeHeap_RTV.h"
#include "DescriptoeHeap_DSV.h"
#include "DescriptorHeap.h"

ViewManager* ViewManager::instance_ = nullptr;

ViewManager::~ViewManager(){
    delete instance_;
    instance_ = nullptr;
}

ViewManager* ViewManager::GetInstance(){
    if(!instance_){
        instance_ = new ViewManager();
    }

    instance_->Initialize();
    return instance_;
}


/// <summary>
/// 初期化
/// </summary>
void ViewManager::Initialize(){

    // 各ヒープの作成
    descriptorHeaps_["SRV_CBV_UAV"] = std::make_unique<DescriptorHeap_SRV_CBV_UAV>();
    descriptorHeaps_["RTV"] = std::make_unique<DescriptorHeap_RTV>();
    descriptorHeaps_["DSV"] = std::make_unique<DescriptorHeap_DSV>();
}


/// <summary>
/// ビューを作成する関数
/// </summary>
/// <param name="viewType"></param>
/// <param name="pResource"></param>
/// <param name="pDesc"></param>
uint32_t ViewManager::CreateView(
    VIEW_TYPE viewType, ID3D12Resource* pResource,
    const void* pDesc, const std::string& viewName
){

    uint32_t handle = 0;

    if(viewType == VIEW_TYPE::SRV or viewType == VIEW_TYPE::CBV or viewType == VIEW_TYPE::UAV){

        handle = instance_->descriptorHeaps_["SRV_CBV_UAV"]->CreateView(viewType, pResource, pDesc);
        instance_->handles_[(uint32_t)DESCRIPTOR_HEAP_TYPE::SRV_CBV_UAV].try_emplace(viewName,handle);
        return handle;

    } else if(viewType == VIEW_TYPE::RTV){

        handle = instance_->descriptorHeaps_["RTV"]->CreateView(viewType, pResource, pDesc);
        instance_->handles_[(uint32_t)DESCRIPTOR_HEAP_TYPE::RTV].try_emplace(viewName, handle);
        return handle;

    } else if(viewType == VIEW_TYPE::DSV){

        handle = instance_->descriptorHeaps_["DSV"]->CreateView(viewType, pResource,pDesc);
        instance_->handles_[(uint32_t)DESCRIPTOR_HEAP_TYPE::DSV].try_emplace(viewName, handle);
        return handle;
    }

    assert(false);
    return 0;
}


/// <summary>
/// テクスチャのハンドルを取得する関数
/// </summary>
/// <param name="textureName"></param>
/// <returns></returns>
int32_t ViewManager::GetTextureHandle(const std::string& textureName){

    if(instance_->handles_[(uint32_t)DESCRIPTOR_HEAP_TYPE::SRV_CBV_UAV].find(textureName)
        == instance_->handles_[(uint32_t)DESCRIPTOR_HEAP_TYPE::SRV_CBV_UAV].end()){
        return -1;
    }

    return instance_->handles_[(uint32_t)DESCRIPTOR_HEAP_TYPE::SRV_CBV_UAV][textureName];
}



/// <summary>
/// 
/// </summary>
/// <param name="heapType"></param>
/// <param name="textureName"></param>
/// <returns></returns>
D3D12_CPU_DESCRIPTOR_HANDLE ViewManager::GetHandle(DESCRIPTOR_HEAP_TYPE heapType, const std::string& textureName){

    D3D12_CPU_DESCRIPTOR_HANDLE heapStart{};
    uint32_t distance = 0;

    if(heapType == DESCRIPTOR_HEAP_TYPE::SRV_CBV_UAV){
        heapStart = instance_->descriptorHeaps_["SRV_CBV_UAV"]->GetHeapStart();
        distance = instance_->descriptorHeaps_["SRV_CBV_UAV"]->GetDescriptorSize() 
            * instance_->handles_[(uint32_t)DESCRIPTOR_HEAP_TYPE::SRV_CBV_UAV][textureName];

    } else if(heapType == DESCRIPTOR_HEAP_TYPE::RTV){
        heapStart = instance_->descriptorHeaps_["RTV"]->GetHeapStart();
        distance = instance_->descriptorHeaps_["RTV"]->GetDescriptorSize()
            * instance_->handles_[(uint32_t)DESCRIPTOR_HEAP_TYPE::RTV][textureName];

    } else if(heapType == DESCRIPTOR_HEAP_TYPE::DSV){
        heapStart = instance_->descriptorHeaps_["DSV"]->GetHeapStart();
        distance = instance_->descriptorHeaps_["DSV"]->GetDescriptorSize()
            * instance_->handles_[(uint32_t)DESCRIPTOR_HEAP_TYPE::DSV][textureName];

    } else{
        assert(false);
    }

    heapStart.ptr += distance;
    return heapStart;
}



/// <summary>
/// 
/// </summary>
/// <param name="heapType"></param>
/// <returns></returns>
ComPtr<ID3D12DescriptorHeap> ViewManager::GetHeap(DESCRIPTOR_HEAP_TYPE heapType){

    if(heapType == DESCRIPTOR_HEAP_TYPE::SRV_CBV_UAV){
        return instance_->descriptorHeaps_["SRV_CBV_UAV"]->GetHeap();

    } else if(heapType == DESCRIPTOR_HEAP_TYPE::RTV){
        return instance_->descriptorHeaps_["RTV"]->GetHeap();

    } else if(heapType == DESCRIPTOR_HEAP_TYPE::DSV){
        return instance_->descriptorHeaps_["DSV"]->GetHeap();

    } else{
        assert(false);
        return nullptr;
    }
}


/// <summary>
/// 
/// </summary>
/// <param name="heapType"></param>
/// <returns></returns>
uint32_t ViewManager::GetDescriptorSize(DESCRIPTOR_HEAP_TYPE heapType){

    if(heapType == DESCRIPTOR_HEAP_TYPE::SRV_CBV_UAV){
        return instance_->descriptorHeaps_["SRV_CBV_UAV"]->GetDescriptorSize();

    } else if(heapType == DESCRIPTOR_HEAP_TYPE::RTV){
        return instance_->descriptorHeaps_["RTV"]->GetDescriptorSize();

    } else if(heapType == DESCRIPTOR_HEAP_TYPE::DSV){
        return instance_->descriptorHeaps_["DSV"]->GetDescriptorSize();

    } else{
        assert(false);
        return 0;
    }
}
