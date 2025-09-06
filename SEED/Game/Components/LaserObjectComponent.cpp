#include "LaserObjectComponent.h"

//============================================================================
//	include
//============================================================================
#include <Game/Objects/Stage/Objects/Laser/Methods/LaserHelper.h>
#include <Game/Components/StageObjectComponent.h>

// lasers
#include <Game/Objects/Stage/Objects/Laser/Laser.h>

//============================================================================
//	LaserObjectComponent classMethods
//============================================================================

LaserObjectComponent::LaserObjectComponent(GameObject2D* pOwner, const std::string& tagName) : IComponent(pOwner, tagName) {

    // タグの名前が指定されていなければIDをタグ名にする
    if (tagName == "") {
        componentTag_ = "LaserObjectComponent_ID:" + std::to_string(componentID_);
    }
}

void LaserObjectComponent::Initialize(LaserObjectType objectType, const Vector2& translate) {

    objectType_ = objectType;

    // インスタンスを作成
    object_ = CreateInstance(objectType);

    // 座標を設定
    object_->SetTranslate(translate);
}

std::unique_ptr<ILaserObject> LaserObjectComponent::CreateInstance(LaserObjectType objectType) const {

    // タイプで作成するインスタンスを作成する
    switch (objectType) {
    case LaserObjectType::Normaml: {

        std::unique_ptr<Laser> laser = std::make_unique<Laser>(owner_.owner2D);
        laser->Initialize();
        return laser;
    }
    }
    return nullptr;
}

bool LaserObjectComponent::CheckEndExtend(GameObject2D* other) {

    const auto type = other->GetObjectType();
    if (LaserHelper::HasObejctType(type, ObjectType::Player) ||
        LaserHelper::HasObejctType(type, ObjectType::Goal) ||
        LaserHelper::HasObejctType(type, ObjectType::EmptyBlock) ||
        LaserHelper::HasObejctType(type, ObjectType::Laser)) {
        return true;
    }
    return false;
}

Vector2 LaserObjectComponent::AxisFrom(DIRECTION4 direction) {

    switch (direction) {
    case DIRECTION4::UP:    return { 0.0f, -1.0f };
    case DIRECTION4::DOWN:  return { 0.0f,1.0f };
    case DIRECTION4::RIGHT: return { 1.0f, 0.0f };
    case DIRECTION4::LEFT:  return { -1.0f,0.0f };
    }
    return { 0.0f,1.0f };
}

float LaserObjectComponent::ComputeFrontDistance(const Vector2& center, const Vector2& size) {

    const Vector2 origin = owner_.owner2D->GetWorldTranslate();
    const Vector2 half = size * 0.5f;

    // 向き別で面の距離を計算する
    switch (object_->GetDirection()) {
    case DIRECTION4::UP:

        return origin.y - (center.y + half.y);
    case DIRECTION4::DOWN:

        return (center.y - half.y) - origin.y;
    case DIRECTION4::RIGHT:

        return (center.x - half.x) - origin.x;
    case DIRECTION4::LEFT:

        return origin.x - (center.x + half.x);
    }
    return std::numeric_limits<float>::infinity();
}

//============================================================================
//	LaserObjectComponent loopMethods
//============================================================================

void LaserObjectComponent::Update() {

    // objectの更新
    object_->SetTranslate(owner_.owner2D->GetWorldTranslate());
    object_->Update();
}

void LaserObjectComponent::Draw() {

    // objectの描画
    object_->Draw();
}

void LaserObjectComponent::OnCollisionEnter([[maybe_unused]] GameObject2D* other) {
}

void LaserObjectComponent::OnCollisionStay(GameObject2D* other) {

    // 他のフィールドオブジェクトと衝突したらレーザーの伸びる処理を止める
    // プレイヤー、ゴール以外
    if (CheckEndExtend(other)) {
        return;
    }

    // 他のオブジェクトと衝突したら
    if (StageObjectComponent* component = other->GetComponent<StageObjectComponent>()) {

        const Vector2 center = component->GetBlockTranslate(); // ブロック中心
        const Vector2 size = component->GetMapSize();          // ブロックサイズ

        // 前面距離を比較して近い方を設定する
        const float fd = ComputeFrontDistance(center, size);
        const Vector2 axis = AxisFrom(object_->GetDirection());
        const Vector2 half = size * 0.5f;
        // 軸方向半サイズ
        const float padding = std::fabs(half.x * axis.x) + std::fabs(half.y * axis.y);
        const float eps = 1e-4f;
        if (fd >= -padding - eps && fd < blocker_.frontDistance) {

            // 衝突相手を設定
            blocker_.isFound = true;
            blocker_.frontDistance = fd;
            blocker_.translate = center;
            blocker_.size = size;
        }
    }
}

void LaserObjectComponent::OnCollisionExit([[maybe_unused]] GameObject2D* other) {
}

//============================================================================
//	LaserObjectComponent otherMethods
//============================================================================

void LaserObjectComponent::BeginFrame() {

    // 衝突相手をリセット
    blocker_.isFound = false;
    blocker_.frontDistance = std::numeric_limits<float>::infinity();
}

void LaserObjectComponent::EndFrame() {

    if (blocker_.isFound) {

        // 近い方の前面まででClampして停止
        auto size = object_->GetSize();
        size.y = (std::max)(0.0f, blocker_.frontDistance);
        object_->SetSize(size);

        // 伸びる処理を停止させる
        object_->StopExtend();
    } else {

        // レーザーをもう一度動かす
        object_->ReExtend();
    }
}

void LaserObjectComponent::EditGUI() {
#ifdef _DEBUG
    ImGui::Indent();
    ImGui::Unindent();
#endif // _DEBUG
}

nlohmann::json LaserObjectComponent::GetJsonData() const {
    nlohmann::json jsonData;
    jsonData["componentType"] = "Block";
    jsonData.update(IComponent::GetJsonData());
    return jsonData;
}

void LaserObjectComponent::LoadFromJson(const nlohmann::json& jsonData) {
    IComponent::LoadFromJson(jsonData);
}