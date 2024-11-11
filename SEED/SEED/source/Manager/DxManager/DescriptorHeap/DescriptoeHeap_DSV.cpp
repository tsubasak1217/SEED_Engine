#include "DescriptoeHeap_DSV.h"

DescriptorHeap_DSV::DescriptorHeap_DSV(){
    kMaxViewCount_ = 0xff;
    descriptorSize_ = 
        DxManager::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(
            D3D12_DESCRIPTOR_HEAP_TYPE_DSV
        );

    // DSV用のヒ－プはヒープタイプが違うので別途作る
    descriptorHeap_.Attach(CreateDescriptorHeap(
        DxManager::GetInstance()->GetDevice(),
        D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 
        kMaxViewCount_, 
        false
    ));
}


/// <summary>
/// 
/// </summary>
/// <param name="viewType"></param>
/// <param name="pResource"></param>
/// <param name="pDesc"></param>
uint32_t DescriptorHeap_DSV::CreateView(VIEW_TYPE viewType, ID3D12Resource* pResource, const void* pDesc){

    D3D12_CPU_DESCRIPTOR_HANDLE handleCPU;
    handleCPU = GetCPUDescriptorHandle(descriptorHeap_.Get(), descriptorSize_, viewCount_);

    if(viewType == VIEW_TYPE::DSV){
        const D3D12_DEPTH_STENCIL_VIEW_DESC* dsvDesc = static_cast<const D3D12_DEPTH_STENCIL_VIEW_DESC*>(pDesc);
        DxManager::GetInstance()->GetDevice()->CreateDepthStencilView(pResource, dsvDesc, handleCPU);

    } else{
        assert(false);
    }

    return viewCount_++;
}
