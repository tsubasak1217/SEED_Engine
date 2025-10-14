#include "TextBox.h"
#include <SEED/Source/SEED.h>
#include <SEED/Source/Manager/TextSystem/TextSystem.h>
#include <SEED/Lib/Functions/MyFunc/ShapeMath.h>

#ifdef _DEBUG
void TextBox2D::Edit(const std::string& hash){
    // text内容の編集(ctrl+enterで改行)
    ImFunc::InputTextMultiLine("テキスト##" + hash, text, ImGuiInputTextFlags_CtrlEnterForNewLine);

    // フォント設定
    static std::filesystem::path currentDir = "Resources/Fonts";
    std::string selectedFont = ImFunc::FolderView("フォント設定##" + hash, currentDir, false, { ".ttf",".otf" }, "Resources/Fonts");
    if(!selectedFont.empty()){
        SetFont(selectedFont);
    }

    // 各種パラメータ設定
    if(ImGui::CollapsingHeader("トランスフォーム##" + hash)){
        ImGui::Indent();
        static bool guizmo = false;
        ImGui::Checkbox("Gizmoで操作##" + hash, &guizmo);
        ImGui::DragFloat2("スケール##" + hash, &transform.scale.x, 0.01f);
        ImGui::DragFloat("回転##" + hash, &transform.rotate, 0.05f);
        ImGui::DragFloat2("移動##" + hash, &transform.translate.x);
        if(guizmo){
            if(!parentMat){
                ImGuiManager::RegisterGuizmoItem(&transform);
            } else{
                ImGuiManager::RegisterGuizmoItem(&transform, parentMat->ToMat4x4());
            }
        }
        ImGui::Unindent();
    }

    if(ImGui::CollapsingHeader("配置や大きさ、アンカーなどの設定##" + hash)){
        ImGui::Indent();
        ImFunc::Combo<TextAlignX>("配置(X)##" + hash, alignX, { "LEFT","CENTER","RIGHT" });
        ImFunc::Combo<TextAlignY>("配置(Y)##" + hash, alignY, { "TOP","CENTER","BOTTOM" });
        ImGui::DragFloat("フォントサイズ(縦幅基準)##" + hash, &fontSize, 1.0f, 1.0f, 1000.0f);
        ImGui::DragFloat2("ボックスサイズ##" + hash, &size.x, 1.0f, 0.0f);
        ImGui::DragFloat2("アンカーポイント##" + hash, &anchorPos.x, 0.01f);
        ImGui::DragFloat("行間##" + hash, &lineSpacing, 0.1f);
        ImGui::DragFloat("文字間隔##" + hash, &glyphSpacing, 0.1f);
        ImGui::Unindent();
    }

    if(ImGui::CollapsingHeader("描画設定##" + hash)){
        ImGui::Indent();
        ImFunc::Combo<BlendMode>("ブレンドモード##" + hash, blendMode, { "NONE","0MUL" ,"SUB","NORMAL","ADD","SCREEN" });
        ImFunc::Combo<DrawLocation>("描画位置##" + hash, drawLocation, { "背景","前景" }, 1);
        ImGui::DragInt("描画レイヤー##" + hash, &layer);
        ImGui::ColorEdit4("文字色##" + hash, (float*)&color);
        ImGui::Checkbox("テキストボックス表示##" + hash, &textBoxVisible);
        ImGui::Checkbox("ビュー行列を適用##" + hash, &isApplyViewMat);
        ImGui::Checkbox("アウトライン##" + hash, &useOutline);
        if(useOutline){
            ImGui::DragFloat("アウトライン幅##" + hash, &outlineWidth, 0.1f, 0.0f);
            ImGui::ColorEdit4("アウトライン色##" + hash, (float*)&outlineColor);
            ImGui::DragInt("アウトライン分割数##" + hash, &outlineSplitCount, 1, 1, 64);
        }
        ImGui::Unindent();
    }
}
#endif // _DEBUG

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
    jsonData["alignX"] = static_cast<int>(alignX);
    jsonData["alignY"] = static_cast<int>(alignY);
    jsonData["blendMode"] = static_cast<int>(blendMode);
    jsonData["useOutline"] = useOutline;
    jsonData["outlineWidth"] = outlineWidth;
    jsonData["outlineSplitCount"] = outlineSplitCount;
    jsonData["color"] = color;
    jsonData["outlineColor"] = outlineColor;
    jsonData["textBoxVisible"] = textBoxVisible;
    jsonData["isApplyViewMat"] = isApplyViewMat;
    jsonData["layer"] = layer;
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
    alignX = static_cast<TextAlignX>(jsonData.value("alignX",1));
    alignY = static_cast<TextAlignY>(jsonData.value("alignY",0));
    blendMode = static_cast<BlendMode>(jsonData["blendMode"]);
    useOutline = jsonData["useOutline"];
    outlineWidth = jsonData["outlineWidth"];
    outlineSplitCount = jsonData["outlineSplitCount"];
    color = jsonData["color"];
    outlineColor = jsonData["outlineColor"];
    textBoxVisible = jsonData.value("textBoxVisible", true);
    isApplyViewMat = jsonData.value("isApplyViewMat", false);
    layer = jsonData.value("layer", 0);
    // フォントを設定
    SetFont(fontName);
}


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
void TextBox2D::Draw(const std::optional<Vector4>& masterColor)const{

    // マスターカラーが指定されていれば乗算
    Vector4 _color = this->color;
    if(masterColor != std::nullopt){
        _color *= masterColor.value();
    }

    // 変換行列を作成
    static Vector2 baseAnchorPos = { 0.5f, 0.5f }; // デフォルトのアンカー位置
    Vector2 anchorOffset = (anchorPos - baseAnchorPos) * size;
    Matrix3x3 textBoxMat;

    if(parentMat){
        textBoxMat = transform.ToMatrix() * (*parentMat);
    } else{
        textBoxMat = transform.ToMatrix();
    }

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
        SEED::DrawLine2D(vertices[0], vertices[1], _color);
        SEED::DrawLine2D(vertices[1], vertices[3], _color);
        SEED::DrawLine2D(vertices[3], vertices[2], _color);
        SEED::DrawLine2D(vertices[2], vertices[0], _color);

        // アンカー位置の描画(常にtranslateの位置に描画)
        Triangle2D anchorTri = MakeEqualTriangle2D(10.0f, _color);
        anchorTri.translate = ExtractTranslation(textBoxMat);
        anchorTri.rotate = ExtractRotation(textBoxMat);
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
    // 最終的に描画する矩形
    std::vector<std::vector<Quad2D>> quadLines;


    //====================== テキストを行ごとに分割して描画 =========================//
    for(auto& codePoints : codePointsList){

        // 新しい行の矩形リストを追加
        auto& quads = quadLines.emplace_back();

        // 一行
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
                if(alignX == TextAlignX::LEFT){// 左寄せ
                    leftX = 0.0f;
                } else if(alignX == TextAlignX::CENTER){// 中央寄せ
                    leftX = (size.x - totalWidth) * 0.5f;
                } else if(alignX == TextAlignX::RIGHT){// 右寄せ
                    leftX = size.x - totalWidth;
                }
                float curX = leftX;


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
                    }


                    // テクスチャ座標を設定
                    quad.texCoord[0] = lineGlyph->texcoordLT;
                    quad.texCoord[1] = { lineGlyph->texcoordRB.x, lineGlyph->texcoordLT.y };
                    quad.texCoord[2] = { lineGlyph->texcoordLT.x, lineGlyph->texcoordRB.y };
                    quad.texCoord[3] = lineGlyph->texcoordRB;
                    // グラフハンドルを設定
                    quad.GH = lineGlyph->graphHandle;
                    // 色を設定
                    quad.color = _color;
                    // その他設定
                    quad.isText = true;

                    // 描画設定
                    quad.isApplyViewMat = isApplyViewMat;
                    quad.layer = layer;
                    quad.blendMode = blendMode;
                    quad.drawLocation = drawLocation;

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

    // Y軸のalignに応じて全体のオフセットを計算
    float top = FLT_MAX;
    float bottom = -FLT_MAX;
    float yOffset = 0.0f;

    // 最大の高さを計算
    auto& topLine = quadLines.front();
    auto& bottomLine = quadLines.back();
    if(!topLine.empty()){
        for(auto& quad : topLine){
            if(quad.localVertex[0].y < top){ top = quad.localVertex[0].y; }
        }
        for(auto& quad : bottomLine){
            if(quad.localVertex[2].y > bottom){ bottom = quad.localVertex[2].y; }
        }

        // 最大落差
        float totalHeight = bottom - top;

        // alignYに応じてオフセットを計算
        float baselineOffset = fontSize * (1.0f - fontData.baselneHeightRate);
        if(alignY == TextAlignY::TOP){// 上寄せ
            yOffset = 0.0f;
        } else if(alignY == TextAlignY::CENTER){// 中央寄せ
            yOffset = size.y * 0.5f - (totalHeight * 0.5f) - baselineOffset;
        } else if(alignY == TextAlignY::BOTTOM){// 下寄せ
            yOffset = size.y - totalHeight - baselineOffset;
        }

        // オフセットや行列を適用
        for(auto& quads : quadLines){
            for(auto& quad : quads){
                for(int i = 0; i < 4; i++){
                    // Yオフセットを適用
                    quad.localVertex[i].y += yOffset;

                    // 行列を適用
                    quad.localVertex[i] *= textBoxMat;
                }

                // アウトラインの描画
                if(useOutline){
                    Matrix3x3 rotateMat = RotateMatrix(transform.rotate);
                    Vector2 rotatedVec = Vector2(0.0f, 1.0f) * rotateMat;
                    float radianEvery = (3.14f * 2.0f) / outlineSplitCount;
                    Quad2D outlineQuad = quad;
                    if(masterColor != std::nullopt){
                        outlineQuad.color = outlineColor * masterColor.value();
                    } else{
                        outlineQuad.color = outlineColor;
                    }
                    outlineQuad.color.w *= _color.w; // 透明度は元の文字の透明度を乗算

                    // アウトラインを移動させて描画
                    for(int j = 0; j < outlineSplitCount; j++){
                        // アウトラインの位置を計算
                        Vector2 offset = rotatedVec * outlineWidth;
                        outlineQuad.localVertex[0] = quad.localVertex[0] + offset;
                        outlineQuad.localVertex[1] = quad.localVertex[1] + offset;
                        outlineQuad.localVertex[2] = quad.localVertex[2] + offset;
                        outlineQuad.localVertex[3] = quad.localVertex[3] + offset;
                        // アウトラインの色を設定
                        outlineQuad.layer = layer - 1;
                        outlineQuad.isApplyViewMat = isApplyViewMat;
                        outlineQuad.blendMode = blendMode;
                        outlineQuad.drawLocation = drawLocation;
                        SEED::DrawQuad2D(outlineQuad);
                        // 回転ベクトルを更新
                        rotatedVec *= RotateMatrix(radianEvery);
                    }
                }

                // 文字本体の描画
                SEED::DrawQuad2D(quad);
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
    if(!fileName.empty()){
        TextSystem::GetInstance()->LoadFont(fileName);
        this->fontName = fileName;
    } else{
        std::string defaulFont = "DefaultAssets/M_PLUS_Rounded_1c/MPLUSRounded1c-Bold.ttf";
        TextSystem::GetInstance()->LoadFont(defaulFont);
        this->fontName = defaulFont;
    }
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
