#pragma once
#include "Keyframe.h"
#include <vector>

template<typename T>
struct AnimationCurve{
    std::vector<Keyframe<T>> keyframes;
};

struct NodeAnimation{
    AnimationCurve<Vector3> translate;
    AnimationCurve<Quaternion> rotate;
    AnimationCurve<Vector3> scale;
};

struct Animation{
    std::string name;// アニメーション名
    float duration;// アニメーションの長さ(秒)
    std::vector<NodeAnimation> nodeAnimations;
};