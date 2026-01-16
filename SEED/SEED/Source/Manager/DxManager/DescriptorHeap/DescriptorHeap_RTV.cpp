#include <SEED/Source/Manager/DxManager/DescriptorHeap/DescriptorHeap_RTV.h>

namespace SEED{
    DescriptorHeap_RTV::DescriptorHeap_RTV(){
        kMaxViewCount_ = 0xff;

        descriptorSize_ =
            DxManager::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(
                D3D12_DESCRIPTOR_HEAP_TYPE_RTV
            );

        // RTVのディスクリプタヒープを作成
        descriptorHeap_ = Methods::DxFunc::CreateDescriptorHeap(
            DxManager::GetInstance()->GetDevice(),
            D3D12_DESCRIPTOR_HEAP_TYPE_RTV,//レンダーターゲットビュー用に設定
            kMaxViewCount_,// ダブルバッファ用とオフスクリーンように計3つ。 多くても別に構わない
            false
        );

        // フリーリストに全てのインデックスを追加
        for(uint32_t i = 0; i < kMaxViewCount_; ++i){
            freeIndices_.push(i);
        }
    }

    uint32_t DescriptorHeap_RTV::CreateView(VIEW_TYPE viewType, ID3D12Resource* pResource, const void* pDesc){

        D3D12_CPU_DESCRIPTOR_HANDLE handleCPU;
        handleCPU = Methods::DxFunc::GetCPUDescriptorHandle(descriptorHeap_.Get(), descriptorSize_, freeIndices_.front());

        if(viewType == VIEW_TYPE::RTV){
            const D3D12_RENDER_TARGET_VIEW_DESC* rtvDesc = static_cast<const D3D12_RENDER_TARGET_VIEW_DESC*>(pDesc);
            DxManager::GetInstance()->GetDevice()->CreateRenderTargetView(pResource, rtvDesc, handleCPU);

        } else{
            assert(false);
        }

        viewCount_++;
        uint32_t index = freeIndices_.front();
        freeIndices_.pop();
        return index;
    }

}