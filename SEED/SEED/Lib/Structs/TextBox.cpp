#include "TextBox.h"
#include <SEED/Source/SEED.h>
#include <SEED/Source/Manager/TextSystem/TextSystem.h>
#include <SEED/Lib/Functions/MyFunc/ShapeMath.h>

#ifdef _DEBUG
void TextBox2D::Edit(){
    // beginされているウインドウ内で使うこと
    ImFunc::InputTextMultiLine("テキスト", text);
    auto fontNames = MyFunc::GetFileList("Resources/fonts", { ".ttf", ".otf" });
    ImFunc::ComboText("フォント", fontName, fontNames);
    ImFunc::Combo<TextAlign>("配置", align, { "LEFT","CENTER","RIGHT" });
    ImGui::DragFloat("フォントサイズ(縦幅基準)", &fontSize, 1.0f, 1.0f, 100.0f);
    ImGui::DragFloat2("テキスト位置", &transform.translate.x, 1.0f);
    ImGui::DragFloat2("ボックスサイズ", &size.x, 1.0f, 0.0f);
    ImGui::DragFloat2("アンカーポイント", &anchorPos.x, 0.01f);
    ImGui::DragFloat("回転", &transform.rotate, 0.01f);
    ImGui::DragFloat2("スケール", &transform.scale.x, 0.01f, 0.0f);
    ImGui::DragFloat("行間", &lineSpacing, 0.1f);
    ImGui::DragFloat("文字間隔", &glyphSpacing, 0.1f);
    ImGui::ColorEdit4("文字色", (float*)&color);
    ImGui::Checkbox("テキストボックス表示", &textBoxVisible);
    ImGui::Checkbox("アウトライン", &useOutline);
    if(useOutline){
        ImGui::DragFloat("アウトライン幅", &outlineWidth, 0.1f, 0.0f);
        ImGui::ColorEdit4("アウトライン色", (float*)&outlineColor);
        ImGui::DragInt("アウトライン分割数", &outlineSplitCount, 1, 1, 64);
    }
}

// jsonデータの取得
nlohmann::json TextBox2D::GetJsonData() const{
    nlohmann::json jsonData;
    jsonData["text"] = text;
    jsonData["fontName"] = fontName;
    jsonData["transform"]["translate"] = transform.translate;
    jsonData["transform"]["rotate"] = transform.rotate;
    jsonData["transform"]["scale"] = transform.scale;
    jsonData["size"] = size;
    jsonData["anchorPos"] = anchorPos;
    jsonData["fontSize"] = fontSize;
    jsonData["lineSpacing"] = lineSpacing;
    jsonData["glyphSpacing"] = glyphSpacing;
    jsonData["align"] = static_cast<int>(align);
    jsonData["blendMode"] = static_cast<int>(blendMode);
    jsonData["useOutline"] = useOutline;
    jsonData["outlineWidth"] = outlineWidth;
    jsonData["outlineSplitCount"] = outlineSplitCount;
    jsonData["color"] = color;
    jsonData["outlineColor"] = outlineColor;
    return jsonData; // JSONデータを返す
}

// jsonデータからの読み込み
void TextBox2D::LoadFromJson(const nlohmann::json& jsonData){
    text = jsonData["text"];
    fontName = jsonData["fontName"];
    transform.translate = jsonData["transform"]["translate"];
    transform.rotate = jsonData["transform"]["rotate"];
    transform.scale = jsonData["transform"]["scale"];
    size = jsonData["size"];
    anchorPos = jsonData["anchorPos"];
    fontSize = jsonData["fontSize"];
    lineSpacing = jsonData["lineSpacing"];
    glyphSpacing = jsonData["glyphSpacing"];
    align = static_cast<TextAlign>(jsonData["align"]);
    blendMode = static_cast<BlendMode>(jsonData["blendMode"]);
    useOutline = jsonData["useOutline"];
    outlineWidth = jsonData["outlineWidth"];
    outlineSplitCount = jsonData["outlineSplitCount"];
    color = jsonData["color"];
    outlineColor = jsonData["outlineColor"];
    // フォントを設定
    SetFont(fontName);
}

#endif // _DEBUG

