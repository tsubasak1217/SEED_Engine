#include "LaserObjectComponent.h"

//============================================================================
//	include
//============================================================================
#include <Game/Objects/Stage/Objects/Laser/Methods/LaserHelper.h>

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
        LaserHelper::HasObejctType(type, ObjectType::Laser)) {
        return true;
    }
    return false;
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

void LaserObjectComponent::OnCollisionEnter(GameObject2D* other) {

    // 他のフィールドオブジェクトと衝突したらレーザーの伸びる処理を止める
    // プレイヤー、ゴール以外
    if (CheckEndExtend(other)) {
        return;
    }

    // レーザーの伸びる処理を停止させる
    object_->StopExtend();
}

void LaserObjectComponent::OnCollisionStay(GameObject2D* other) {

    // 他のフィールドオブジェクトと衝突したらレーザーの伸びる処理を止める
    // プレイヤー、ゴール以外
    if (CheckEndExtend(other)) {
        return;
    }

    // レーザーの伸びる処理を停止させる
    object_->StopExtend();
}

void LaserObjectComponent::OnCollisionExit(GameObject2D* other) {

    // 他のフィールドオブジェクトと衝突したらレーザーの伸びる処理を止める
   // プレイヤー、ゴール以外
    if (CheckEndExtend(other)) {
        return;
    }

    // 当たらなくなったので伸びる処理を再開
    object_->ReExtend();
}

//============================================================================
//	LaserObjectComponent otherMethods
//============================================================================

void LaserObjectComponent::BeginFrame() {
}

void LaserObjectComponent::EndFrame() {
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