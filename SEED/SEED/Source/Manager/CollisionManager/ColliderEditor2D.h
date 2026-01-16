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
#include <SEED/Source/Basic/Collision/2D/Collider2D.h>

namespace SEED{
    // 前方宣言
    class GameObject2D;

    /// <summary>
    /// コライダーの編集クラス(2D)
    /// </summary>
    class ColliderEditor2D{
        friend class Collision2DComponent;
    private:// 基本関数
        ColliderEditor2D() = default;
    public:
        ColliderEditor2D(const std::string& className, GameObject2D* parent);
        ~ColliderEditor2D();

    public:// 編集・ファイル操作関数
        void Edit();
        static void LoadColliders(const std::string& fileName, GameObject2D* parentObject, std::vector<std::unique_ptr<Collider2D>>* pColliderArray);

    private:
        void AddColliderOnGUI();
        bool DeleteColliderOnGUI(uint32_t index);
        void OutputOnGUI();
        void InputOnGUI();

        static void LoadColliderData(const std::string fileName);
        static std::vector<Collider2D*> LoadColliderData(const nlohmann::json& json);
        void OutputToJson();
        void LoadFromJson(const std::string& fileName);

    public:// コライダー関連
        void HandOverColliders();

    public:// アクセッサ
        void SetParentMat(const Matrix3x3* parentMat){ parentMat_ = parentMat; }

    private:
        std::string className_;
        ColliderType2D addColliderType_ = ColliderType2D::Circle;
        GameObject2D* parentObject_ = nullptr;
        const Matrix3x3* parentMat_ = nullptr;
        std::vector<std::unique_ptr<Collider2D>>colliders_;

        // ロードしたコライダーのデータ
        inline static std::unordered_map<std::string, std::vector<std::unique_ptr<Collider2D>>>colliderData_{};
        inline static std::vector<std::string> colliderFileNames_{};
        int32_t selectedColliderIndex_ = 0;
    };
}