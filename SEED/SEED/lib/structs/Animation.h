#pragma once
#include "Keyframe.h"
#include <vector>
#include <unordered_map>
#include <string>
#include <optional>

template<typename T>
struct AnimationCurve{
    std::vector<Keyframe<T>> keyframes;
};

struct NodeAnimation{
    AnimationCurve<Vector3> translate;
    AnimationCurve<Quaternion> rotate;
    AnimationCurve<Vector3> scale;
};

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

struct Animation{
    float duration;// アニメーションの長さ(秒)
    std::unordered_map<std::string,NodeAnimation> nodeAnimations;
};