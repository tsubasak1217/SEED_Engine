#include <SEED/Source/Manager/DxManager/ViewManager.h>
#include <SEED/Source/Manager/DxManager/DescriptorHeap/DescriptorHeap_SRV_CBV_UAV.h>
#include <SEED/Source/Manager/DxManager/DescriptorHeap/DescriptorHeap_RTV.h>
#include <SEED/Source/Manager/DxManager/DescriptorHeap/DescriptorHeap_DSV.h>
#include <SEED/Source/Manager/DxManager/DescriptorHeap/DescriptorHeap.h>

namespace SEED{
    ViewManager* ViewManager::instance_ = nullptr;

    ViewManager::~ViewManager(){}

    void ViewManager::Finalize(){
        if(instance_){
            delete instance_;
            instance_ = nullptr;
        }
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

        // ビューを作成してハンドルを格納する
        if(viewType == VIEW_TYPE::SRV or viewType == VIEW_TYPE::CBV or viewType == VIEW_TYPE::UAV){

            handle = instance_->descriptorHeaps_["SRV_CBV_UAV"]->CreateView(viewType, pResource, pDesc);
            instance_->handles_[(uint32_t)HEAP_TYPE::SRV_CBV_UAV][viewName] = handle;
            return handle;

        } else if(viewType == VIEW_TYPE::RTV){

            handle = instance_->descriptorHeaps_["RTV"]->CreateView(viewType, pResource, pDesc);
            instance_->handles_[(uint32_t)HEAP_TYPE::RTV][viewName] = handle;
            return handle;

        } else if(viewType == VIEW_TYPE::DSV){

            handle = instance_->descriptorHeaps_["DSV"]->CreateView(viewType, pResource, pDesc);
            instance_->handles_[(uint32_t)HEAP_TYPE::DSV][viewName] = handle;
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

        if(instance_->handles_[(uint32_t)HEAP_TYPE::SRV_CBV_UAV].find(textureName)
            == instance_->handles_[(uint32_t)HEAP_TYPE::SRV_CBV_UAV].end()){
            return -1;
        }
        int32_t gh = instance_->handles_[(uint32_t)HEAP_TYPE::SRV_CBV_UAV][textureName];
        return gh;
    }


    /// <summary>
    /// 
    /// </summary>
    /// <param name="heapType"></param>
    /// <param name="index"></param>
    /// <returns></returns>
    D3D12_CPU_DESCRIPTOR_HANDLE ViewManager::GetHandleCPU(HEAP_TYPE heapType, uint32_t index){
        D3D12_CPU_DESCRIPTOR_HANDLE heapStart{};
        uint32_t distance = 0;

        if(heapType == HEAP_TYPE::SRV_CBV_UAV){
            heapStart = instance_->descriptorHeaps_["SRV_CBV_UAV"]->GetHeapStartCPU();
            distance = instance_->descriptorHeaps_["SRV_CBV_UAV"]->GetDescriptorSize() * index;

        } else if(heapType == HEAP_TYPE::RTV){
            heapStart = instance_->descriptorHeaps_["RTV"]->GetHeapStartCPU();
            distance = instance_->descriptorHeaps_["RTV"]->GetDescriptorSize() * index;

        } else if(heapType == HEAP_TYPE::DSV){
            heapStart = instance_->descriptorHeaps_["DSV"]->GetHeapStartCPU();
            distance = instance_->descriptorHeaps_["DSV"]->GetDescriptorSize() * index;

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
    /// <param name="textureName"></param>
    /// <returns></returns>
    D3D12_CPU_DESCRIPTOR_HANDLE ViewManager::GetHandleCPU(HEAP_TYPE heapType, const std::string& viewName){

        uint32_t distance = 0;

        if(heapType == HEAP_TYPE::SRV_CBV_UAV){
            distance = instance_->handles_[(uint32_t)HEAP_TYPE::SRV_CBV_UAV][viewName];

        } else if(heapType == HEAP_TYPE::RTV){
            distance = instance_->handles_[(uint32_t)HEAP_TYPE::RTV][viewName];

        } else if(heapType == HEAP_TYPE::DSV){
            distance = instance_->handles_[(uint32_t)HEAP_TYPE::DSV][viewName];

        } else{
            assert(false);
        }

        return GetHandleCPU(heapType, distance);
    }



    /// <summary>
    /// 
    /// </summary>
    /// <param name="heapType"></param>
    /// <param name="index"></param>
    /// <returns></returns>
    D3D12_GPU_DESCRIPTOR_HANDLE ViewManager::GetHandleGPU(HEAP_TYPE heapType, uint32_t index){

        D3D12_GPU_DESCRIPTOR_HANDLE heapStart{};
        uint32_t distance = 0;

        if(heapType == HEAP_TYPE::SRV_CBV_UAV){
            heapStart = instance_->descriptorHeaps_["SRV_CBV_UAV"]->GetHeapStartGPU();
            distance = instance_->descriptorHeaps_["SRV_CBV_UAV"]->GetDescriptorSize() * index;

        } else if(heapType == HEAP_TYPE::RTV){
            heapStart = instance_->descriptorHeaps_["RTV"]->GetHeapStartGPU();
            distance = instance_->descriptorHeaps_["RTV"]->GetDescriptorSize() * index;

        } else if(heapType == HEAP_TYPE::DSV){
            heapStart = instance_->descriptorHeaps_["DSV"]->GetHeapStartGPU();
            distance = instance_->descriptorHeaps_["DSV"]->GetDescriptorSize() * index;

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
    /// <param name="textureName"></param>
    /// <returns></returns>
    D3D12_GPU_DESCRIPTOR_HANDLE ViewManager::GetHandleGPU(HEAP_TYPE heapType, const std::string& viewName){

        uint32_t distance = 0;

        if(heapType == HEAP_TYPE::SRV_CBV_UAV){
            distance = instance_->handles_[(uint32_t)HEAP_TYPE::SRV_CBV_UAV][viewName];

        } else if(heapType == HEAP_TYPE::RTV){
            distance = instance_->handles_[(uint32_t)HEAP_TYPE::RTV][viewName];

        } else if(heapType == HEAP_TYPE::DSV){
            distance = instance_->handles_[(uint32_t)HEAP_TYPE::DSV][viewName];

        } else{
            assert(false);
        }

        return GetHandleGPU(heapType, distance);
    }



    /// <summary>
    /// 
    /// </summary>
    /// <param name="heapType"></param>
    /// <returns></returns>
    ID3D12DescriptorHeap* ViewManager::GetHeap(HEAP_TYPE heapType){

        if(heapType == HEAP_TYPE::SRV_CBV_UAV){
            return instance_->descriptorHeaps_["SRV_CBV_UAV"]->GetHeap();

        } else if(heapType == HEAP_TYPE::RTV){
            return instance_->descriptorHeaps_["RTV"]->GetHeap();

        } else if(heapType == HEAP_TYPE::DSV){
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
    uint32_t ViewManager::GetDescriptorSize(HEAP_TYPE heapType){

        if(heapType == HEAP_TYPE::SRV_CBV_UAV){
            return instance_->descriptorHeaps_["SRV_CBV_UAV"]->GetDescriptorSize();

        } else if(heapType == HEAP_TYPE::RTV){
            return instance_->descriptorHeaps_["RTV"]->GetDescriptorSize();

        } else if(heapType == HEAP_TYPE::DSV){
            return instance_->descriptorHeaps_["DSV"]->GetDescriptorSize();

        } else{
            assert(false);
            return 0;
        }
    }

    /// <summary>
    /// viewをアンロードする関数
    /// </summary>
    /// <param name="heapType"></param>
    /// <param name="index"></param>
    void ViewManager::UnloadView(HEAP_TYPE heapType, uint32_t index){

        if(!instance_){ return; }

        if(heapType == HEAP_TYPE::SRV_CBV_UAV){
            instance_->descriptorHeaps_["SRV_CBV_UAV"]->UnloadView(index);
            return;
        } else if(heapType == HEAP_TYPE::RTV){
            instance_->descriptorHeaps_["RTV"]->UnloadView(index);
            return;
        } else if(heapType == HEAP_TYPE::DSV){
            instance_->descriptorHeaps_["DSV"]->UnloadView(index);
            return;
        }
    }
}