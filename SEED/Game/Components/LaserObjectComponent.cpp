#include "LaserObjectComponent.h"

//============================================================================
//	include
//============================================================================

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
        laser->Initialize("Scene_Game/StageObject/laserForward.png");
        return laser;
    }
    }
    return nullptr;
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

void LaserObjectComponent::OnCollisionStay([[maybe_unused]] GameObject2D* other) {


}

void LaserObjectComponent::OnCollisionEnter([[maybe_unused]] GameObject2D* other) {


}

void LaserObjectComponent::OnCollisionExit([[maybe_unused]] GameObject2D* other) {


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