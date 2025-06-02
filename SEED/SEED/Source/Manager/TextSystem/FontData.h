#pragma once
// stb
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
// stl
#include <cstdint>
#include <unordered_map>
// local
#include <SEED/Lib/Tensor/Vector2.h>

// 文字データ
struct GlyphData{
    uint32_t graphHandle; // グラフハンドル(フォントアトラスの)
    Vector2 texcoord; // グリフのテクスチャ座標
    Vector2 size; // グリフのサイズ
};

// フォントデータ
struct FontData{
    // 文字コードでグリフデータを管理
    std::unordered_map<int32_t,GlyphData> glyphDatas;
};