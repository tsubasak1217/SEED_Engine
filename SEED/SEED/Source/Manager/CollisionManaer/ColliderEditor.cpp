#include "ColliderEditor.h"
#include "CollisionManager.h"
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
        ImGui::Separator();
        ImGui::Dummy({ 0.0f,10.0f });


        // 各コライダーの編集
        for(int i = 0; i < colliders_.size(); i++){
            // ヘッダー名
            std::string colliderID = "##" + std::to_string(colliders_[i]->GetColliderID());
            std::string colliderName = "Collider( " + std::to_string(i) + " )" + colliderID;

            // ヘッダーの表示
            if(ImGui::CollapsingHeader(colliderName.c_str())){
                ImGui::Indent();

                // コライダーの削除
                if(ImGui::Button("Delete Collider")){
                    ImGui::OpenPopup("Delete Collider");
                }


                // コライダーの編集
                colliders_[i]->Edit();


                // 削除の確認
                if(ImGui::BeginPopupModal("Delete Collider", NULL, ImGuiWindowFlags_AlwaysAutoResize)){
                    ImGui::Text("Are you sure you want to delete this collider?");
                    ImGui::Separator();

                    // 削除ボタン
                    if(ImGui::Button("OK", ImVec2(120, 0))){
                        colliders_.erase(colliders_.begin() + i);
                        ImGui::CloseCurrentPopup();
                        ImGui::EndPopup();
                        break;// 配列数が合わなくなるので一旦抜ける
                    }
                    ImGui::SameLine();
                    // キャンセルボタン
                    if(ImGui::Button("Cancel", ImVec2(120, 0))){
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndPopup();
                }

                ImGui::Unindent();

                // アニメーションの編集
                if(colliders_[i]->isAnimation_){
                    colliders_[i]->EditAnimation();
                }

                ImGui::Separator();
            }
        }

        // コライダーを渡す
        HandOverColliders();

        ImGui::Unindent();
    }

#endif // _DEBUG
}

void ColliderEditor::AddCollider(){
#ifdef _DEBUG
    // 追加するコライダーの形状を選択
    ImGui::Combo(
        "ColliderType", (int*)&addColliderType_,
        "Sphere\0AABB\0OBB\0Line\0Capsule\0Plane\0"
    );

    if(ImGui::Button("Add Collider")){
        switch(addColliderType_)
        {
        case ColliderType::Sphere:
        {
            colliders_.push_back(std::make_unique<Collider_Sphere>());
            Collider_Sphere* sphere = dynamic_cast<Collider_Sphere*>(colliders_.back().get());
            sphere->SetRadius(1.0f);
            break;
        }
        case ColliderType::AABB:
        {
            colliders_.push_back(std::make_unique<Collider_AABB>());
            Collider_AABB* aabb = dynamic_cast<Collider_AABB*>(colliders_.back().get());
            aabb->SetSize(Vector3(1.0f, 1.0f, 1.0f));
            break;
        }
        case ColliderType::OBB:
        {
            colliders_.push_back(std::make_unique<Collider_OBB>());
            Collider_OBB* obb = dynamic_cast<Collider_OBB*>(colliders_.back().get());
            obb->SetSize(Vector3(1.0f, 1.0f, 1.0f));
            break;
        }
        case ColliderType::Line:
        {
            colliders_.push_back(std::make_unique<Collider_Line>());
            Collider_Line* line = dynamic_cast<Collider_Line*>(colliders_.back().get());
            line->SetLocalPosition({ 0.0f,0.0f,0.0f }, { 0.0f,1.0f,0.0f });
            break;
        }
        case ColliderType::Capsule:
        {
            colliders_.push_back(std::make_unique<Collider_Capsule>());
            Collider_Capsule* capsule = dynamic_cast<Collider_Capsule*>(colliders_.back().get());
            capsule->SetRadius(1.0f);
            capsule->SetLocalPosition({ 0.0f,0.0f,0.0f }, { 0.0f,1.0f,0.0f });
            break;
        }
        case ColliderType::Plane:
        {
            colliders_.push_back(std::make_unique<Collider_Plane>());
            Collider_Plane* plane = dynamic_cast<Collider_Plane*>(colliders_.back().get());
            plane->SetLocalVertices({ -1.0f,0.0f,-1.0f }, { 1.0f,0.0f,-1.0f }, { 1.0f,0.0f,1.0f }, { -1.0f,0.0f,1.0f });
            break;
        }
        default:
            break;
        }

        // 行列に親子付け
        colliders_.back()->SetParentMatrix(parentMat_);
    }

#endif // _DEBUG
}


////////////////////////////////////////////////////////////
// Jsonファイルへの出力
////////////////////////////////////////////////////////////
void ColliderEditor::OutputToJson(){

    nlohmann::json j;

    // コライダーの数
    for(int i = 0; i < colliders_.size(); i++){
        j["colliders"][i] = colliders_[i]->GetJsonData();
    }

    // ファイルへの書き込み
    std::ofstream ofs("Resources/jsond/Colliders" + className_ + ".json");
    ofs << j.dump(4);
    ofs.close();
}

////////////////////////////////////////////////////////////
// Jsonファイルからの読み込み
////////////////////////////////////////////////////////////
void ColliderEditor::LoadFromJson(){


}

////////////////////////////////////////////////////////////
// コライダーをCollisionManagerに渡す
////////////////////////////////////////////////////////////
void ColliderEditor::HandOverColliders(){
    // コライダーをCollisionManagerに渡す
    for(int i = 0; i < colliders_.size(); i++){
        colliders_[i]->Update();
        CollisionManager::AddCollider(colliders_[i].get());
    }
}
