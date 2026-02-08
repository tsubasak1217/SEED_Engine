#pragma once
// stl
#include <string>
#include <variant>
#include <initializer_list>
#include <functional>
// directX
#include <d3d12.h>
/// json
#include <json.hpp>
// local
#include <SEED/Lib/Structs/Transform.h>
#include <SEED/Lib/Structs/blendMode.h>
#include <SEED/Lib/Tensor/Vector4.h>
#include <SEED/Lib/Shapes/Quad.h>
#include <SEED/Lib/Functions/FileFunc.h>

using BindData = std::variant<std::reference_wrapper<std::string>, std::reference_wrapper<int>, std::reference_wrapper<float>>;

namespace SEED{

    // 横の字詰め方式
    enum class TextAlignX{
        LEFT,
        CENTER,
        RIGHT
    };

    // 縦の字詰め方式
    enum class TextAlignY{
        TOP,
        CENTER,
        BOTTOM
    };


    /// <summary>
    /// 2次元テキストボックス構造体
    /// </summary>
    struct TextBox2D{
        TextBox2D() = default;
        TextBox2D(const std::string& _text) : text(_text){ SetFont(""); }
        void Draw(const std::optional<Color>& masterColor = std::nullopt)const;
        void ScrollDraw(float scrollSpeed, const std::optional<Color>& masterColor = std::nullopt)const;
        void SetFont(const std::string& fileName);
        void BindDatas(std::initializer_list<BindData> datas);

    private:
        void DrawTextBoxRange(const Matrix3x3& textBoxMat, const Vector2& anchorOffset, const Color& boxColor)const;

    public:// パラメータなど
        std::string text;
        std::string fontName;
        Transform2D transform;
        const Matrix3x3* parentMat = nullptr;// 親行列
        Vector2 size = { 128.0f,64.0f };
        Vector2 anchorPos = { 0.5f,0.5f };
        float fontSize = 100.0f;
        float lineSpacing = 8.0f;// 行間の間隔
        float glyphSpacing = 8.0f;// 字間の間隔
        TextAlignX alignX = TextAlignX::CENTER;// テキストの配置
        TextAlignY alignY = TextAlignY::TOP;// テキストの配置
        BlendMode blendMode = BlendMode::NORMAL;
        float textDisplayRate = 1.0f; // テキスト表示率(0.0~1.0)
        bool useOutline = false;
        float outlineWidth = 2.0f; // アウトラインの幅
        int outlineSplitCount = 16;
        Color color = { 1.0f, 1.0f, 1.0f, 1.0f };
        Vector4 outlineColor = { 0.0f, 0.0f, 0.0f, 1.0f };
        bool isApplyViewMat = false;// ビュー行列を適用するかどうか
        bool textBoxVisible = true;
        bool isStaticDraw = false; // 静的描画かどうか
        DrawLocation drawLocation = DrawLocation::Front;
        int32_t layer = 0; // 描画順。大きいほど手前に描画

    private:// フォーマット解析
        std::vector<BindData> bindedDatas; // バインドされたデータ(表示するもの)
        float scrollTime = 0.0f; // 総スクロール時間

    public:// 編集・入出力関連
    #ifdef _DEBUG
        void Edit(const std::string& hash = "");
    #endif // _DEBUG
        nlohmann::json GetJsonData() const;
        void LoadFromJson(const nlohmann::json& jsonData);
    };



    struct TextBox3D{
        std::string text;
        std::string fontName;
        Transform transform;
        Vector2 size;
        Vector2 anchorPos;
        float fontSize = 16.0f;
        float lineSpacing = 8.0f;// 行間の間隔
        float glyphSpacing = 8.0f;// 字間の間隔
        TextAlignX align = TextAlignX::CENTER;// テキストの配置
        BlendMode blendMode = BlendMode::NORMAL;
        Color color = { 1.0f, 1.0f, 1.0f, 1.0f }; // RGBA color

    public:// 関数
        void Draw()const;
    };

} // namespace SEED