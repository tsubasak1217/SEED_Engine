#pragma once
#include "BaseNode.h"
#include "Vector4.h"

class InScene : public BaseNode{

public:
    TransitionType type_;
    float time_;
    Vector4 color_;
};