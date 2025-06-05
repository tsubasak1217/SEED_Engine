#pragma once
// stl
#include <string>
// directX
#include <d3d12.h>
// local
#include <SEED/Lib/Structs/Transform.h>
#include <SEED/Lib/Structs/blendMode.h>
#include <SEED/Lib/Tensor/Vector4.h>

enum class TextAlign{
    LEFT,
    CENTER,
    RIGHT
};

struct TextBox2D{
    std::string text;
    std::string fontName;
    Transform2D transform;
    Vector2 size = {128.0f,64.0f};
    Vector2 anchorPos = {0.5f,0.5f};
    float fontSize = 16.0f;
    float lineSpacing = 8.0f;// 行間の間隔
    float glyphSpacing = 8.0f;// 字間の間隔
    TextAlign align = TextAlign::CENTER;// テキストの配置
    BlendMode blendMode = BlendMode::NORMAL;
    Vector4 color = { 1.0f, 1.0f, 1.0f, 1.0f }; // RGBA color

#ifdef _DEBUG
    bool textBoxVisible = true;
#endif // _DEBUG

public:// 関数
    void Draw()const;
    void SetFont(const std::string& fileName);
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
    TextAlign align = TextAlign::CENTER;// テキストの配置
    BlendMode blendMode = BlendMode::NORMAL;
    Vector4 color = { 1.0f, 1.0f, 1.0f, 1.0f }; // RGBA color

public:// 関数
    void Draw()const;
};