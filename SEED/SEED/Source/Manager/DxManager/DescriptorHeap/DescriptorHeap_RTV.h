#pragma once
#include <SEED/Source/Manager/DxManager/DescriptorHeap/DescriptorHeap.h>

namespace SEED{
    /// <summary>
    /// RenderTargetView用のディスクリプターヒープ
    /// </summary>
    class DescriptorHeap_RTV : public DescriptorHeap{

    public:
        DescriptorHeap_RTV();
        ~DescriptorHeap_RTV()override{};

    public:
        uint32_t CreateView(VIEW_TYPE viewType, ID3D12Resource* pResource, const void* pDesc)override;
    };
}