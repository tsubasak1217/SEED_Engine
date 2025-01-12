#include "Collider_Line.h"
#include "Base/BaseObject.h"
#include <SEED.h>

//////////////////////////////////////////////////////////////////
// コンストラクタ・デストラクタ
//////////////////////////////////////////////////////////////////
Collider_Line::Collider_Line() : Collider(){
    colliderType_ = ColliderType::Line;
}

Collider_Line::~Collider_Line(){
}

//////////////////////////////////////////////////////////////////
// 行列の更新
//////////////////////////////////////////////////////////////////
void Collider_Line::UpdateMatrix(){
    // 行列の更新
    Collider::UpdateMatrix();

    // 本体の更新
    body_.origin_ = local_.origin_ * worldMat_ + offset_;
    body_.end_ = local_.end_ * worldMat_ + offset_;

    // 八分木用のAABB更新
    UpdateBox();
}

//////////////////////////////////////////////////////////////////
// 描画
//////////////////////////////////////////////////////////////////
void Collider_Line::Draw(){
    SEED::DrawLine(body_.origin_, body_.end_, color_);
}

//////////////////////////////////////////////////////////////////
// フレーム開始時処理
//////////////////////////////////////////////////////////////////
void Collider_Line::BeginFrame(){
    // 前回のAABBを保存
    preBody_ = body_;

    Collider::BeginFrame();
}

//////////////////////////////////////////////////////////////////
// 八分木用のAABB更新
//////////////////////////////////////////////////////////////////
void Collider_Line::UpdateBox(){
    Vector3 min;
    Vector3 max;

    min.x = (std::min)({ body_.origin_.x, body_.end_.x });
    min.y = (std::min)({ body_.origin_.y, body_.end_.y });
    min.z = (std::min)({ body_.origin_.z, body_.end_.z });

    max.x = (std::max)({ body_.origin_.x, body_.end_.x });
    max.y = (std::max)({ body_.origin_.y, body_.end_.y });
    max.z = (std::max)({ body_.origin_.z, body_.end_.z });

    coverAABB_.center = (min + max) * 0.5f;
    coverAABB_.halfSize = (max - min) * 0.5f;
}


//////////////////////////////////////////////////////////////////
// ImGuiでの編集
//////////////////////////////////////////////////////////////////
void Collider_Line::Edit(){
#ifdef _DEBUG

    std::string colliderID = "##" + std::to_string(colliderID_);// コライダーID
    color_ = { 1.0f,1.0f,0.0f,1.0f };// 編集中のコライダーの色(黄色)

    // 全般情報
    Collider::Edit();

    // 中心座標
    ImGui::Text("------ Origin ------");
    ImGui::Indent();
    ImGui::DragFloat3(std::string("Origin" + colliderID).c_str(), &local_.origin_.x, 0.05f);
    ImGui::Unindent();

    ImGui::Dummy(ImVec2(0.0f, 10.0f));

    // 半径
    ImGui::Text("-------- End -------");
    ImGui::Indent();
    ImGui::DragFloat3(std::string("End" + colliderID).c_str(), &local_.end_.x, 0.05f);
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
nlohmann::json Collider_Line::GetJsonData(){
    nlohmann::json json;

    // 全般の情報
    json.merge_patch(Collider::GetJsonData());

    // コライダーの種類
    json["colliderType"] = "Line";

    // ローカル座標
    json["local"]["origin"] = local_.origin_;
    json["local"]["end"] = local_.end_;

    // オフセット
    json["offset"] = offset_;

    return json;
}

//////////////////////////////////////////////////////////////////
// jsonデータから読み込み
//////////////////////////////////////////////////////////////////
void Collider_Line::LoadFromJson(const nlohmann::json& jsonData){
    // 全般情報の読み込み
    Collider::LoadFromJson(jsonData);

    // ローカル座標
    local_.origin_ = jsonData["local"]["origin"];
    local_.end_ = jsonData["local"]["end"];

    // オフセット
    offset_ = jsonData["offset"];
}
