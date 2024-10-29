#include "DescriptorHeap.h"

uint32_t DescriptorHeap::CreateView(VIEW_TYPE viewType,ID3D12Resource* pResource,const void* pDesc){
    viewType;
    pResource;
    pDesc;

    return 0;
}


D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetHeapStart(){
    return GetCPUDescriptorHandle(descriptorHeap_.Get(), descriptorSize_, 0);
}
