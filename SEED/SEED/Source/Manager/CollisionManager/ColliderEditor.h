#pragma once
// stl
#include <vector>
#include <string>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <memory>
// json
#include <json.hpp>
// local
#include <SEED/Source/Manager/ImGuiManager/ImGuiManager.h>
#include <SEED/Lib/Tensor/Vector3.h>
#include <SEED/Source/Basic/Collision/3D/Collider.h>

class GameObject;

/// <summary>
/// コライダーの編集クラス(3D)
/// </summary>
class ColliderEditor{
    friend class Collision3DComponent;
private:// 基本関数
    ColliderEditor() = default;
public:
    ColliderEditor(const std::string& className, GameObject* parent);
    ~ColliderEditor();

public:// 編集・ファイル操作関数
    void Edit();
    static void LoadColliders(const std::string& fileName, GameObject* parentObject, std::vector<std::unique_ptr<Collider>>* pColliderArray);

private:
    void AddColliderOnGUI();
    bool DeleteColliderOnGUI(uint32_t index);
    void OutputOnGUI();
    void InputOnGUI();

    static void LoadColliderData(const std::string fileName);
    static std::vector<Collider*> LoadColliderData(const nlohmann::json& json);
    void OutputToJson();
    void LoadFromJson(const std::string& fileName);

public:// コライダー関連
    void HandOverColliders();

public:// アクセッサ
    void SetParentMat(const Matrix4x4* parentMat){ parentMat_ = parentMat; }

private:
    std::string className_;
    ColliderType addColliderType_ = ColliderType::OBB;
    GameObject* parentObject_ = nullptr;
    const Matrix4x4* parentMat_ = nullptr;
    std::vector<std::unique_ptr<Collider>>colliders_;

    // ロードしたコライダーのデータ
    inline static std::unordered_map<std::string, std::vector<std::unique_ptr<Collider>>>colliderData_{};
    inline static std::vector<std::string> colliderFileNames_{};
    int32_t selectedColliderIndex_ = 0;
};