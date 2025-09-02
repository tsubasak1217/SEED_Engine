#pragma once
#include <SEED/Source/Basic/Components/IComponent.h>

class BlockComponent : public IComponent{
public:
    BlockComponent(GameObject* pOwner, const std::string& tagName = "");
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

private:
    // IBlockを継承したブロックのインスタンスを持つ
    //std::unique_ptr<IBlock*> block_ = nullptr;
};