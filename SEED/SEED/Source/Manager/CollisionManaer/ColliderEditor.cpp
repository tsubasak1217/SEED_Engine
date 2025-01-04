#include "ColliderEditor.h"
#include "Collision/Collider_AABB.h"
#include "Collision/Collider_Sphere.h"
#include "Collision/Collider_AABB.h"
#include "Collision/Collider_OBB.h"
#include "Collision/Collider_Line.h"
#include "Collision/Collider_Capsule.h"
#include "Collision/Collider_Plane.h"

////////////////////////////////////////////////////////////
// コンストラクタ ・ デストラクタ
////////////////////////////////////////////////////////////
ColliderEditor::ColliderEditor(const std::string& className, const Matrix4x4* parentMat){
    className_ = className;
    parentMat_ = parentMat;
}

ColliderEditor::~ColliderEditor(){}

////////////////////////////////////////////////////////////
// コライダーの編集
////////////////////////////////////////////////////////////
void ColliderEditor::Edit(){
#ifdef _DEBUG

    std::string headerName = "ColliderEditor( " + className_ + " )";
    if(ImGui::CollapsingHeader(headerName.c_str())){
        ImGui::Indent();

        // コライダーの追加
        AddCollider();

        // 各コライダーの編集
        for(int i = 0; i < colliders_.size(); i++){
            std::string colliderName = "Collider" + std::to_string(i);
            colliders_[i]->Edit(colliderName);
        }

        ImGui::Unindent();
    }

#endif // _DEBUG
}

void ColliderEditor::AddCollider(){
#ifdef _DEBUG
    // 追加するコライダーの形状を選択
    ImGui::Combo(
        "blendMode", (int*)&addColliderType_,
        "Sphere\0AABB\0OBB\0Line\0Capsule\0Plane\0"
    );

    if(ImGui::Button("Add Collider")){
        switch(addColliderType_)
        {
        case ColliderType::Sphere:
            colliders_.push_back(std::make_unique<Collider_Sphere>());
            break;
        case ColliderType::AABB:
            colliders_.push_back(std::make_unique<Collider_AABB>());
            break;
        case ColliderType::OBB:
            colliders_.push_back(std::make_unique<Collider_OBB>());
            break;
        case ColliderType::Line:
            colliders_.push_back(std::make_unique<Collider_Line>());
            break;
        case ColliderType::Capsule:
            colliders_.push_back(std::make_unique<Collider_Capsule>());
            break;
        case ColliderType::Plane:
            colliders_.push_back(std::make_unique<Collider_Plane>());
            break;
        default:
            break;
        }

        // 行列に親子付け
        colliders_.back()->SetParentMatrix(parentMat_);
    }

#endif // _DEBUG
}

void ColliderEditor::OutputToJson(){}
