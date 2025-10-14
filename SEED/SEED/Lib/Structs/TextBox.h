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

using BindData = std::variant<std::reference_wrapper<std::string>, std::reference_wrapper<int>, std::reference_wrapper<float>>;

enum class TextAlignX{
    LEFT,
    CENTER,
    RIGHT
};

enum class TextAlignY{
    TOP,
    CENTER,
    BOTTOM
};

struct TextBox2D{

    TextBox2D() = default;
    TextBox2D(const std::string& _text) : text(_text){ SetFont(""); }
    void Draw(const std::optional<Vector4>& masterColor = std::nullopt)const;
    void SetFont(const std::string& fileName);
    void BindDatas(std::initializer_list<BindData> datas);

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
    bool useOutline = false;
    float outlineWidth = 2.0f; // アウトラインの幅
    int outlineSplitCount = 16;
    Vector4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
    Vector4 outlineColor = { 0.0f, 0.0f, 0.0f, 1.0f };
    bool isApplyViewMat = false;// ビュー行列を適用するかどうか
    bool textBoxVisible = true;
    DrawLocation drawLocation = DrawLocation::Front;
    int32_t layer = 0; // 描画順。大きいほど手前に描画

private:// フォーマット解析
    std::vector<BindData> bindedDatas; // バインドされたデータ(表示するもの)
    std::vector<std::string> AnalyzeFormatToken(const std::string& sourceText)const;

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
    Vector4 color = { 1.0f, 1.0f, 1.0f, 1.0f }; // RGBA color

public:// 関数
    void Draw()const;
};