#include "TextBox.h"
#include <SEED/Source/SEED.h>
#include <SEED/Source/Manager/TextSystem/TextSystem.h>
#include <SEED/Lib/Functions/MyFunc/ShapeMath.h>

// 2Dテキスト
void TextBox2D::Draw()const{

    // 変換行列を作成
    Vector2 anchorOffset = anchorPos * (transform.scale_ * size);
    Matrix3x3 textBoxMat = AffineMatrix(
        transform.scale_, transform.rotate_, transform.translate_ + anchorOffset
    );

    // 文字コードの一覧を取得
    auto codePoints = TextSystem::GetInstance()->Utf8ToCodepoints(text);
    // 行の文字データ
    std::vector<const GlyphData*> lineGlyphs;
    // オフセット計算用
    int lineCount = 0;
    float totalWidth = 0.0f;


    // テキストを行ごとに分割して描画
    for(size_t i = 0; i < codePoints.size(); i++){

        // 文字データを取得
        const GlyphData* glyph = TextSystem::GetInstance()->GetGlyphData(fontName, codePoints[i]);

        // 文字の幅を計算
        float glyphWidth = fontSize * glyph->xRatio;

        // 行がいっぱいになる or 最後の文字に達したら矩形を作成して次の行へ
        if(totalWidth + glyphWidth + glyphSpacing > size.x or i == codePoints.size() - 1){

            // 左端X座標を計算
            float leftX = (size.x - totalWidth) * 0.5f;
            float curX = leftX;

            // 最終的に描画する矩形
            std::vector<Quad2D> quads;

            // 矩形を追加していく
            for(const auto& lineGlyph : lineGlyphs){
                auto& quad = quads.emplace_back(
                    MakeQuad2D({ fontSize * lineGlyph->xRatio, fontSize }, { 1.0f,1.0f,1.0f,1.0f }, { 0.0f,1.0f })
                );

                // 座標を設定
                for(int idx = 0; idx < 4; idx++){
                    // ローカルな位置
                    quad.localVertex[idx] += Vector2({
                        curX,
                        lineCount * (fontSize + lineSpacing),
                        });
                    // アンカーオフセットを適用
                    quad.localVertex[idx] -= anchorOffset;
                    // 行列を適用
                    quad.localVertex[idx] *= textBoxMat;
                    // offsetを元に戻す
                    quad.localVertex[idx] -= anchorOffset;
                }

                // テクスチャ座標を設定
                quad.texCoord[0] = lineGlyph->texcoordLT;
                quad.texCoord[1] = { lineGlyph->texcoordRB.x, lineGlyph->texcoordLT.y };
                quad.texCoord[2] = { lineGlyph->texcoordLT.x, lineGlyph->texcoordRB.y };
                quad.texCoord[3] = lineGlyph->texcoordRB;
                // グラフハンドルを設定
                quad.GH = lineGlyph->graphHandle;
                // 色を設定
                quad.color = color;
                // 描画
                quad.isText = true;
                SEED::DrawQuad2D(quad);
                // X座標オフセットを加算
                curX += fontSize * lineGlyph->xRatio + glyphSpacing;
            }

            lineCount++;
            totalWidth = 0.0f;
            lineGlyphs.clear(); // 次の行のためにクリア

        }

        // まだ行に収まる場合
        totalWidth += glyphWidth + glyphSpacing;
        lineGlyphs.push_back(glyph);

    }

    // テキストボックスの範囲描画
    static Vector2 vertices[4] = {};

}

void TextBox2D::SetFont(const std::string& fileName){
    TextSystem::GetInstance()->LoadFont(fileName);
    this->fontName = fileName;
}


// 3Dテキスト
void TextBox3D::Draw()const{

    // 変換行列を作成
    Vector3 anchorOffset = Vector3({ anchorPos.x,-anchorPos.y,1.0f }) * (transform.scale_ * Vector3({ size.x,-size.y,1.0f }));
    Matrix4x4 textBoxMat = AffineMatrix(
        transform.scale_, transform.rotate_, transform.translate_ + anchorOffset
    );
    // 文字コードの一覧を取得
    auto codePoints = TextSystem::GetInstance()->Utf8ToCodepoints(text);
    // 行の文字データ
    std::vector<const GlyphData*> lineGlyphs;
    // オフセット計算用
    int lineCount = 0;
    float totalWidth = 0.0f;


    // テキストを行ごとに分割して描画
    for(size_t i = 0; i < codePoints.size(); i++){
        // 文字データを取得
        const GlyphData* glyph = TextSystem::GetInstance()->GetGlyphData(fontName, codePoints[i]);
        // 文字の幅を計算
        float glyphWidth = fontSize * glyph->xRatio;
        // 行がいっぱいになる or 最後の文字に達したら矩形を作成して次の行へ
        if(totalWidth + glyphWidth + glyphSpacing > size.x or i == codePoints.size() - 1){
            // 左端X座標を計算
            float leftX = (size.x - totalWidth) * 0.5f;
            float curX = leftX;
            // 最終的に描画する矩形
            std::vector<Quad> quads;
            // 矩形を追加していく
            for(const auto& lineGlyph : lineGlyphs){
                auto& quad = quads.emplace_back(
                    MakeQuad({ fontSize * lineGlyph->xRatio, fontSize }, { 1.0f,1.0f,1.0f,1.0f }, { 0.0f,0.0f })
                );
                // 座標を設定
                for(int idx = 0; idx < 4; idx++){
                    // ローカルな位置
                    quad.localVertex[idx] += Vector3({
                        curX,
                        lineCount * (fontSize + lineSpacing),
                        0.0f,
                        });
                    // アンカーオフセットを適用
                    quad.localVertex[idx] -= anchorOffset;
                    // 行列を適用
                    quad.localVertex[idx] *= textBoxMat;
                    // offsetを元に戻す
                    quad.localVertex[idx] -= anchorOffset;
                }

                // テクスチャ座標を設定
                quad.texCoord[0] = lineGlyph->texcoordLT;
                quad.texCoord[1] = { lineGlyph->texcoordRB.x, lineGlyph->texcoordLT.y };
                quad.texCoord[2] = { lineGlyph->texcoordLT.x, lineGlyph->texcoordRB.y };
                quad.texCoord[3] = lineGlyph->texcoordRB;

                // グラフハンドルを設定
                quad.GH = lineGlyph->graphHandle;
                // 色を設定
                quad.color = color;
                // 描画
                quad.isText = true;
                SEED::DrawQuad(quad);
                // X座標オフセットを加算
                curX += fontSize * lineGlyph->xRatio + glyphSpacing;
            }

            lineCount++;
            totalWidth = 0.0f;
        }

        // まだ行に収まる場合
        totalWidth += glyphWidth + glyphSpacing;
        lineGlyphs.push_back(glyph);
    }
}
