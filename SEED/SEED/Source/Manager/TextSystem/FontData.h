#pragma once
// stl
#include <cstdint>
#include <unordered_map>
#include <vector>
// local
#include <SEED/Lib/Tensor/Vector2.h>

// 文字データ
struct GlyphData{
    uint32_t graphHandle; // グラフハンドル(フォントアトラスの)
    Vector2 texcoordLT; // グリフのテクスチャ座標(左上)
    Vector2 texcoordRB; // グリフのテクスチャ座標(右下)
    float yRatio; // 最大縦幅に対するyサイズの比率
    float xRatio;// yサイズに対するxサイズの比率
    float yOffset; // yオフセット(ベースラインからのオフセット)
};

// フォントアトラスデータ
struct FontAtlas{
    uint32_t textureHandle;
    std::unordered_map<int32_t, GlyphData> glyphs;
};

// フォントデータ
struct FontData{
    std::vector<FontAtlas> atlases;
    // 文字コードでグリフデータを管理
    std::unordered_map<int32_t,GlyphData*> glyphDatas;
};