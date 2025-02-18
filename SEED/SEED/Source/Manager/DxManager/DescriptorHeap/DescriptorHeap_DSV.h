#pragma once
#include <SEED/Source/Manager/DxManager/DescriptorHeap/DescriptorHeap.h>

class DescriptorHeap_DSV : public DescriptorHeap{

public:
    DescriptorHeap_DSV();
    ~DescriptorHeap_DSV()override{};

public:
    uint32_t CreateView(VIEW_TYPE viewType,ID3D12Resource* pResource, const void* pDesc)override;
};