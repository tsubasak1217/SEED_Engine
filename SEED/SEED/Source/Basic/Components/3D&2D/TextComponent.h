#pragma once
#include <SEED/Source/Basic/Components/IComponent.h>
#include <SEED/Lib/Structs/TextBox.h>

/// <summary>
/// テキストを描画するコンポーネント
/// </summary>
class TextComponent : public IComponent{
public:
    TextComponent(std::variant<GameObject*, GameObject2D*> pOwner, const std::string& tagName = "");
    ~TextComponent() = default;
    void Initialize(const std::string& text, const Vector2& position, const Color& color);
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
    std::unique_ptr<TextBox2D> textBox_; // テキストボックス
};