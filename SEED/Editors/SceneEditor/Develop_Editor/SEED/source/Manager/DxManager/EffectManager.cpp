// local
#include <DxManager.h>
#include <EffectManager.h>
#include <SEED.h>
#include <MyMath.h>
#include <CS_Buffers.h>

// external
#include <cmath>

EffectManager::EffectManager(DxManager* pDxManager)
{
    pDxManager_ = pDxManager;
    pSEED_ = SEED::GetInstance();
}

EffectManager::~EffectManager(){}
void EffectManager::Initialize(){}
void EffectManager::Finalize(){}


void EffectManager::TransfarToCS()
{

    pDxManager_->TransitionResourceState(
        pDxManager_->offScreenResource.Get(),
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE
    );

    pDxManager_->TransitionResourceState(
        pDxManager_->depthStencilResource.Get(),
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE
    );


    // ========================== RootSignatureを設定 ============================ //

    pDxManager_->commandList->SetComputeRootSignature(pDxManager_->csRootSignature.Get());

    // =============================== CBVを設定 ================================= //

    Blur_CS_ConstantBuffer* blurCBVData;
    pDxManager_->CS_ConstantBuffer->Map(0, nullptr, reinterpret_cast<void**>(&blurCBVData));
    blurCBVData[0].resolutionRate = pDxManager_->resolutionRate_;

    pDxManager_->commandList->SetComputeRootConstantBufferView(
        4, pDxManager_->CS_ConstantBuffer->GetGPUVirtualAddress()
    );

    // ========================== 送信する画像の決定ゾーン ============================ //

    // SRVHeapを設定
    ID3D12DescriptorHeap* descriptorHeaps[] = { pDxManager_->SRV_UAV_DescriptorHeap.Get() };
    pDxManager_->commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

    // まずは通常のスクリーンショットを転送する
    D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle;
    srvGpuHandle = GetGPUDescriptorHandle(
        pDxManager_->SRV_UAV_DescriptorHeap.Get(), pDxManager_->descriptorSizeSRV_UAV,
        pDxManager_->systemTextures_["offScreenTexture"] + 1
    );

    // 転送
    pDxManager_->commandList->SetComputeRootDescriptorTable(0, srvGpuHandle);


    // 次に書き込み用のリソースを転送する
    D3D12_GPU_DESCRIPTOR_HANDLE uavGpuHandle;
    uavGpuHandle = GetGPUDescriptorHandle(
        pDxManager_->SRV_UAV_DescriptorHeap.Get(), pDxManager_->descriptorSizeSRV_UAV, 
        pDxManager_->systemTextures_["blurredTexture_UAV"] + 1
    );

    // 転送
    pDxManager_->commandList->SetComputeRootDescriptorTable(1, uavGpuHandle);


    // DepthStencilResourceのSRVを転送する
    srvGpuHandle = GetGPUDescriptorHandle(
        pDxManager_->SRV_UAV_DescriptorHeap.Get(), pDxManager_->descriptorSizeSRV_UAV,
        pDxManager_->systemTextures_["depthStencilSRV"] + 1
    );

    // 転送
    pDxManager_->commandList->SetComputeRootDescriptorTable(2, srvGpuHandle);



    // 深度情報書き込み用のテクスチャを転送する
    srvGpuHandle = GetGPUDescriptorHandle(
        pDxManager_->SRV_UAV_DescriptorHeap.Get(), pDxManager_->descriptorSizeSRV_UAV,
        pDxManager_->systemTextures_["depthTexture_UAV"] + 1
    );

    // 転送
    pDxManager_->commandList->SetComputeRootDescriptorTable(3, uavGpuHandle);

    // =============================================================================//


    pDxManager_->commandList->SetPipelineState(pDxManager_->csPipelineState.Get());

    UINT dispatchX = (SEED::GetInstance()->kClientWidth_ + 15) / 16;
    UINT dispatchY = (SEED::GetInstance()->kClientHeight_ + 15) / 16;

    pDxManager_->commandList->Dispatch(dispatchX,dispatchY,1);

    //
    pDxManager_->TransitionResourceState(
        pDxManager_->offScreenResource.Get(),
        D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
        D3D12_RESOURCE_STATE_RENDER_TARGET
    );

    pDxManager_->TransitionResourceState(
        pDxManager_->depthStencilResource.Get(),
        D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
        D3D12_RESOURCE_STATE_DEPTH_WRITE
    );
}

// デプスバッファからリードバックバッファへコピー
void EffectManager::CopyDepthStencil(
    ID3D12Resource* dstResource,
    ID3D12Resource* sourceResource,
    D3D12_RESOURCE_STATES currentState_source,
    D3D12_RESOURCE_STATES currentState_dst
){

    D3D12_RESOURCE_DESC depthDesc = sourceResource->GetDesc();


    // コピー先の情報記述
    D3D12_TEXTURE_COPY_LOCATION dstLocation = {};
    dstLocation.pResource = dstResource;
    dstLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
    dstLocation.PlacedFootprint.Offset = 0;
    dstLocation.PlacedFootprint.Footprint.Format = DXGI_FORMAT_R32_TYPELESS;
    dstLocation.PlacedFootprint.Footprint.Width = (UINT)depthDesc.Width;
    dstLocation.PlacedFootprint.Footprint.Height = (UINT)depthDesc.Height;
    dstLocation.PlacedFootprint.Footprint.Depth = 1;
    dstLocation.PlacedFootprint.Footprint.RowPitch = (uint32_t)depthDesc.Width * sizeof(float);

    // コピー元の情報記述
    D3D12_TEXTURE_COPY_LOCATION srcLocation = {};
    srcLocation.pResource = sourceResource;
    srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    srcLocation.SubresourceIndex = 0;

    /*--------------------------- リソースをコピー可能状態にする----------------------------------*/

    pDxManager_->TransitionResourceState(
        sourceResource,
        currentState_source,
        D3D12_RESOURCE_STATE_COPY_SOURCE
    );

    pDxManager_->TransitionResourceState(
        dstResource,
        currentState_dst,
        D3D12_RESOURCE_STATE_COPY_DEST
    );

    /*-----------------------------------------  コピー  --------------------------------------*/

    pDxManager_->commandList->CopyTextureRegion(&dstLocation, 0, 0, 0, &srcLocation, nullptr);


    /*---------------------------------- リソースの状態を戻す------------------------------------*/

    // リソースの状態を戻す
    pDxManager_->TransitionResourceState(
        dstResource,
        D3D12_RESOURCE_STATE_COPY_SOURCE,
        currentState_dst
    );

    pDxManager_->TransitionResourceState(
        sourceResource,
        currentState_source,
        D3D12_RESOURCE_STATE_COPY_SOURCE
    );
}