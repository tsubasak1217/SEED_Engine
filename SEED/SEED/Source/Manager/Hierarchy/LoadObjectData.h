#pragma once
#include <vector>
#include <SEED/Source/Basic/Object/GameObject.h>
#include <SEED/Source/Basic/Object/GameObject2D.h>

/// <summary>
/// 読み込んだシーン上のオブジェクトをポインタ配列で格納する構造体
/// </summary>
struct LoadObjectData{
    std::vector<GameObject*> objects3D_;
    std::vector<GameObject2D*> objects2D_;
};