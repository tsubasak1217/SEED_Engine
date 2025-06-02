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
    static const TextSystem* GetInstance();
    static void Initialize();

public:
    const FontData* LoadFont(const std::string& filename);// 失敗したらnullptr
    const GlyphData& GetGlyphData(const std::string& fontName, int32_t codePoint);// フォント名と文字コードからグリフデータを取得

private:
    std::vector<unsigned char> ParseTTF(const char* filename);

private:
    std::unordered_map<std::string, std::unique_ptr<FontData>> fontDataMap_; // フォントデータのマップ
};