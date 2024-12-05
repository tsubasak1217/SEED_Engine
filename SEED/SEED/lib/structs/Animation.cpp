#include "Animation.h"
#include "ViewManager.h"

SkinCluster CreateSkinCluster(
    const ComPtr<ID3D12Device>& device, const ModelSkeleton& skeleton, const ModelData& modelData
){
    SkinCluster skinCluster;

    /////////////////////////////////////////////////////
    //                 Paletteの作成
    /////////////////////////////////////////////////////

    // Resourceの作成
    skinCluster.paletteResource = CreateBufferResource(device.Get(), sizeof(WellForGPU) * skeleton.joints.size());
    WellForGPU* mappedPalette = nullptr;
    skinCluster.paletteResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedPalette));
    skinCluster.mappedPalette = { mappedPalette, skeleton.joints.size() };

    // bufferにSRVの作成。(StructuredBufferで参照できるようにするため)
    D3D12_SHADER_RESOURCE_VIEW_DESC paletteSrvDesc = {};
    paletteSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
    paletteSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    paletteSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    paletteSrvDesc.Buffer.FirstElement = 0;
    paletteSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
    paletteSrvDesc.Buffer.NumElements = skeleton.joints.size();
    paletteSrvDesc.Buffer.StructureByteStride = sizeof(WellForGPU);
    ViewManager::CreateView(VIEW_TYPE::SRV, skinCluster.influenceResource.Get(), &paletteSrvDesc, "skinClusterPalette");

    /////////////////////////////////////////////////////
    //                 Influenceの作成
    /////////////////////////////////////////////////////

    int32_t modelVertexCount = 0;
    for(const auto& mesh : modelData.meshes){
        modelVertexCount += mesh.vertices.size();
    }

    // Resourceの作成
    skinCluster.influenceResource = CreateBufferResource(device.Get(), sizeof(VertexInfluence) * modelVertexCount);
    VertexInfluence* mappedInfluences = nullptr;
    skinCluster.influenceResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedInfluences));
    std::memset(mappedInfluences, 0, sizeof(VertexInfluence) * modelVertexCount);// 0で初期化
    skinCluster.mappedInfluences = { mappedInfluences, modelVertexCount };

    // Influence用のVBVの作成
    skinCluster.influenceBufferView.BufferLocation = skinCluster.influenceResource->GetGPUVirtualAddress();
    skinCluster.influenceBufferView.SizeInBytes = UINT(sizeof(VertexInfluence) * modelVertexCount);
    skinCluster.influenceBufferView.StrideInBytes = sizeof(VertexInfluence);

    // InverrseBindPoseMatrixを格納する場所を作成し、単位行列で初期化
    skinCluster.inverseBindPoseMatrices.resize(skeleton.joints.size(), IdentityMat4());

    // modelDataを解析して、Influenceを作成
}
