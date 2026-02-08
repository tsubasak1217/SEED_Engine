#pragma once
#include <SEED/Source/Manager/DxManager/DescriptorHeap/DescriptorHeap.h>

namespace SEED{
    /// <summary>
    /// SRV・UAV・CBV用のディスクリプタヒープ
    /// </summary>
    class DescriptorHeap_SRV_CBV_UAV : public DescriptorHeap{
    public:
        DescriptorHeap_SRV_CBV_UAV();
        ~DescriptorHeap_SRV_CBV_UAV()override{};

    public:
        uint32_t CreateView(VIEW_TYPE viewType, ID3D12Resource* pResource, const void* pDesc)override;
    };
}