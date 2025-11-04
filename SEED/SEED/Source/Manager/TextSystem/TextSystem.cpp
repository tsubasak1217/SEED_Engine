#include "TextSystem.h"
#include <SEED/Source/Manager/DxManager/ViewManager.h>
#include <execution>

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
    // フォントの起動時読み込み
    //instance_->StartupLoad();
}

///////////////////////////////////////////////////////////////////
// 起動時に読み込むフォント
///////////////////////////////////////////////////////////////////
void TextSystem::StartupLoad(){

    std::vector<std::string> fontFiles = MyFunc::GetFileList("Resources/Fonts/", { ".ttf",".otf" });

    // 存在するフォントをアトラスデータだけ先に読み込む
    for(const auto& fontFile : fontFiles){
        LoadFont(fontFile, false);
    }

    fontFiles;
}

/////////////////////////////////////////////////////////////////
// 終了処理
/////////////////////////////////////////////////////////////////
void TextSystem::Release(){
    if(instance_){
        instance_->textureResources.clear();
        instance_->intermediateResources.clear();
    }
}


/////////////////////////////////////////////////////////////////
// 文字データの取得
/////////////////////////////////////////////////////////////////
const GlyphData* TextSystem::GetGlyphData(const std::string& fontName, int32_t codePoint){
    // フォントデータが存在しない場合はnullptrを返す
    std::string fullFontName;
    if(fontName.starts_with("Resources")){
        fullFontName = fontName;
    } else{
        fullFontName = directory_ + fontName;
    }
    std::filesystem::path pathObj = fullFontName;
    std::string fontFileName = pathObj.filename().string();
    if(fontDataMap_.find(fontFileName) == fontDataMap_.end()){
        assert(false);
    }
    // フォントデータを取得
    const FontData& fontData = *fontDataMap_[fontFileName];

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
void TextSystem::CreateFontAtlas(
    FontAtlas& atlas,
    const std::string& atlasName,
    FontData* fontData,
    const stbtt_fontinfo& font,
    std::vector<int32_t>& codePoints
){
    int padding = 16;
    std::vector<unsigned char> atlasBitmap(FontAtlas::texWidth * FontAtlas::texHeight, 0);

    // 各種情報を取得
    float fontSize = 64.0f;
    float scale = stbtt_ScaleForPixelHeight(&font, fontSize);
    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(&font, &ascent, &descent, &lineGap);
    // baselineを計算
    int baseline = static_cast<int>(ascent * scale);
    fontData->baselneHeightRate = static_cast<float>(baseline) / fontSize;


    int penX = 0, penY = 0;
    int rowHeight = 0;

    for(int32_t codePoint : codePoints){
        int x0, y0, x1, y1;
        stbtt_GetCodepointBitmapBox(&font, codePoint, scale, scale, &x0, &y0, &x1, &y1);
        int glyphWidth = x1 - x0;
        int glyphHeight = y1 - y0;

        if(penX + glyphWidth >= FontAtlas::texWidth){
            penX = 0;
            penY += rowHeight + padding;
            rowHeight = 0;
        }

        // グリフビットマップ生成
        std::vector<unsigned char> bitmap(glyphWidth * glyphHeight);
        stbtt_MakeCodepointBitmap(&font, bitmap.data(), glyphWidth, glyphHeight, glyphWidth, scale, scale, codePoint);

        int drawY = penY + (baseline - y0); // baseline 揃え位置

        // アトラスに書き込み
        for(int yy = 0; yy < glyphHeight; ++yy){
            for(int xx = 0; xx < glyphWidth; ++xx){
                int dstIndex = (drawY + yy) * FontAtlas::texWidth + (penX + xx);
                if(dstIndex < FontAtlas::texWidth * FontAtlas::texHeight)
                    atlasBitmap[dstIndex] = bitmap[yy * glyphWidth + xx];
            }
        }

        // GlyphData 作成
        GlyphData glyph;

        glyph.texcoordLT = {
            static_cast<float>(penX) / FontAtlas::texWidth,
            static_cast<float>(drawY) / FontAtlas::texHeight
        };
        glyph.texcoordRB = {
            static_cast<float>(penX + glyphWidth) / FontAtlas::texWidth,
            static_cast<float>(drawY + glyphHeight) / FontAtlas::texHeight
        };

        Vector2 texSize = glyph.texcoordRB - glyph.texcoordLT;
        glyph.xRatio = (texSize.y == 0.0f) ? 0.0f : texSize.x / texSize.y;
        glyph.yRatio = static_cast<float>(glyphHeight) / fontSize;

        // yOffset（描画時に足すことで baseline 揃え）
        glyph.yOffset = static_cast<float>(y0) / fontSize;

        atlas.glyphs[codePoint] = glyph;

        penX += glyphWidth + padding;
        rowHeight = (std::max)(rowHeight, glyphHeight);
    }

    // アトラスの情報を保存
    atlas.bitmap = atlasBitmap;
    atlas.atlasName = atlasName;
    atlas.textureHandle = -1;
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
const FontData* TextSystem::LoadFont(const std::string& filename, bool isTextureCreate){
    // すでに読み込まれている場合はそのポインタを返す
    std::string fullPath;
    std::filesystem::path pathObj;

    if(filename.starts_with("Resources")){
        fullPath = filename; // すでにディレクトリが付加されている場合
    } else{
        fullPath = directory_ + filename; // ディレクトリを付加
    }

    // 読み込み状況を確認
    pathObj = fullPath;
    auto it = fontDataMap_.find(pathObj.filename().string());
    bool atlasCreated = false;
    if(it != fontDataMap_.end()){
        atlasCreated = true;
        // アトラスが作成されていてかつテクスチャが生成されていればそのまま返す
        if(it->second->atlases[0].textureHandle != -1){
            return it->second.get();
        }
    }

    if(!atlasCreated){
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
        constexpr size_t MAX_GLYPHS_PER_ATLAS = 4000;
        size_t offset = 0;
        int pages = 0;

        // フォントアトラスの生成
        while(offset < GlyphRanges::jpGlyph.size()){
            size_t batchSize = (std::min)(MAX_GLYPHS_PER_ATLAS, GlyphRanges::jpGlyph.size() - offset);
            std::vector<int32_t> codePoints(GlyphRanges::jpGlyph.begin() + offset, GlyphRanges::jpGlyph.begin() + offset + batchSize);

            // atlas を直接 emplace_back して構築（破棄されないように）
            fontData->atlases.emplace_back();
            FontAtlas* atlas = &fontData->atlases.back();

            std::string atlasName = fullPath + "_atlas" + std::to_string(pages++);
            CreateFontAtlas(*atlas, atlasName, fontData.get(), font, codePoints);

            // offsetを更新
            offset += batchSize;
        }

        // テクスチャの生成
        if(isTextureCreate){
            for(auto& atlas : fontData->atlases){
                atlas.textureHandle = CreateFontTexture(atlas.atlasName, atlas.bitmap.data(), FontAtlas::texWidth, FontAtlas::texHeight);
                // 並列に更新
                std::for_each(std::execution::par_unseq, atlas.glyphs.begin(), atlas.glyphs.end(), [&](auto& g){
                    g.second.graphHandle = atlas.textureHandle;
                });
            }
        }

        // フォントデータをマップに追加
        auto [insertedIt, success] = fontDataMap_.emplace(pathObj.filename().string(), std::move(fontData));

        // glyphをマップに追加(moveした後じゃないと無効になります)
        for(auto& atlas : insertedIt->second->atlases){
            // glyphIndex にも追加（ポインタを保存）
            for(auto& glyph : atlas.glyphs){
                insertedIt->second.get()->glyphDatas[glyph.first] = &glyph.second;
            }
        }

        return insertedIt->second.get();

    } else{
        // アトラスは作成されているがテクスチャがない場合
        if(isTextureCreate){
            auto fontData = it->second.get();
            for(auto& atlas : fontData->atlases){
                atlas.textureHandle = CreateFontTexture(atlas.atlasName, atlas.bitmap.data(), FontAtlas::texWidth, FontAtlas::texHeight);
                
                // 並列に更新
                std::for_each(std::execution::par_unseq, atlas.glyphs.begin(), atlas.glyphs.end(), [&](auto& g){
                    g.second.graphHandle = atlas.textureHandle;
                });
            }
            return fontData;
        } else{
            return it->second.get();
        }
    }
}


///////////////////////////////////////////////////////////////////
// フォントデータを取得する関数
///////////////////////////////////////////////////////////////////
const FontData& TextSystem::GetFont(const std::string& filename){

    std::string fullPath;
    if(filename.starts_with("Resources")){
        fullPath = filename; // すでにディレクトリが付加されている場合
    } else{
        fullPath = directory_ + filename; // ディレクトリを付加
    }

    std::filesystem::path pathObj = fullPath;
    std::string fontFileName = pathObj.filename().string();
    if(fontDataMap_.find(fontFileName) == fontDataMap_.end()){
        // フォントが読み込まれていない場合はロードを試みる
        LoadFont(filename);
    }

    // フォントデータを取得
    return *fontDataMap_[fontFileName];
}
