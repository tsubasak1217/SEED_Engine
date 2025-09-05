#include "Collider_AABB2D.h"
#include <SEED/Source/Basic/Object/GameObject.h>
#include <SEED/Source/Basic/Collision/2D/Collider_Circle.h>
#include <SEED/Source/Manager/CollisionManager/Collision.h>
#include <SEED/Source/SEED.h>

//////////////////////////////////////////////////////////////////
// コンストラクタ・デストラクタ
//////////////////////////////////////////////////////////////////
Collider_AABB2D::Collider_AABB2D() : Collider2D() {

    colliderType_ = ColliderType2D::AABB;
    anchor_ = Vector2(0.5f, 0.5f);
}

Collider_AABB2D::~Collider_AABB2D() {
}


//////////////////////////////////////////////////////////////////
// 行列の更新
//////////////////////////////////////////////////////////////////
void Collider_AABB2D::UpdateMatrix() {

    // 行列の更新
    Collider2D::UpdateMatrix();

    // アンカーを考慮したローカル中心
    Vector2 anchoredLocalCenter = local_.center +
        Vector2((0.5f - anchor_.x) * 2.0f * local_.halfSize.x,
            (0.5f - anchor_.y) * 2.0f * local_.halfSize.y);

    // 本体の更新
    body_.center = anchoredLocalCenter * worldMat_ + offset_;

    Vector2 scale = ExtractScale(worldMat_);
    float  angle = ExtractRotation(worldMat_);
    Vector2 e = Vector2(local_.halfSize.x * scale.x, local_.halfSize.y * scale.y);
    float cos = std::cos(angle);
    float sin = std::sin(angle);
    // 回転させ矩形をAABBに回転変換
    body_.halfSize = Vector2(std::abs(cos) * e.x + std::abs(sin) * e.y, std::abs(sin) * e.x + std::abs(cos) * e.y);

    // 八分木用のAABB更新
    UpdateBox();
}


//////////////////////////////////////////////////////////////////
// 描画
//////////////////////////////////////////////////////////////////
void Collider_AABB2D::Draw() {

    SEED::DrawAABB2D(body_, color_);
}


//////////////////////////////////////////////////////////////////
// フレーム開始時処理
//////////////////////////////////////////////////////////////////
void Collider_AABB2D::BeginFrame() {

    // 前回のAABBを保存
    preBody_ = body_;

    Collider2D::BeginFrame();
}


//////////////////////////////////////////////////////////////////
// 衝突判定
//////////////////////////////////////////////////////////////////
void Collider_AABB2D::CheckCollision(Collider2D* collider) {

    // すでに衝突している場合は処理を行わない
    if (collisionList_.find(collider->GetColliderID()) != collisionList_.end()) { return; }

    CollisionData2D collisionData;

    switch (collider->GetColliderType()) {
    case ColliderType2D::AABB:
    {
        Collider_AABB2D* aabb = dynamic_cast<Collider_AABB2D*>(collider);
        collisionData = Collision::AABB2D::AABB2D(this, aabb);


        if (collisionData.isCollide) {
            // 押し戻しを行う
            PushBack(this, collider, collisionData);

            // 衝突時の処理
            OnCollision(collider, collider->GetObjectType());
            collider->OnCollision(this, objectType_);
        }

        break;
    }
    }

    if (collisionData.isCollide) {
        collidedPosition_ = collider->GetWoarldTranslate();
        collisionList_.insert(collider->GetColliderID());
    }
}


//////////////////////////////////////////////////////////////////
// トンネリングを考慮しないtrue or falseの衝突判定
//////////////////////////////////////////////////////////////////
bool Collider_AABB2D::CheckCollision(const AABB2D& aabb) {
    return Collision::AABB2D::AABB2D(body_, aabb);
}

