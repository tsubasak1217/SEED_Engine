#include "DescriptorHeap_DSV.h"

namespace SEED{
    DescriptorHeap_DSV::DescriptorHeap_DSV(){
        kMaxViewCount_ = 0xff;
        descriptorSize_ =
            DxManager::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(
                D3D12_DESCRIPTOR_HEAP_TYPE_DSV
            );

        // DSV用のヒ－プはヒープタイプが違うので別途作る
        descriptorHeap_ = Methods::DxFunc::CreateDescriptorHeap(
            DxManager::GetInstance()->GetDevice(),
            D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
            kMaxViewCount_,
            false
        );

        // フリーリストに全てのインデックスを追加
        for(uint32_t i = 0; i < kMaxViewCount_; ++i){
            freeIndices_.push(i);
        }
    }


    /// <summary>
    /// 
    /// </summary>
    /// <param name="viewType"></param>
    /// <param name="pResource"></param>
    /// <param name="pDesc"></param>
    uint32_t DescriptorHeap_DSV::CreateView(VIEW_TYPE viewType, ID3D12Resource* pResource, const void* pDesc){

        D3D12_CPU_DESCRIPTOR_HANDLE handleCPU;
        handleCPU = Methods::DxFunc::GetCPUDescriptorHandle(descriptorHeap_.Get(), descriptorSize_, freeIndices_.front());

        if(viewType == VIEW_TYPE::DSV){
            const D3D12_DEPTH_STENCIL_VIEW_DESC* dsvDesc = static_cast<const D3D12_DEPTH_STENCIL_VIEW_DESC*>(pDesc);
            DxManager::GetInstance()->GetDevice()->CreateDepthStencilView(pResource, dsvDesc, handleCPU);

        } else{
            assert(false);
        }

        viewCount_++;
        uint32_t index = freeIndices_.front();
        freeIndices_.pop();
        return index;
    }
}