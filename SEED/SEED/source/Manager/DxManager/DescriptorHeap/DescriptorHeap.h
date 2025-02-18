#pragma once
#include <cstdint>
#include <SEED/Source/Manager/DxManager/DxManager.h>
#include <SEED/Lib/Functions/MyFunc/DxFunc.h>
#include <SEED/Source/Manager/DxManager/DescriptorHeap/ViewManager_Enum.h>


class DescriptorHeap{
public:
    DescriptorHeap() = default;
    virtual ~DescriptorHeap(){};

public:
    virtual uint32_t CreateView(VIEW_TYPE viewType,ID3D12Resource* pResource, const void* pDesc);
    ID3D12DescriptorHeap* GetHeap(){ return descriptorHeap_.Get(); }
    uint32_t GetDescriptorSize(){ return descriptorSize_; };
    D3D12_CPU_DESCRIPTOR_HANDLE GetHeapStartCPU();
    D3D12_GPU_DESCRIPTOR_HANDLE GetHeapStartGPU();

protected:
    uint32_t kMaxViewCount_;
    uint32_t viewCount_ = 0;
    uint32_t descriptorSize_;

protected:
    ComPtr<ID3D12DescriptorHeap> descriptorHeap_;
};