//////////////////////////////////////////////////////////////////
// 八分木用のAABB更新
//////////////////////////////////////////////////////////////////
void Collider_AABB2D::UpdateBox() {

    Vector2 min = {
        (std::min)(body_.center.x - body_.halfSize.x, preBody_.center.x - preBody_.halfSize.x),
        (std::min)(body_.center.y - body_.halfSize.y, preBody_.center.y - preBody_.halfSize.y)
    };

    Vector2 max = {
        (std::max)(body_.center.x + body_.halfSize.x, preBody_.center.x + preBody_.halfSize.x),
        (std::max)(body_.center.y + body_.halfSize.y, preBody_.center.y + preBody_.halfSize.y)
    };

    coverAABB_.halfSize = (max - min) * 0.5f;
    coverAABB_.center = min + coverAABB_.halfSize;
}


//////////////////////////////////////////////////////////////////
// 前回のコライダーを破棄
//////////////////////////////////////////////////////////////////
void Collider_AABB2D::DiscardPreCollider() {
    UpdateMatrix();
}

//////////////////////////////////////////////////////////////////
// ImGuiでのパラメーター編集
//////////////////////////////////////////////////////////////////
void Collider_AABB2D::Edit() {
#ifdef _DEBUG

    std::string colliderID = "##" + std::to_string(colliderID_);// コライダーID
    color_ = { 1.0f,1.0f,0.0f,1.0f };// 編集中のコライダーの色(黄色)

    // 全般情報
    Collider2D::Edit();

    // 中心座標
    ImGui::Text("------ Center ------");
    ImGui::Indent();
    ImGui::DragFloat2(std::string("Center" + colliderID).c_str(), &local_.center.x, 0.1f);
    ImGui::Unindent();

    ImGui::Dummy(ImVec2(0.0f, 10.0f));

    // 半径
    ImGui::Text("------ HalfSie ------");
    ImGui::Indent();
    ImGui::DragFloat2(std::string("HalfSie" + colliderID).c_str(), &body_.halfSize.x, 0.025f);
    ImGui::Unindent();

    ImGui::Dummy(ImVec2(0.0f, 10.0f));

    // オフセット
    ImGui::Text("------ Offset ------");
    ImGui::Indent();
    ImGui::DragFloat2(std::string("Offset" + colliderID).c_str(), &offset_.x, 0.1f);
    ImGui::Unindent();

    // ワールド座標
    ImGui::Text("World: %.2f, %.2f", body_.center.x, body_.center.y);

#endif // _DEBUG
}

//////////////////////////////////////////////////////////////////
// コライダーの情報をjson形式でまとめる
//////////////////////////////////////////////////////////////////
nlohmann::json Collider_AABB2D::GetJsonData() {
    nlohmann::json json;

    // 全般の情報
    json.merge_patch(Collider2D::GetJsonData());

    // コライダーの種類
    json["colliderType"] = "AABB2D";

    // ローカル座標
    json["local"]["center"] = local_.center;
    json["local"]["halfSize"] = body_.halfSize;

    // オフセット
    json["offset"] = offset_;

    return json;
}

//////////////////////////////////////////////////////////////////
// jsonデータから読み込み
//////////////////////////////////////////////////////////////////
void Collider_AABB2D::LoadFromJson(const nlohmann::json& jsonData) {
    // 全般情報の読み込み
    Collider2D::LoadFromJson(jsonData);

    // ローカル座標
    local_.center = jsonData["local"]["center"];
    body_.halfSize = jsonData["local"]["halfSize"];

    // オフセット
    offset_ = jsonData["offset"];
}

//////////////////////////////////////////////////////////////////
// 移動したかどうか
//////////////////////////////////////////////////////////////////
bool Collider_AABB2D::IsMoved() {
    AABB2D aabb[2] = {
        GetAABB(),
        GetPreAABB()
    };

    if (aabb[0].center != aabb[1].center or aabb[0].halfSize != aabb[1].halfSize) {
        return true;
    }

    return false;
}
