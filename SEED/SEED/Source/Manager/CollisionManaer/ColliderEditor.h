#pragma once
#include <vector>
#include <json.hpp>
#include <string>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include "ImGuiManager.h"
#include "Vector3.h"
#include "Collision/Collider.h"

class BaseObject;

class ColliderEditor{

private:// 基本関数
    ColliderEditor() = default;
public:
    ColliderEditor(const std::string& className, const Matrix4x4* parentMat);
    ~ColliderEditor();

public:// 編集・ファイル操作関数
    void Edit();
    static void LoadColliders(const std::string& fileName, BaseObject* parentObject, std::vector<std::unique_ptr<Collider>>* pColliderArray);

private:
    void AddColliderOnGUI();
    bool DeleteColliderOnGUI(uint32_t index);
    void OutputOnGUI();
    void InputOnGUI();

    static void LoadColliderData(const std::string fileName);
    void OutputToJson();
    void LoadFromJson(const std::string& fileName);

public:// コライダー関連
    void HandOverColliders();

public:// アクセッサ
    void SetParentMat(const Matrix4x4* parentMat){ parentMat_ = parentMat; }

private:
    std::string className_;
    ColliderType addColliderType_ = ColliderType::OBB;
    const Matrix4x4* parentMat_ = nullptr;
    std::vector<std::unique_ptr<Collider>>colliders_;
    // ロードしたコライダーのデータ
    static std::unordered_map<std::string, std::vector<std::unique_ptr<Collider>>>colliderData_;
};