#pragma once

//============================================================================
//	include
//============================================================================
#include <SEED/Source/Basic/Components/IComponent.h>
#include <SEED/Source/Basic/Object/GameObject2D.h>
#include <Game/Objects/Stage/Block/Interface/IBlock.h>

//============================================================================
//	BlockComponent class
//============================================================================
class BlockComponent : public IComponent {
public:
    BlockComponent(GameObject2D* pOwner, const std::string& tagName = "");
    ~BlockComponent() = default;

    void Initialize()override;
    void BeginFrame() override;
    void Update() override;
    void Draw() override;
    void EndFrame() override;
    void Finalize() override;
    // GUI編集
    void EditGUI() override;
    // json
    nlohmann::json GetJsonData() const override;
    void LoadFromJson(const nlohmann::json& jsonData) override;

    void Initialize(BlockType blockType, const Vector2& translate);

    //--------- accessor -----------------------------------------------------

    void SetBlockCommonState(BlockCommonState state) { block_->SetCommonState(state); }

    BlockType GetBlockType() const { return blockType_; }
    const Vector2& GetBlockTranslate() const { return block_->GetTranslate(); }
private:
    //========================================================================
    //	private Methods
    //========================================================================

    //--------- variables ----------------------------------------------------

    // タイプ
    BlockType blockType_;
    // IBlockを継承したブロックのインスタンスを持つ
    std::unique_ptr<IBlock> block_ = nullptr;

    //--------- functions ----------------------------------------------------

    // helper
    std::unique_ptr<IBlock> CreateBlockInstance(BlockType blockType) const;
};