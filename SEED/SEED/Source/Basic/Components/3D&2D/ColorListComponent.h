#pragma once
#include <SEED/Source/Basic/Components/IComponent.h>
#include <SEED/Lib/Structs/Color.h>
#include <string>
#include <vector>

/// <summary>
/// 色を制御するコンポーネント
/// </summary>
class ColorListComponent : public IComponent{
public:// 基礎関数
    ColorListComponent(std::variant<GameObject*, GameObject2D*> pOwner, const std::string& tagName = "");
    ~ColorListComponent() = default;
    void Initialize()override;
    void BeginFrame() override;
    void Update() override;
    void Draw() override;
    void EndFrame() override;
    void Finalize() override;

public:
    const Color& GetColor(const std::string& name) const;
    const Color& GetColor(uint32_t index) const;
    const std::vector<Color>& GetColorName() const{ return colorList_; }

public:// 入出力
    // GUI編集
    void EditGUI() override;
    // json
    nlohmann::json GetJsonData() const override;
    void LoadFromJson(const nlohmann::json& jsonData) override;

private:
    // 色と名前リスト
    std::vector<std::string> colorNameList_;
    std::vector<Color> colorList_;
};