////////////////////////////////////////////////////////////////////////////////////////////////////////
//  テクストからトークンを解析して新しい文字列を返す関数
////////////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<std::string> TextBox2D::AnalyzeFormatToken(const std::string& sourceText)const{
    // %d, %f, %s などのフォーマット文字列があればバインドデータに置き換える
    std::string convertedText;
    int offset = 0; // 解析位置のオフセット

    // 先頭から文字列を解析
    for(size_t i = 0; i < sourceText.size(); i++){
        if(sourceText[i] == '%'){
            // フォーマット文字列の開始
            if(i + 1 < sourceText.size()){
                // 次の文字を確認
                if(bindedDatas.size() <= offset){
                    // バインドデータが足りない場合はそのまま追加
                    convertedText += sourceText[i];
                    convertedText += sourceText[i + 1];
                    i++; // 次の文字をスキップ

                } else{
                    if(sourceText[i + 1] == 'd'){ // 整数
                        // データがint&であることを確認
                        if(!std::holds_alternative<std::reference_wrapper<int>>(bindedDatas[offset])){
                            convertedText += sourceText[i];
                            convertedText += sourceText[i + 1];
                            i++; // 次の文字をスキップ
                            continue;
                        }

                        convertedText += std::to_string(std::get<std::reference_wrapper<int>>(bindedDatas[offset]).get());
                        offset++;
                        i++; // 次の文字をスキップ

                    } else if(sourceText[i + 1] == 'f'){ // 浮動小数点数
                        if(!std::holds_alternative<std::reference_wrapper<float>>(bindedDatas[offset])){
                            convertedText += sourceText[i];
                            convertedText += sourceText[i + 1];
                            i++; // 次の文字をスキップ
                            continue;
                        }
                        // 小数点以下の桁数を制限する
                        std::string fStr = std::to_string(std::get<std::reference_wrapper<float>>(bindedDatas[offset]).get());
                        size_t dotPos = fStr.find('.');
                        if(dotPos != std::string::npos){
                            fStr = fStr.substr(0, dotPos + 3); // 小数点以下2桁まで
                        }
                        convertedText += fStr;
                        offset++;
                        i++; // 次の文字をスキップ

                    } else if(sourceText[i + 1] == 's'){ // 文字列
                        if(!std::holds_alternative<std::reference_wrapper<std::string>>(bindedDatas[offset])){
                            convertedText += sourceText[i];
                            convertedText += sourceText[i + 1];
                            i++; // 次の文字をスキップ
                            continue;
                        }
                        convertedText += std::get<std::reference_wrapper<std::string>>(bindedDatas[offset]).get();
                        offset++;
                        i++; // 次の文字をスキップ
                    } else{
                        convertedText += sourceText[i]; // %以外はそのまま追加
                    }
                }
            } else{
                convertedText += sourceText[i]; // %の後に文字がない場合はそのまま追加}
            }

        } else{
            convertedText += sourceText[i]; // フォーマット文字列以外はそのまま追加
        }
    }

    // '\n'区切りで行ごとに分割(文字列としての"\n"も判別)
    std::vector<std::string> lines;
    size_t start = 0;
    size_t end = convertedText.find('\n');
    while(end != std::string::npos){
        lines.push_back(convertedText.substr(start, end - start));
        start = end + 1; // 次の行の開始位置
        end = convertedText.find('\n', start);
    }

    // 最後の行を追加
    if(start < convertedText.size()){
        lines.push_back(convertedText.substr(start));
    }

    return lines; // 行ごとの文字列のリストを返す
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
//  2Dテキスト描画
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void TextBox2D::Draw()const{

    // 変換行列を作成
    static Vector2 baseAnchorPos = { 0.5f, 0.5f }; // デフォルトのアンカー位置
    Vector2 anchorOffset = (anchorPos - baseAnchorPos) * size;
    Matrix3x3 textBoxMat = AffineMatrix(transform);

    // デバッグ時描画
#ifdef _DEBUG
    if(textBoxVisible){
        // テキストボックスの範囲描画
        static Vector2 vertices[4] = {};
        Vector2 halfSize = size * 0.5f;
        // ローカル頂点
        vertices[0] = { -halfSize.x, -halfSize.y };
        vertices[1] = { halfSize.x, -halfSize.y };
        vertices[2] = { -halfSize.x, halfSize.y };
        vertices[3] = { halfSize.x, halfSize.y };
        // アンカーオフセット,行列を適用
        for(int i = 0; i < 4; i++){
            vertices[i] -= anchorOffset;
            vertices[i] *= textBoxMat;
        }
        // テキストボックスのライン描画
        SEED::DrawLine2D(vertices[0], vertices[1], color);
        SEED::DrawLine2D(vertices[1], vertices[3], color);
        SEED::DrawLine2D(vertices[3], vertices[2], color);
        SEED::DrawLine2D(vertices[2], vertices[0], color);

        // アンカー位置の描画(常にtranslateの位置に描画)
        Triangle2D anchorTri = MakeEqualTriangle2D(10.0f, color);
        anchorTri.translate = transform.translate;
        anchorTri.rotate = transform.rotate;
        SEED::DrawTriangle2D(anchorTri);

    }
#endif // _DEBUG

    // 文字・フォントが設定されていない場合は何もしない
    if(fontName.empty()){ return; }
    if(text.empty()){ return; }

    // 文字列を解析・分解
    std::vector<std::string> lines = AnalyzeFormatToken(text);
    std::vector<std::vector<unsigned int>> codePointsList; // 各行の文字コードリスト

    for(auto& line : lines){
        // 各行の文字コードを取得
        codePointsList.emplace_back(TextSystem::GetInstance()->Utf8ToCodepoints(line));
    }


    // 行の文字データ
    std::vector<const GlyphData*> lineGlyphs;
    // オフセット計算用
    int lineCount = 0;
    float totalWidth = 0.0f;
    const FontData& fontData = TextSystem::GetInstance()->GetFont(fontName);
    float heightOffset = fontSize * fontData.baselneHeightRate; // ベースラインのオフセット


    //====================== テキストを行ごとに分割して描画 =========================//
    for(auto& codePoints : codePointsList){

        for(size_t i = 0; i < codePoints.size(); i++){

            // 文字データを取得
            const GlyphData* glyph = TextSystem::GetInstance()->GetGlyphData(fontName, codePoints[i]);

            // 文字のサイズを計算
            float glyphHaight = fontSize * glyph->yRatio;
            float glyphWidth = glyphHaight * glyph->xRatio;

            // 文字を進める
            totalWidth += glyphWidth + glyphSpacing;
            lineGlyphs.push_back(glyph);

            //========== 行がいっぱいになる or 最後の文字に達したら矩形を作成して次の行へ ==========//
            if(totalWidth + glyphWidth + glyphSpacing > size.x or i == codePoints.size() - 1){

                // 字詰めモードに応じて左端座標を調整
                float leftX = 0.0f;
                if(align == TextAlign::LEFT){// 左寄せ
                    leftX = 0.0f;
                } else if(align == TextAlign::CENTER){// 中央寄せ
                    leftX = (size.x - totalWidth) * 0.5f;
                } else if(align == TextAlign::RIGHT){// 右寄せ
                    leftX = size.x - totalWidth;
                }
                float curX = leftX;

                // 最終的に描画する矩形
                std::vector<Quad2D> quads;

                // 矩形を追加していく
                for(const auto& lineGlyph : lineGlyphs){
                    auto& quad = quads.emplace_back(
                        MakeQuad2D({ fontSize * lineGlyph->yRatio * lineGlyph->xRatio, fontSize * lineGlyph->yRatio }, { 1.0f,1.0f,1.0f,1.0f }, { 0.0f,0.0f })
                    );

                    // 座標を設定
                    for(int idx = 0; idx < 4; idx++){
                        // ローカルな位置
                        quad.localVertex[idx] += Vector2({
                            curX,
                            lineCount * (fontSize + lineSpacing) + fontSize * (lineGlyph->yOffset) + heightOffset,
                            });
                        // アンカーオフセットを適用
                        quad.localVertex[idx] -= (baseAnchorPos * size) + anchorOffset;
                        // 行列を適用
                        quad.localVertex[idx] *= textBoxMat;
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
                    // その他設定
                    quad.isText = true;
                    quad.layer = 1;

                    if(useOutline){
                        Matrix3x3 rotateMat = RotateMatrix(transform.rotate);
                        Vector2 rotatedVec = Vector2(0.0f, 1.0f) * rotateMat;
                        float radianEvery = (3.14f * 2.0f) / outlineSplitCount;
                        Quad2D outlineQuad = quad;
                        outlineQuad.color = outlineColor;
                        outlineQuad.layer = 0; // アウトラインは下に描画するためレイヤーを0に設定

                        // アウトラインを移動させて描画
                        for(int j = 0; j < outlineSplitCount; j++){
                            // アウトラインの位置を計算
                            Vector2 offset = rotatedVec * outlineWidth;
                            outlineQuad.localVertex[0] = quad.localVertex[0] + offset;
                            outlineQuad.localVertex[1] = quad.localVertex[1] + offset;
                            outlineQuad.localVertex[2] = quad.localVertex[2] + offset;
                            outlineQuad.localVertex[3] = quad.localVertex[3] + offset;
                            // アウトラインの色を設定
                            SEED::DrawQuad2D(outlineQuad);
                            // 回転ベクトルを更新
                            rotatedVec *= RotateMatrix(radianEvery);
                        }
                    }

                    // 描画
                    SEED::DrawQuad2D(quad);
                    
                    // X座標オフセットを加算
                    float height = fontSize * lineGlyph->yRatio;
                    curX += height * lineGlyph->xRatio + glyphSpacing;
                }

                lineCount++;
                totalWidth = 0.0f;
                lineGlyphs.clear(); // 次の行のためにクリア
            }
        }
    }
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////
//  3Dテキスト描画
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void TextBox3D::Draw()const{

    // 変換行列を作成
    Vector3 anchorOffset = Vector3({ anchorPos.x,-anchorPos.y,1.0f }) * (transform.scale * Vector3({ size.x,-size.y,1.0f }));
    Matrix4x4 textBoxMat = AffineMatrix(
        transform.scale, transform.rotate, transform.translate + anchorOffset
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


//////////////////////////////////////////////////////////////////
// フォントを設定
//////////////////////////////////////////////////////////////////
void TextBox2D::SetFont(const std::string& fileName){
    TextSystem::GetInstance()->LoadFont(fileName);
    this->fontName = fileName;
}

//////////////////////////////////////////////////////////////////
// バインドデータを設定
//////////////////////////////////////////////////////////////////
void TextBox2D::BindDatas(std::initializer_list<BindData> datas){
    bindedDatas.clear();
    for(auto& data : datas){
        bindedDatas.push_back(data);
    }
}
