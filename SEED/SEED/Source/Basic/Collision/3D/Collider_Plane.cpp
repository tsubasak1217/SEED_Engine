#include "Collider_Plane.h"
#include <SEED/Source/Basic/Object/GameObject.h>
#include <SEED/Source/Basic/Collision/3D/Collider_Sphere.h>
#include <SEED/Source/Basic/Collision/3D/Collider_AABB.h>
#include <SEED/Source/Basic/Collision/3D/Collider_OBB.h>
#include <SEED/Source/Basic/Collision/3D/Collider_Line.h>
#include <SEED/Source/Basic/Collision/3D/Collider_Capsule.h>
#include <SEED/Source/Manager/CollisionManager/Collision.h>
#include <SEED/Source/SEED.h>

//////////////////////////////////////////////////////////////////
// コンストラクタ・デストラクタ
//////////////////////////////////////////////////////////////////
Collider_Plane::Collider_Plane() : Collider(){
    colliderType_ = ColliderType::Plane;
}

Collider_Plane::~Collider_Plane(){
}

//////////////////////////////////////////////////////////////////
// 行列の更新
//////////////////////////////////////////////////////////////////
void Collider_Plane::UpdateMatrix(){

    // ワールド行列の更新
    Collider::UpdateMatrix();

    // ローカル座標をワールド座標に変換
    for(int i = 0; i < 4; i++){
        body_.localVertex[i] = local_.localVertex[i] * worldMat_ + offset_;
    }

    // 八分木用のAABB更新
    UpdateBox();
}

//////////////////////////////////////////////////////////////////
// 描画
//////////////////////////////////////////////////////////////////
void Collider_Plane::Draw(){
   SEED::Instance::DrawLine(body_.localVertex[0], body_.localVertex[1], color_);
   SEED::Instance::DrawLine(body_.localVertex[1], body_.localVertex[2], color_);
   SEED::Instance::DrawLine(body_.localVertex[2], body_.localVertex[3], color_);
   SEED::Instance::DrawLine(body_.localVertex[3], body_.localVertex[0], color_);
}


//////////////////////////////////////////////////////////////////
// フレーム開始時処理
//////////////////////////////////////////////////////////////////
void Collider_Plane::BeginFrame(){
    // 前回のAABBを保存
    preBody_ = body_;

    Collider::BeginFrame();

}

//////////////////////////////////////////////////////////////////
// 衝突判定
//////////////////////////////////////////////////////////////////
void Collider_Plane::CheckCollision(Collider* collider){
    // Planeはまだ未実装
    collider;
}

//////////////////////////////////////////////////////////////////
// 八分木用のAABB更新
//////////////////////////////////////////////////////////////////
void Collider_Plane::UpdateBox(){
    Vector3 min;
    Vector3 max;

    min.x = (std::min)({ body_.localVertex[0].x, body_.localVertex[1].x, body_.localVertex[2].x, body_.localVertex[3].x });
    min.y = (std::min)({ body_.localVertex[0].y, body_.localVertex[1].y, body_.localVertex[2].y, body_.localVertex[3].y });
    min.z = (std::min)({ body_.localVertex[0].z, body_.localVertex[1].z, body_.localVertex[2].z, body_.localVertex[3].z });

    max.x = (std::max)({ body_.localVertex[0].x, body_.localVertex[1].x, body_.localVertex[2].x, body_.localVertex[3].x });
    max.y = (std::max)({ body_.localVertex[0].y, body_.localVertex[1].y, body_.localVertex[2].y, body_.localVertex[3].y });
    max.z = (std::max)({ body_.localVertex[0].z, body_.localVertex[1].z, body_.localVertex[2].z, body_.localVertex[3].z });

    coverAABB_.center = (min + max) * 0.5f;
    coverAABB_.halfSize = (max - min) * 0.5f;
}

//////////////////////////////////////////////////////////////////
// 前回のコライダーを破棄
//////////////////////////////////////////////////////////////////
void Collider_Plane::DiscardPreCollider(){
    UpdateMatrix();
}

//////////////////////////////////////////////////////////////////
// ImGuiでの編集
//////////////////////////////////////////////////////////////////
void Collider_Plane::Edit(){
#ifdef _DEBUG

    std::string colliderID = "##" + std::to_string(colliderID_);// コライダーID
    color_ = { 1.0f,1.0f,0.0f,1.0f };// 編集中のコライダーの色(黄色)

    // 全般情報
    Collider::Edit();

    // ローカル座標
    ImGui::Text("------ Vertices ------");
    ImGui::Indent();
    ImGui::DragFloat3(std::string("v0" + colliderID).c_str(), &local_.localVertex[0].x, 0.05f);
    ImGui::DragFloat3(std::string("v1" + colliderID).c_str(), &local_.localVertex[1].x, 0.05f);
    ImGui::DragFloat3(std::string("v2" + colliderID).c_str(), &local_.localVertex[2].x, 0.05f);
    ImGui::DragFloat3(std::string("v3" + colliderID).c_str(), &local_.localVertex[3].x, 0.05f);
    ImGui::Unindent();

    ImGui::Dummy(ImVec2(0.0f, 10.0f));

    // オフセット
    ImGui::Text("------ Offset ------");
    ImGui::Indent();
    ImGui::DragFloat3(std::string("Offset" + colliderID).c_str(), &offset_.x, 0.1f);
    ImGui::Unindent();

    // アニメーションフラグ
    ImGui::Text("------ Animation ------");
    ImGui::Checkbox("Animation", &isAnimation_);

#endif // _DEBUG
}


//////////////////////////////////////////////////////////////////
// コライダーの情報をjson形式でまとめる
//////////////////////////////////////////////////////////////////
nlohmann::json Collider_Plane::GetJsonData(){
    nlohmann::json json;

    // タイプ
    json["Type"] = "Plane";

    // 全般の情報
    json.merge_patch(Collider::GetJsonData());

    // オフセット
    json["Offset"] = offset_;

    // ローカル座標
    json["Vertices"] = {
        { "v0", local_.localVertex[0]},
        { "v1", local_.localVertex[1]},
        { "v2", local_.localVertex[2]},
        { "v3", local_.localVertex[3]}
    };

    return json;
}

//////////////////////////////////////////////////////////////////
// jsonデータ読み込み関数
//////////////////////////////////////////////////////////////////
void Collider_Plane::LoadFromJson(const nlohmann::json& jsonData){
    // 全般情報の読み込み
    Collider::LoadFromJson(jsonData);

    // オフセット
    offset_ = jsonData["Offset"];

    // ローカル座標
    local_.localVertex[0] = jsonData["Vertices"]["v0"];
    local_.localVertex[1] = jsonData["Vertices"]["v1"];
    local_.localVertex[2] = jsonData["Vertices"]["v2"];
    local_.localVertex[3] = jsonData["Vertices"]["v3"];
}

//////////////////////////////////////////////////////////////////
// ローカル座標の設定
//////////////////////////////////////////////////////////////////
void Collider_Plane::SetLocalVertices(const Vector3& v0, const Vector3& v1, const Vector3& v2, const Vector3& v3){
    local_.localVertex[0] = v0;
    local_.localVertex[1] = v1;
    local_.localVertex[2] = v2;
    local_.localVertex[3] = v3;
}
