#pragma once
#include "Vector3.h"
#include "Quaternion.h"

template<typename T>
struct Keyframe{
    T value;
    float time;
};

using KeyframeVector3 = Keyframe<Vector3>;
using KeyframeQuaternion = Keyframe<Quaternion>;