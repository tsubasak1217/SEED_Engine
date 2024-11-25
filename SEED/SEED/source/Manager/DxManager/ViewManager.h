#pragma once
#include <unordered_map>
#include <string>
#include "ViewManager_Enum.h"
#include "DxManager.h"

class DescriptorHeap;

class ViewManager{

private:
    // privateコンストラクタ
    ViewManager() = default;

    // コピー禁止
    ViewManager(const ViewManager&) = delete;
    void operator=(const ViewManager&) = delete;

    // インスタンス
    static ViewManager* instance_;

public:

    ~ViewManager();
    static ViewManager* GetInstance();

public:

    void Initialize();
    static uint32_t CreateView(
        VIEW_TYPE viewType, ID3D12Resource* pResource, 
        const void* pDesc,const std::string& viewName
    );

public:
    static int32_t GetTextureHandle(const std::string& textureName);
    static D3D12_CPU_DESCRIPTOR_HANDLE GetHandleCPU(DESCRIPTOR_HEAP_TYPE heapType, uint32_t index);
    static D3D12_CPU_DESCRIPTOR_HANDLE GetHandleCPU(DESCRIPTOR_HEAP_TYPE heapType, const std::string& viewName);
    static D3D12_GPU_DESCRIPTOR_HANDLE GetHandleGPU(DESCRIPTOR_HEAP_TYPE heapType, uint32_t index);
    static D3D12_GPU_DESCRIPTOR_HANDLE GetHandleGPU(DESCRIPTOR_HEAP_TYPE heapType, const std::string& viewName);
    static ComPtr<ID3D12DescriptorHeap> GetHeap(DESCRIPTOR_HEAP_TYPE heapType);
    static uint32_t GetDescriptorSize(DESCRIPTOR_HEAP_TYPE heapType);

private:
    std::unordered_map<std::string,std::unique_ptr<DescriptorHeap>>descriptorHeaps_;
    std::unordered_map<std::string, uint32_t>handles_[(int32_t)DESCRIPTOR_HEAP_TYPE::CountOfHeapType];
};