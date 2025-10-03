#pragma once
#include <vector>
#include <SEED/Source/Basic/Object/GameObject.h>
#include <SEED/Source/Basic/Object/GameObject2D.h>

struct LoadObjectData{
    std::vector<GameObject*> objects3D_;
    std::vector<GameObject2D*> objects2D_;
};