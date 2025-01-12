#include "Collider_Capsule.h"
#include "Base/BaseObject.h"
#include <SEED.h>

//////////////////////////////////////////////////////////////////////////
// コンストラクタ・デストラクタ
//////////////////////////////////////////////////////////////////////////
Collider_Capsule::Collider_Capsule() : Collider(){
    colliderType_ = ColliderType::Capsule;
}

Collider_Capsule::~Collider_Capsule(){
}

//////////////////////////////////////////////////////////////////////////
// 行列の更新
//////////////////////////////////////////////////////////////////////////
void Collider_Capsule::UpdateMatrix(){
    // 行列の更新
    Collider::UpdateMatrix();

    // 本体の更新
    body_.origin = local_.origin * worldMat_ + offset_;
    body_.end = local_.end * worldMat_ + offset_;

    // 八分木用のAABB更新
    UpdateBox();
}

//////////////////////////////////////////////////////////////////////////
// 描画
//////////////////////////////////////////////////////////////////////////
void Collider_Capsule::Draw(){
    SEED::DrawCapsule(body_.origin, body_.end, body_.radius, 6, color_);
}


//////////////////////////////////////////////////////////////////////////
// フレーム開始時処理
//////////////////////////////////////////////////////////////////////////
void Collider_Capsule::BeginFrame(){
    // 前回のAABBを保存
    preBody_ = body_;

    Collider::BeginFrame();
}

//////////////////////////////////////////////////////////////////////////
// 八分木用のAABB更新
//////////////////////////////////////////////////////////////////////////
void Collider_Capsule::UpdateBox(){
    Vector3 min;
    Vector3 max;

    min.x = (std::min)({ body_.origin.x, body_.end.x }) - body_.radius;
    min.y = (std::min)({ body_.origin.y, body_.end.y }) - body_.radius;
    min.z = (std::min)({ body_.origin.z, body_.end.z }) - body_.radius;

    max.x = (std::max)({ body_.origin.x, body_.end.x }) + body_.radius;
    max.y = (std::max)({ body_.origin.y, body_.end.y }) + body_.radius;
    max.z = (std::max)({ body_.origin.z, body_.end.z }) + body_.radius;

    coverAABB_.center = (min + max) * 0.5f;
    coverAABB_.halfSize = (max - min) * 0.5f;
}


//////////////////////////////////////////////////////////////////////////
// ImGuiでの編集
//////////////////////////////////////////////////////////////////////////
void Collider_Capsule::Edit(){

#ifdef _DEBUG

    std::string colliderID = "##" + std::to_string(colliderID_);// コライダーID
    color_ = { 1.0f,1.0f,0.0f,1.0f };// 編集中のコライダーの色(黄色)

    // 全般情報
    Collider::Edit();

    // 中心座標
    ImGui::Text("------ Origin ------");
    ImGui::Indent();
    ImGui::DragFloat3(std::string("Origin" + colliderID).c_str(), &local_.origin.x, 0.05f);
    ImGui::Unindent();

    ImGui::Dummy(ImVec2(0.0f, 10.0f));

    // 半径
    ImGui::Text("-------- End -------");
    ImGui::Indent();
    ImGui::DragFloat3(std::string("End" + colliderID).c_str(), &local_.end.x, 0.05f);
    ImGui::Unindent();

    ImGui::Dummy(ImVec2(0.0f, 10.0f));

    // 半径
    ImGui::Text("------ Radius ------");
    ImGui::Indent();
    ImGui::DragFloat(std::string("Radius" + colliderID).c_str(), &body_.radius, 0.05f, 0.0f);
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


//////////////////////////////////////////////////////////////////////////
// コライダーの情報をjson形式でまとめる
//////////////////////////////////////////////////////////////////////////
nlohmann::json Collider_Capsule::GetJsonData(){
    nlohmann::json json;

    // 全般の情報
    json.merge_patch(Collider::GetJsonData());

    // ローカル座標
    json["local"]["origin"] = local_.origin;
    json["local"]["end"] = local_.end;
    json["radius"] = body_.radius;

    // オフセット
    json["offset"] = offset_;

    return json;
}

//////////////////////////////////////////////////////////////////////////
// jsonデータから読み込み
//////////////////////////////////////////////////////////////////////////
void Collider_Capsule::LoadFromJson(const nlohmann::json& jsonData){
    // 全般情報の読み込み
    Collider::LoadFromJson(jsonData);

    // ローカル座標
    local_.origin = jsonData["local"]["origin"];
    local_.end = jsonData["local"]["end"];
    body_.radius = jsonData["radius"];

    // オフセット
    offset_ = jsonData["offset"];
}

