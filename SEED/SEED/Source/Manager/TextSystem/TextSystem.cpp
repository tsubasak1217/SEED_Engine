#include "TextSystem.h"


// インスタンスの取得
const TextSystem* TextSystem::GetInstance(){
    if(!instance_){
        instance_ = new TextSystem();
    }
    return instance_;
}

/////////////////////////////////////////////////////////////////
// 初期化
/////////////////////////////////////////////////////////////////
void TextSystem::Initialize(){
}


/////////////////////////////////////////////////////////////////
// 文字データの取得
/////////////////////////////////////////////////////////////////
const GlyphData& TextSystem::GetGlyphData(const std::string& fontName, int32_t codePoint){
    // フォントデータが存在しない場合はnullptrを返す
    if(fontDataMap_.find(fontName) == fontDataMap_.end()){
        assert(false);
    }
    // フォントデータを取得
    const FontData& fontData = *fontDataMap_[fontName];

    // 文字コードに対応するグリフデータを取得
    auto it = fontData.glyphDatas.find(codePoint);
    if(it != fontData.glyphDatas.end()){
        return it->second; // グリフデータを返す
    }

    // 存在しない場合は空のグリフデータを返す
    static GlyphData emptyGlyphData;
    return emptyGlyphData;
}


///////////////////////////////////////////////////////////////////
// ttfファイルをバイナリから変換する関数(LoadFont用)
///////////////////////////////////////////////////////////////////
std::vector<unsigned char> TextSystem::ParseTTF(const char* filename){
    std::ifstream file(filename, std::ios::binary);
    return std::vector<unsigned char>((std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>());
}


///////////////////////////////////////////////////////////////////
// フォントを読み込む
///////////////////////////////////////////////////////////////////
const FontData* TextSystem::LoadFont(const std::string& filename){
    // すでに読み込まれている場合はそのポインタを返す
    auto it = fontDataMap_.find(filename);
    if(it != fontDataMap_.end()){
        return it->second.get();
    }

    // フォントファイル読み込み（バイナリとして）
    std::vector<unsigned char> fontBuffer = ParseTTF(filename.c_str());
    if(fontBuffer.empty()){
        return nullptr; // 読み込み失敗
    }

    // stb_truetype 初期化
    stbtt_fontinfo font;
    stbtt_InitFont(&font, fontBuffer.data(), 0);

    // パッカー初期化
    stbtt_pack_context pc;
    stbtt_PackBegin(&pc, bitmap.data(), texWidth, texHeight, 0, 1, nullptr);

    // 記号範囲の設定
    std::vector<stbtt_packedchar> packed_chars(jpGlyph.size());
    stbtt_pack_range range = {};
    range.font_size = 32.0f;
    range.first_unicode_codepoint_in_range = 0; // ← codepoints に実際のコードを指定
    range.array_of_unicode_codepoints = jpGlyph.data();
    range.num_chars = jpGlyph.size();
    range.chardata_for_range = packed_chars.data();

    stbtt_PackFontRanges(&pc, fontBuffer.data(), 0, &range, 1);
    stbtt_PackEnd(&pc);


    // テクスチャ作成・グリフ情報生成（ここは省略、後で実装可）
    FontData fontData;
    for(int32_t codePoint : codePoints){
        GlyphData glyph = CreateGlyph(fontInfo, codePoint); // ユーザー定義関数
        fontData.glyphDatas[codePoint] = glyph;
    }

    // マップに追加（moveで無駄コピーを回避）
    auto [insertedIt, success] = fontDataMap_.emplace(filename, std::move(fontData));
    if(!success){
        return nullptr; // 追加失敗（まれ）
    }

    // ポインタを返す
    return insertedIt->second.get();
}