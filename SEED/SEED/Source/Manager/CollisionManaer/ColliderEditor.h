#pragma once
#include <vector>
#include <json.hpp>
#include <string>
#include <fstream>
#include <iostream>
#include "ImGuiManager.h"
#include "Vector3.h"
#include "Collision/Collider.h"

class ColliderEditor{

private:// 基本関数
    ColliderEditor() = default;
public:
    ColliderEditor(const std::string& className, const Matrix4x4* parentMat);
    ~ColliderEditor();

public:// 編集・ファイル操作関数
    void Edit();
private:
    void AddCollider();
    void OutputToJson();

public:// アクセッサ
    void SetParentMat(const Matrix4x4* parentMat){ parentMat_ = parentMat; }

private:
    std::string className_;
    ColliderType addColliderType_ = ColliderType::OBB;
    const Matrix4x4* parentMat_ = nullptr;
    std::vector<std::unique_ptr<Collider>>colliders_;
};