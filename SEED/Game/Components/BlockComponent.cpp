#include "BlockComponent.h"

//============================================================================
//	include
//============================================================================
#include <Game/Objects/Stage/Block/Blocks/BlockNormal.h>

BlockComponent::BlockComponent(GameObject2D* pOwner, const std::string& tagName) : IComponent(pOwner, tagName){

    // タグの名前が指定されていなければIDをタグ名にする
    if (tagName == "") {
        componentTag_ = "BlockComponent_ID:" + std::to_string(componentID_);
    }
}

void BlockComponent::Initialize() {
}

void BlockComponent::Initialize(BlockType blockType, const Vector2& translate) {

    blockType_ = blockType;

    // インスタンスを作成
    block_ = CreateBlockInstance(blockType);
    // 座標を設定
    block_->SetTranslate(translate);
}

std::unique_ptr<IBlock> BlockComponent::CreateBlockInstance(BlockType blockType) const {

    // タイプで作成するインスタンスを作成する
    switch (blockType) {
    case BlockType::Normal: {

        std::unique_ptr<BlockNormal> block = std::make_unique<BlockNormal>();
        block->Initialize("Scene_Game/normalBlock.png");
        return block;
    }
    }
    return nullptr;
}

void BlockComponent::BeginFrame() {
}

void BlockComponent::Update() {

    // blockの更新
    block_->Update();
}

void BlockComponent::Draw() {

    // blockの描画
    block_->Draw();
}

void BlockComponent::EndFrame() {
}

void BlockComponent::Finalize() {
}

//////////////////////////////////////////////////////////////////////////////
// GUI編集
//////////////////////////////////////////////////////////////////////////////
void BlockComponent::EditGUI() {
#ifdef _DEBUG

    ImGui::Indent();

    // 編集

    ImGui::Unindent();

#endif // _DEBUG

}

//////////////////////////////////////////////////////////////////////////////
// jsonデータの取得
//////////////////////////////////////////////////////////////////////////////
nlohmann::json BlockComponent::GetJsonData() const {
    nlohmann::json jsonData;
    jsonData["componentType"] = "Block";
    jsonData.update(IComponent::GetJsonData());

    // block情報をjsonに書き込む

    return jsonData; // JSONデータを返す
}

//////////////////////////////////////////////////////////////////////////////
// jsonデータからの読み込み
////////////////////////////////////////////////////////////////////////////////
void BlockComponent::LoadFromJson(const nlohmann::json& jsonData) {
    IComponent::LoadFromJson(jsonData); // 基底クラスのjsonデータを読み込み

    // block情報をjsonから読み込む

}