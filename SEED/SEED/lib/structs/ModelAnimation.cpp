#include "ModelAnimation.h"
#include "ModelData.h"
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
    paletteSrvDesc.Buffer.NumElements = (UINT)skeleton.joints.size();
    paletteSrvDesc.Buffer.StructureByteStride = sizeof(WellForGPU);
    ViewManager::CreateView(VIEW_TYPE::SRV, skinCluster.influenceResource.Get(), &paletteSrvDesc, "skinClusterPalette");

    /////////////////////////////////////////////////////
    //                 Influenceの作成
    /////////////////////////////////////////////////////

    int32_t modelVertexCount = 0;
    for(const auto& mesh : modelData.meshes){
        modelVertexCount += (int32_t)mesh.vertices.size();
    }

    // Resourceの作成
    skinCluster.influenceResource = CreateBufferResource(device.Get(), sizeof(VertexInfluence) * modelVertexCount);
    VertexInfluence* mappedInfluences = nullptr;
    skinCluster.influenceResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedInfluences));
    std::memset(mappedInfluences, 0, sizeof(VertexInfluence) * modelVertexCount);// 0で初期化
    skinCluster.mappedInfluences = { mappedInfluences, (size_t)modelVertexCount };

    // Influence用のVBVの作成
    skinCluster.influenceBufferView.BufferLocation = skinCluster.influenceResource->GetGPUVirtualAddress();
    skinCluster.influenceBufferView.SizeInBytes = UINT(sizeof(VertexInfluence) * modelVertexCount);
    skinCluster.influenceBufferView.StrideInBytes = sizeof(VertexInfluence);

    // InverrseBindPoseMatrixを格納する場所を作成し、単位行列で初期化
    skinCluster.inverseBindPoseMatrices.resize(skeleton.joints.size(), IdentityMat4());

    // modelDataを解析して、Influenceを作成
    for(const auto& jointWeight : modelData.skinClusterData){// ModelDataのSkinClusterDataを解析

        auto it = skeleton.jointMap.find(jointWeight.first);// 対象のjointが存在するか確認
        if(it == skeleton.jointMap.end()){
            continue;// jointが存在しない場合はスキップ
        }

        skinCluster.inverseBindPoseMatrices[it->second] = jointWeight.second.inverseBindPoseMatrix;// InverrseBindPoseMatrixを格納
        for(const auto& vertexWeight : jointWeight.second.vertexWeights){// 頂点ウェイトを格納

            auto& currentInfluence = skinCluster.mappedInfluences[vertexWeight.vertexIndex];// 対象の頂点ウェイトを取得

            for(uint32_t index = 0; index < kMaxInfluence; ++index){// 空いている場所に格納

                if(currentInfluence.weights[index] == 0.0f){// weight == 0 が空の状態なのでその場所にweightとindexxを格納
                    currentInfluence.weights[index] = vertexWeight.weight;
                    currentInfluence.jointIndices[index] = it->second;
                    break;
                }
            }
        }
    }

    return skinCluster;
}
