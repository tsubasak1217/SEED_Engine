#include "StageObjectComponent.h"

//============================================================================
//	include
//============================================================================

// objects
#include <Game/Objects/Stage/Objects/Block/BlockNormal.h>
#include <Game/Objects/Stage/Objects/Goal/Goal.h>
#include <Game/Objects/Stage/Objects/Player/Entity/Player.h>

//============================================================================
//	StageObjectComponent classMethods
//============================================================================

StageObjectComponent::StageObjectComponent(GameObject2D* pOwner, const std::string& tagName) : IComponent(pOwner, tagName) {

    // タグの名前が指定されていなければIDをタグ名にする
    if (tagName == "") {
        componentTag_ = "BlockComponent_ID:" + std::to_string(componentID_);
    }
}

void StageObjectComponent::Initialize() {
}

void StageObjectComponent::Initialize(StageObjectType objectType, const Vector2& translate,
    const Vector2& size) {

    objectType_ = objectType;

    // インスタンスを作成
    object_ = CreateInstance(objectType);
    // 座標、サイズを設定
    object_->SetTranslate(translate);
    object_->SetSize(size);
}

Player* StageObjectComponent::GetPlayer() const {

    return dynamic_cast<Player*>(object_.get());
}


//============================================================================
// 衝突時の処理
//============================================================================
void StageObjectComponent::OnCollisionStay(GameObject2D* other){
    
    other;

    // プレイヤーインスタンスを持っている場合
    if(objectType_ == StageObjectType::Player){
        Player* player = dynamic_cast<Player*>(object_.get());

        // 着地した瞬間を検知
        if(owner_.owner2D->GetIsOnGroundTrigger()){
            player->OnGroundTrigger();
        }
    }
}


std::unique_ptr<IStageObject> StageObjectComponent::CreateInstance(StageObjectType objectType) const {

    // タイプで作成するインスタンスを作成する
    switch (objectType) {
    case StageObjectType::NormalBlock: {

        std::unique_ptr<BlockNormal> block = std::make_unique<BlockNormal>(owner_.owner2D);
        block->Initialize("Scene_Game/normalBlock.png");
        return block;
    }
    case StageObjectType::Goal: {

        std::unique_ptr<Goal> goal = std::make_unique<Goal>(owner_.owner2D);
        goal->Initialize("DefaultAssets/monsterBall.png");
        return goal;
    }
    case StageObjectType::Player: {

        std::unique_ptr<Player> player = std::make_unique<Player>(owner_.owner2D);
        player->Initialize("DefaultAssets/ellipse.png");
        return player;
    }
    }
    return nullptr;
}

void StageObjectComponent::BeginFrame() {
}

void StageObjectComponent::Update() {

    // objectの更新
    object_->SetTranslate(owner_.owner2D->GetWorldTranslate());
    object_->Update();
}

void StageObjectComponent::Draw() {

    // objectの描画
    object_->Draw();
}

void StageObjectComponent::EndFrame() {
}

void StageObjectComponent::Finalize() {
}

//////////////////////////////////////////////////////////////////////////////
// GUI編集
//////////////////////////////////////////////////////////////////////////////
void StageObjectComponent::EditGUI() {
#ifdef _DEBUG

    ImGui::Indent();

    // 編集

    ImGui::Unindent();

#endif // _DEBUG

}

//////////////////////////////////////////////////////////////////////////////
// jsonデータの取得
//////////////////////////////////////////////////////////////////////////////
nlohmann::json StageObjectComponent::GetJsonData() const {
    nlohmann::json jsonData;
    jsonData["componentType"] = "Block";
    jsonData.update(IComponent::GetJsonData());

    // object情報をjsonに書き込む

    return jsonData; // JSONデータを返す
}

//////////////////////////////////////////////////////////////////////////////
// jsonデータからの読み込み
////////////////////////////////////////////////////////////////////////////////
void StageObjectComponent::LoadFromJson(const nlohmann::json& jsonData) {
    IComponent::LoadFromJson(jsonData); // 基底クラスのjsonデータを読み込み

    // object情報をjsonから読み込む

}