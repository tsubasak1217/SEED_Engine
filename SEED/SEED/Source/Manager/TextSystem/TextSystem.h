#pragma once
// stl
#include <cstdint>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <string>
#include <memory>
// local
#include <SEED/Source/Manager/TextSystem/FontData.h>
#include <SEED/Source/Manager/ImGuiManager/ImGlyph.h>
#include <SEED/Lib/Functions/MyFunc/DxFunc.h>
#include <SEED/Lib/Structs/TextBox.h>
// 前方宣言
struct stbtt_fontinfo;

class TextSystem{
private:
    // privateコンストラクタ
    TextSystem() = default;
    // コピー禁止
    TextSystem(const TextSystem&) = delete;
    void operator=(const TextSystem&) = delete;
    // instance
    inline static TextSystem* instance_ = nullptr;

public:
    ~TextSystem() = default;
    static TextSystem* GetInstance();
    static void Initialize();
    static void Release();

public:
    const FontData* LoadFont(const std::string& filename);// 失敗したらnullptr
    const FontData& GetFont(const std::string& filename);
    const GlyphData* GetGlyphData(const std::string& fontName, int32_t codePoint);// フォント名と文字コードからグリフデータを取得
    const std::vector<std::string>& GetFontNames() { return fontNames; } // 読み込んだフォント名のリストを取得
    int TextCharFromUtf8(unsigned int* out_char, const char* in_text, const char* in_text_end); // UTF-8文字列から文字コードを取得
    std::vector<uint32_t> Utf8ToCodepoints(const std::string& utf8text); // UTF-8文字列から文字コードのリストを取得

private:// 読み込みに使用する内部関数
    std::vector<unsigned char> ParseTTF(const char* filename);

    uint32_t CreateFontAtlas(
        const std::string& fontName, FontData* fontData, 
        const stbtt_fontinfo& font, std::vector<int32_t>& codePoints,
        std::unordered_map<int32_t, GlyphData>& outGlyphs
    );

    uint32_t CreateFontTexture(
        const std::string& fontName,const uint8_t* fontBitmap,
        int width,int height
    );

private:
    std::unordered_map<std::string, std::unique_ptr<FontData>> fontDataMap_; // フォントデータのマップ
    std::vector<std::string> fontNames; // 読み込んだフォント名のリスト
    // 読み込んだTexture(フォントアトラス)のResource
    std::vector<ComPtr<ID3D12Resource>> textureResources;
    std::vector<ComPtr<ID3D12Resource>> intermediateResources;
    std::string directory_ = "Resources/Fonts/"; // フォントファイルのディレクトリ
};