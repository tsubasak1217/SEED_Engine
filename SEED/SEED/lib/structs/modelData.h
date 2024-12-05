#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include "VertexData.h"
#include "Material.h" 
#include "Transform.h"
#include "ModelAnimation.h"

#pragma once
#include "Keyframe.h"
#include "Transform.h"
#include "Matrix4x4.h"
#include "Vector3.h"
#include <vector>
#include <array>
#include <unordered_map>
#include <string>
#include <optional>
#include <span>

#include <DxFunc.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

struct ModelData;

//////////////////////////////////////////////////////////////////////////////////////////////
//                                  基本のアニメーション構造体                                   //
//////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct AnimationCurve{
    std::vector<Keyframe<T>> keyframes;
};

struct NodeAnimation{
    AnimationCurve<Vector3> translate;
    AnimationCurve<Quaternion> rotate;
    AnimationCurve<Vector3> scale;
};

struct ModelAnimation{
    float duration;// アニメーションの長さ(秒)
    std::unordered_map<std::string, NodeAnimation> nodeAnimations;
};

//////////////////////////////////////////////////////////////////////////////////////////////
//                                     スキニング関連の構造体                                   //
//////////////////////////////////////////////////////////////////////////////////////////////

struct ModelJoint{
    QuaternionTransform transform;// トランスフォーム情報
    Matrix4x4 localMatrix;// ローカル行列
    Matrix4x4 skeletonMatrix;// スケルトン行列
    std::string name;// ノード名
    std::vector<int32_t> children;// 子ノードのインデックスリスト
    int32_t index;// インデックス
    std::optional<int32_t> parent;// 親のインデックス,なければnull
};

struct ModelSkeleton{
    int32_t rootIndex;// ルートジョイントのインデックス
    std::unordered_map<std::string, int32_t> jointMap;// ジョイント名からインデックスの索引
    std::vector<ModelJoint> joints;// 所属しているジョイント
};

struct VertexWeightData{
    float weight;// ウェイト
    int32_t vertexIndex;// ジョイントのインデックス

};

struct JointWeightData{
    Matrix4x4 inverseBindPoseMatrix;// 逆バインド行列
    std::vector<VertexWeightData> vertexWeights;// 頂点ウェイト
};

/*-------------- GPUに情報を送る用 --------------*/

const uint8_t kMaxInfluence = 4;
struct VertexInfluence{
    std::array<float, kMaxInfluence> weights;
    std::array<int32_t, kMaxInfluence> jointIndices;
};

struct WellForGPU{
    Matrix4x4 skeletonSpaceMatrix;// 位置用
    Matrix4x4 skeletonSpaceInverceTransposeMatrix;// 法線用
};

struct SkinCluster{
    std::vector<Matrix4x4> inverseBindPoseMatrices;

    ComPtr<ID3D12Resource> influenceResource;
    D3D12_VERTEX_BUFFER_VIEW influenceBufferView;
    std::span<VertexInfluence> mappedInfluences;

    ComPtr<ID3D12Resource> paletteResource;
    std::span<WellForGPU> mappedPalette;
    std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> paletteSrvHandle;

};



// メッシュデータ構造体
struct MeshData{
    std::vector<VertexData> vertices; // 頂点データ
    std::vector<uint32_t> indices;   // インデックスデータ
    int materialIndex;               // 使用するマテリアルのインデックス
};

// モデルのノード構造体
struct ModelNode{
    QuaternionTransform transform;
    Matrix4x4 localMatrix;
    std::string name;
    std::vector<ModelNode> children;
};

// モデルデータ構造体
struct ModelData{
    std::vector<MeshData>meshes;
    std::vector<MaterialData> materials;
    std::unordered_map<std::string, ModelAnimation> animations;
    std::unordered_map<std::string, JointWeightData> skinClusterData;
    ModelSkeleton defaultSkeleton;
    ModelNode rootNode;
    std::string modelName;
};