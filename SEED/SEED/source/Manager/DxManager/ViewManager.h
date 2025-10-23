#pragma once
#include <unordered_map>
#include <string>
#include <SEED/Source/Manager/DxManager/DescriptorHeap/ViewManager_Enum.h>
#include <SEED/Source/Manager/DxManager/DxManager.h>

class DescriptorHeap;

/// <summary>
/// viewの作成や管理を行うクラス
/// </summary>
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
    static void Finalize();
    static ViewManager* GetInstance();

public:

    void Initialize();
    static uint32_t CreateView(
        VIEW_TYPE viewType, ID3D12Resource* pResource, 
        const void* pDesc,const std::string& viewName
    );

public:
    static int32_t GetTextureHandle(const std::string& textureName);
    static D3D12_CPU_DESCRIPTOR_HANDLE GetHandleCPU(HEAP_TYPE heapType, uint32_t index);
    static D3D12_CPU_DESCRIPTOR_HANDLE GetHandleCPU(HEAP_TYPE heapType, const std::string& viewName);
    static D3D12_GPU_DESCRIPTOR_HANDLE GetHandleGPU(HEAP_TYPE heapType, uint32_t index);
    static D3D12_GPU_DESCRIPTOR_HANDLE GetHandleGPU(HEAP_TYPE heapType, const std::string& viewName);
    static ID3D12DescriptorHeap* GetHeap(HEAP_TYPE heapType);
    static uint32_t GetDescriptorSize(HEAP_TYPE heapType);
    static void UnloadView(HEAP_TYPE heapType, uint32_t index);

private:
    std::unordered_map<std::string,std::unique_ptr<DescriptorHeap>>descriptorHeaps_;
    std::unordered_map<std::string, uint32_t>handles_[(int32_t)HEAP_TYPE::CountOfHeapType];
};