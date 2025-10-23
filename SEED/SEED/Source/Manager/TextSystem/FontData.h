#pragma once
// stl
#include <cstdint>
#include <unordered_map>
#include <vector>
// local
#include <SEED/Lib/Tensor/Vector2.h>


/// <summary>
// 文字データ
/// </summary>
struct GlyphData{
    int32_t graphHandle = -1; // グラフハンドル(フォントアトラスの)
    Vector2 texcoordLT; // グリフのテクスチャ座標(左上)
    Vector2 texcoordRB; // グリフのテクスチャ座標(右下)
    float yRatio; // 最大縦幅に対するyサイズの比率
    float xRatio;// yサイズに対するxサイズの比率
    float yOffset; // yオフセット(ベースラインからのオフセット)
};


/// <summary>
// フォントアトラスデータ(フォント画像一枚分)
/// </summary>
struct FontAtlas{
    int32_t textureHandle = -1;
    std::string atlasName;
    std::unordered_map<int32_t, GlyphData> glyphs;// 文字コードでグリフデータを管理
    std::vector<unsigned char> bitmap;// アトラスのビットマップデータ
    static const int texWidth = 4096, texHeight = 4096;// アトラスのテクスチャサイズ
};


/// <summary>
// フォントデータ
/// </summary>
struct FontData{
    std::vector<FontAtlas> atlases;
    // 文字コードでグリフデータを管理
    std::unordered_map<int32_t,GlyphData*> glyphDatas;
    // フォントのベースライン高さの比率(縦幅に対して)
    float baselneHeightRate;
};