#pragma once
#include "Keyframe.h"
#include <vector>
#include <unordered_map>
#include <string>

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
    float duration;// アニメーションの長さ(秒)
    std::unordered_map<std::string,NodeAnimation> nodeAnimations;
};