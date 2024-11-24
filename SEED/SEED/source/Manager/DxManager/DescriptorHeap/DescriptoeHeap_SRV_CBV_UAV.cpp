#include "DescriptoeHeap_SRV_CBV_UAV.h"

DescriptorHeap_SRV_CBV_UAV::DescriptorHeap_SRV_CBV_UAV(){
    kMaxViewCount_ = 0xfff;
    viewCount_ = 1;

    descriptorSize_ =
        DxManager::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(
            D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
        );

    // SRVのディスクリプタヒープを作成
    descriptorHeap_.Attach(CreateDescriptorHeap(
        DxManager::GetInstance()->GetDevice(),
        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,// SRV用に設定
        kMaxViewCount_,// ディスクリプタ数
        true
    ));
}


/// <summary>
/// 
/// </summary>
/// <param name="viewType"></param>
/// <param name="pResource"></param>
/// <param name="pDesc"></param>
uint32_t DescriptorHeap_SRV_CBV_UAV::CreateView(
    VIEW_TYPE viewType, ID3D12Resource* pResource, const void* pDesc
){

    D3D12_CPU_DESCRIPTOR_HANDLE handleCPU;
    handleCPU = GetCPUDescriptorHandle(descriptorHeap_.Get(), descriptorSize_, viewCount_);

    // 形式に応じたviewを作成する
    if(viewType == VIEW_TYPE::SRV){
        const D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc = static_cast<const D3D12_SHADER_RESOURCE_VIEW_DESC*>(pDesc);
        DxManager::GetInstance()->GetDevice()->CreateShaderResourceView(pResource, srvDesc, handleCPU);

    } else if(viewType == VIEW_TYPE::UAV){
        const D3D12_UNORDERED_ACCESS_VIEW_DESC* uavDesc = static_cast<const D3D12_UNORDERED_ACCESS_VIEW_DESC*>(pDesc);
        DxManager::GetInstance()->GetDevice()->CreateUnorderedAccessView(pResource, nullptr,uavDesc, handleCPU);

    } else if(viewType == VIEW_TYPE::CBV){
        const D3D12_CONSTANT_BUFFER_VIEW_DESC* cbvDesc = static_cast<const D3D12_CONSTANT_BUFFER_VIEW_DESC*>(pDesc);
        DxManager::GetInstance()->GetDevice()->CreateConstantBufferView(cbvDesc, handleCPU);
    
    } else{
        assert(false);
    }

    return viewCount_++;
}
