#include "DescriptorHeap.h"

uint32_t DescriptorHeap::CreateView(VIEW_TYPE viewType,ID3D12Resource* pResource,const void* pDesc){
    viewType;
    pResource;
    pDesc;

    return 0;
}


D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetHeapStartCPU(){
    return GetCPUDescriptorHandle(descriptorHeap_.Get(), descriptorSize_, 0);
}

D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetHeapStartGPU(){
    return GetGPUDescriptorHandle(descriptorHeap_.Get(), descriptorSize_, 0);
}
