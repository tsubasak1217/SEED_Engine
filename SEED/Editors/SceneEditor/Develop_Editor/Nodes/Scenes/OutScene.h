#pragma once
#include "BaseNode.h"
#include "Vector4.h"

class OutScene : public BaseNode{

public:
    TransitionType type_;
    float time_;
    Vector4 color_;
};