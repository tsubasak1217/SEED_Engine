#pragma once
#include <SEED/Lib/Structs/Transform.h>
#include <SEED/Lib/Tensor/Matrix4x4.h>
#include <SEED/Lib/Tensor/Quaternion.h>
#include <SEED/Lib/Tensor/Vector3.h>
#include <vector>
#include <array>
#include <unordered_map>
#include <string>
#include <optional>
#include <span>

#include <d3dx12.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

struct ModelData;

//////////////////////////////////////////////////////////////////////////////////////////////
//                                  基本のアニメーション構造体                                   //
//////////////////////////////////////////////////////////////////////////////////////////////

/*--------- キーフレーム ----------*/
template<typename T>
struct Keyframe{
    T value;
    float time;
};

using KeyframeVec3 = Keyframe<Vector3>;
using KeyframeQuaternion = Keyframe<Quaternion>;

/*------ アニメーションカーブ ------*/
template<typename T>
struct AnimationCurve{
    std::vector<Keyframe<T>> keyframes;
};

/*--- アニメーション(ノードごと) ---*/
struct NodeAnimation{
    AnimationCurve<Vector3> translate;
    AnimationCurve<Quaternion> rotate;
    AnimationCurve<Vector3> scale;
};

/*------ アニメーション(全体) ------*/
struct ModelAnimation{
    float duration;// アニメーションの長さ(秒)
    std::unordered_map<std::string, NodeAnimation> nodeAnimations;
};

//////////////////////////////////////////////////////////////////////////////////////////////
//                                     スキニング関連の構造体                                   //
//////////////////////////////////////////////////////////////////////////////////////////////

struct ModelJoint{
    Transform transform;// トランスフォーム情報
    Matrix4x4 localMatrix;// ローカル行列
    Matrix4x4 skeletonSpaceMatrix;// スケルトン行列
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
    // 逆バインドポーズ行列
    std::vector<Matrix4x4> inverseBindPoseMatrices;
    // スキニング用のパレット
    std::vector<WellForGPU> palette;
};

/*----------------- animation用関数 --------------------*/
Vector3 CalcMomentValue(const std::vector<KeyframeVec3>& keyFrames, float time);
Quaternion CalcMomentValue(const std::vector<KeyframeQuaternion>& keyFrames, float time);