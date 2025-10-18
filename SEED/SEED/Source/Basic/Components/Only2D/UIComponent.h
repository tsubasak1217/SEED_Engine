#pragma once
#include <SEED/Source/Basic/Object/GameObject2D.h>
#include <SEED/Source/Basic/Components/IComponent.h>
#include <SEED/Lib/Structs/TextBox.h>
#include <SEED/Lib/Structs/Sprite.h>
// stl
#include <memory>
#include <vector>

/*----------- 衝突判定をするやつ ----------*/
class UIComponent : public IComponent{
public:
    UIComponent(GameObject2D* pOwner, const std::string& tagName = "");
    ~UIComponent() = default;
    void BeginFrame()override;
    void Update()override;
    void Draw()override;
    void EndFrame()override;
    void Finalize()override;
    void EditGUI()override;

public:// accessor
    Sprite& GetSprite(size_t index);
    Sprite& GetSprite(const std::string& name);
    TextBox2D& GetText(size_t index);
    TextBox2D& GetText(const std::string& name);
    void SetMasterColor(const Color& color){ masterColor_ = color; }

public:// json
    void LoadFromJson(const nlohmann::json& jsonData) override;
    nlohmann::json GetJsonData() const override;

protected:
    int32_t spriteCount_ = 0;
    int32_t textCount_ = 0;
    std::vector<std::pair<std::string, Sprite>> sprites_;
    std::vector<std::pair<std::string, TextBox2D>> texts_;
    Color masterColor_ = Color(1.0f);
};