#pragma once
#include <cstdint>
#include "DxManager.h"
#include "DxFunc.h"
#include "ViewManager_Enum.h"


class DescriptorHeap{
public:
    DescriptorHeap() = default;
    virtual ~DescriptorHeap(){};

public:
    virtual uint32_t CreateView(VIEW_TYPE viewType,ID3D12Resource* pResource, const void* pDesc);
    ComPtr<ID3D12DescriptorHeap> GetHeap(){ return descriptorHeap_; }
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