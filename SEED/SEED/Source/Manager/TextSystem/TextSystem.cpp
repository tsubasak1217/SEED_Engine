#include "TextSystem.h"
#include <SEED/Source/Manager/DxManager/ViewManager.h>
// stb
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

// インスタンスの取得
TextSystem* TextSystem::GetInstance(){
    if(!instance_){
        instance_ = new TextSystem();
    }
    return instance_;
}

/////////////////////////////////////////////////////////////////
// 初期化
/////////////////////////////////////////////////////////////////
void TextSystem::Initialize(){
    // インスタンスの取得
    GetInstance();
}

/////////////////////////////////////////////////////////////////
// 終了処理
/////////////////////////////////////////////////////////////////
void TextSystem::Release(){
    if(instance_){
        // テクスチャリソースの解放
        for(auto& resource : instance_->textureResources){
            resource->Release();
        }
        for(auto& intermediate : instance_->intermediateResources){
            intermediate->Release();
        }
    }
}


/////////////////////////////////////////////////////////////////
// 文字データの取得
/////////////////////////////////////////////////////////////////
const GlyphData* TextSystem::GetGlyphData(const std::string& fontName, int32_t codePoint){
    // フォントデータが存在しない場合はnullptrを返す
    if(fontDataMap_.find(directory_ + fontName) == fontDataMap_.end()){
        assert(false);
    }
    // フォントデータを取得
    const FontData& fontData = *fontDataMap_[directory_ + fontName];

    // 文字コードに対応するグリフデータを取得
    auto it = fontData.glyphDatas.find(codePoint);
    if(it != fontData.glyphDatas.end()){
        return it->second; // グリフデータを返す
    }

    // 存在しない場合は空のグリフデータを返す
    static GlyphData emptyGlyphData;
    return &emptyGlyphData;
}

///////////////////////////////////////////////////////////////////
// UTF-8文字列から文字コードのリストを取得する関数
///////////////////////////////////////////////////////////////////
int TextSystem::TextCharFromUtf8(unsigned int* out_char, const char* in_text, const char* in_text_end){
    unsigned int c = (unsigned int)-1;
    const unsigned char* str = (const unsigned char*)in_text;

    if(!in_text)
        return 0;

    if(str[0] < 0x80){
        c = (unsigned int)(str[0]);
        if(out_char) *out_char = c;
        return 1;
    } else if((str[0] & 0xe0) == 0xc0){
        if(in_text_end && in_text_end - (const char*)str < 2) return 0;
        if((str[1] & 0xc0) != 0x80) return 0;
        c = ((str[0] & 0x1f) << 6) | (str[1] & 0x3f);
        if(out_char) *out_char = c;
        return 2;
    } else if((str[0] & 0xf0) == 0xe0){
        if(in_text_end && in_text_end - (const char*)str < 3) return 0;
        if((str[1] & 0xc0) != 0x80 || (str[2] & 0xc0) != 0x80) return 0;
        c = ((str[0] & 0x0f) << 12) | ((str[1] & 0x3f) << 6) | (str[2] & 0x3f);
        if(out_char) *out_char = c;
        return 3;
    } else if((str[0] & 0xf8) == 0xf0){
        if(in_text_end && in_text_end - (const char*)str < 4) return 0;
        if((str[1] & 0xc0) != 0x80 || (str[2] & 0xc0) != 0x80 || (str[3] & 0xc0) != 0x80) return 0;
        c = ((str[0] & 0x07) << 18) | ((str[1] & 0x3f) << 12) | ((str[2] & 0x3f) << 6) | (str[3] & 0x3f);
        if(out_char) *out_char = c;
        return 4;
    }

    return 0;
}

