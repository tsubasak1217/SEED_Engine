#pragma once
#include <SEED/Source/Basic/Object/GameObject2D.h>
#include <SEED/Source/Basic/Components/IComponent.h>
#include <SEED/Lib/Structs/TextBox.h>
#include <SEED/Lib/Structs/Sprite.h>
// stl
#include <memory>
#include <vector>

namespace SEED{
    /// <summary>
    /// UI描画コンポーネント(テキスト・スプライト)
    /// </summary>
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

    private:
        void EditUVScroll(int32_t index);

    private:

        enum ScrollType{
            Normal,// 通常スクロール
            StepImage// 一定時間ごとに指定値をスクロール(連番画像形式はこれ)
        };

        struct SpriteItems{
            Sprite sprite;

            // UVスクロール設定
            bool isUVScroll = false;
            ScrollType uvScrollType = ScrollType::Normal;
            Vector2 uvScrollValue = Vector2(0.0f);
            int32_t maxStepCount;
            int32_t maxColmnCount;
            int32_t curColmnCount;
            int32_t totalStepCount;
            Timer stepTimer;

            // 値保存用
            Transform2D originalUVTransform;
            Transform2D finalUVTransform;
        };

    private:
        int32_t spriteCount_ = 0;
        int32_t textCount_ = 0;
        std::vector<std::pair<std::string, SpriteItems>> sprites_;
        std::vector<std::pair<std::string, TextBox2D>> texts_;
        Color masterColor_ = Color(1.0f);
    };
}