std::vector<uint32_t> TextSystem::Utf8ToCodepoints(const std::string& text){
    std::vector<uint32_t> codepoints;
    const char* p = text.c_str();
    const char* end = p + text.size();

    while(p < end){
        unsigned int cp = 0;
        int bytes = TextCharFromUtf8(&cp, p, end);
        if(bytes <= 0) break; // 不正なUTF-8文字
        codepoints.push_back(cp);
        p += bytes;
    }

    return codepoints;
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
// フォントアトラスを作成する関数
///////////////////////////////////////////////////////////////////
uint32_t TextSystem::CreateFontAtlas(
    const std::string& fontName,
    const stbtt_fontinfo& font,
    std::vector<int32_t>& codePoints,
    std::unordered_map<int32_t, GlyphData>& outGlyphs
){
    const int texWidth = 1024, texHeight = 1024;
    std::vector<unsigned char> atlasBitmap(texWidth * texHeight, 0);

    stbtt_pack_context pc;
    stbtt_PackBegin(&pc, atlasBitmap.data(), texWidth, texHeight, 0, 1, nullptr);

    int32_t size = static_cast<int32_t>(codePoints.size());
    std::vector<stbtt_packedchar> packedChars(size);
    stbtt_pack_range range{};
    range.font_size = 32.0f;
    range.first_unicode_codepoint_in_range = 0;
    range.array_of_unicode_codepoints = codePoints.data();
    range.num_chars = size;
    range.chardata_for_range = packedChars.data();

    stbtt_PackSetOversampling(&pc, 2, 2);
    stbtt_PackFontRanges(&pc, font.data, 0, &range, 1);
    stbtt_PackEnd(&pc);

    // フォントの共通スケールとベースラインを計算
    float fontSize = range.font_size;
    float scale = stbtt_ScaleForPixelHeight(&font, fontSize);
    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(&font, &ascent, &descent, &lineGap);
    float baseline = ascent * scale;

    // テクスチャ化
    uint32_t textureHandle = CreateFontTexture(fontName, atlasBitmap.data(), texWidth, texHeight);

    // 出力 GlyphData を構築
    for(size_t i = 0; i < size; ++i){
        int codePoint = codePoints[i];
        const auto& ch = packedChars[i];

        GlyphData glyphData;
        glyphData.graphHandle = textureHandle;

        glyphData.texcoordLT = {
            static_cast<float>(ch.x0) / texWidth,
            static_cast<float>(ch.y0) / texHeight
        };
        glyphData.texcoordRB = {
            static_cast<float>(ch.x1) / texWidth,
            static_cast<float>(ch.y1) / texHeight
        };

        // サイズ比（横幅 / 縦幅）
        Vector2 _size = glyphData.texcoordRB - glyphData.texcoordLT;
        glyphData.yRatio = (_size.y == 0.0f) ? 0.0f : (_size.y / fontSize); // yRatioはフォントサイズに対する比率
        glyphData.xRatio = (_size.y == 0.0f) ? 0.0f : (_size.x / _size.y);

        // ベースラインからのオフセットを計算
        int x0, y0, x1, y1;
        stbtt_GetCodepointBitmapBox(&font, codePoint, scale, scale, &x0, &y0, &x1, &y1);
        glyphData.yOffset = baseline + static_cast<float>(y0); // ← ここがポイント！

        outGlyphs[codePoint] = glyphData;
    }

    return textureHandle;
}

///////////////////////////////////////////////////////////////////
// フォントテクスチャリソースを作成する関数
///////////////////////////////////////////////////////////////////
uint32_t TextSystem::CreateFontTexture(const std::string& fontName, const uint8_t* fontBitmap, int width, int height){

    // 重複チェック（例：フォント名 + "_atlas0"など）
    if(ViewManager::GetTextureHandle(fontName) != -1){
        return ViewManager::GetTextureHandle(fontName);
    }

    // メタデータの構築（自前で）
    DirectX::TexMetadata metadata{};
    metadata.width = width;
    metadata.height = height;
    metadata.depth = 1;
    metadata.arraySize = 1;
    metadata.mipLevels = 1;
    metadata.format = DXGI_FORMAT_R8_UNORM;// stbで生成されるのは1チャンネルのビットマップなので R8_UNORM
    metadata.dimension = DirectX::TEX_DIMENSION_TEXTURE2D;

    // ScratchImage にビットマップをコピー
    DirectX::ScratchImage image;
    image.Initialize2D(metadata.format, width, height, 1, 1);

    uint8_t* dst = image.GetImages()->pixels;
    memcpy(dst, fontBitmap, width * height); // stb の 1ch ビットマップ

    // GPU用リソース作成・アップロード
    textureResources.push_back(CreateTextureResource(DxManager::GetInstance()->GetDevice(), metadata));
    intermediateResources.push_back(
        UploadTextureData(
            textureResources.back().Get(), image,
            DxManager::GetInstance()->GetDevice(),
            DxManager::GetInstance()->commandList.Get()
        )
    );

    // SRV作成
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = metadata.format;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;

    return ViewManager::CreateView(VIEW_TYPE::SRV, textureResources.back().Get(), &srvDesc, fontName);
}

///////////////////////////////////////////////////////////////////
// フォントを読み込む
///////////////////////////////////////////////////////////////////
const FontData* TextSystem::LoadFont(const std::string& filename){
    // すでに読み込まれている場合はそのポインタを返す
    std::string fullPath = directory_ + filename; // ディレクトリを付加
    auto it = fontDataMap_.find(fullPath);
    if(it != fontDataMap_.end()){
        return it->second.get();
    }

    // フォントファイル読み込み（バイナリとして）
    std::vector<unsigned char> fontBuffer = ParseTTF(fullPath.c_str());
    if(fontBuffer.empty()){
        return nullptr; // 読み込み失敗
    }

    // stb_truetype 初期化
    stbtt_fontinfo font;
    stbtt_InitFont(&font, fontBuffer.data(), 0);


    // ここで複数アトラスに分けて読み込む
    std::unique_ptr<FontData> fontData = std::make_unique<FontData>();
    constexpr size_t MAX_GLYPHS_PER_ATLAS = 1024;
    size_t offset = 0;
    int pages = 0;


    while(offset < GlyphRanges::jpGlyph.size()){
        size_t batchSize = (std::min)(MAX_GLYPHS_PER_ATLAS, GlyphRanges::jpGlyph.size() - offset);
        std::vector<int32_t> codePoints(GlyphRanges::jpGlyph.begin() + offset, GlyphRanges::jpGlyph.begin() + offset + batchSize);

        // atlas を直接 emplace_back して構築（破棄されないように）
        fontData->atlases.emplace_back();
        FontAtlas* atlas = &fontData->atlases.back();

        std::string atlasName = fullPath + "_atlas" + std::to_string(pages++);
        atlas->textureHandle = CreateFontAtlas(atlasName, font, codePoints, atlas->glyphs);

        // offsetを更新
        offset += batchSize;
    }

    // フォントデータをマップに追加
    auto [insertedIt, success] = fontDataMap_.emplace(fullPath, std::move(fontData));

    // glyphをマップに追加(moveした後じゃないと無効になります)
    for(auto& atlas : insertedIt->second->atlases){
        // glyphIndex にも追加（ポインタを保存）
        for(auto& glyph : atlas.glyphs){
            insertedIt->second.get()->glyphDatas[glyph.first] = &glyph.second;
        }
    }

    return insertedIt->second.get();
}