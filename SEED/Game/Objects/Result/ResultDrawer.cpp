#include "ResultDrawer.h"
#include <SEED/Source/SEED.h>

// リザルトを描画する関数
void ResultDrawer::DrawResult(ResultStep step, float progress){

    // 初期化されていない場合は初期化を行う
    if(!initialized){
        Initialize();
    }

    // 描画の進行状況に応じて描画処理を分岐
    switch(step){
    case ResultStep::Appear:// リザルトが表示される段階
    {
        break;
    }
    case ResultStep::CountUp:// 各種カウントを数え上げる
    {
        break;
    }
    case ResultStep::Display:// すべての結果が表示されている状態
    {
        break;
    }
    case ResultStep::Exit:// リザルトを閉じる段階
    {
        break;
    }
    default:
    {
        // 何もしない
        break;
    }
    }

    progress;
}

void ResultDrawer::Initialize(){

    // 新しい項目を追加
    resultItems.emplace("SongName", ResultItem());
    resultItems.emplace("Score", ResultItem());
    resultItems.emplace("Rank", ResultItem());
    resultItems.emplace("Combo", ResultItem());
    resultItems.emplace("Perfect", ResultItem());
    resultItems.emplace("Great", ResultItem());
    resultItems.emplace("Good", ResultItem());
    resultItems.emplace("Miss", ResultItem());
    resultItems.emplace("Fast", ResultItem());
    resultItems.emplace("Late", ResultItem());

    LoadFromJson();
    initialized = true;
}

#ifdef _DEBUG
void ResultDrawer::Edit(){
    // transformをguizmoに登録
    ImFunc::CustomBegin("リザルト描画クラス", MoveOnly_TitleBar);
    {

        ImGui::Checkbox("isEditTransform", &isEditTransform);
        if(ImGui::Button("保存")){
            SaveToJson();
        }

        if(isEditTransform){
            static TextBox2D textBox;
            static bool isTextInitialized = false;

            if(!isTextInitialized){
                textBox.size = { 500.0f,100.0f };
                textBox.align = TextAlign::LEFT;
                textBox.fontSize = 30.0f;
                textBox.anchorPos = { 0.0f,0.0f };
                textBox.textBoxVisible = false;
                textBox.SetFont("DefaultAssets/M_PLUS_Rounded_1c/MPLUSRounded1c-Bold.ttf");
            }

            for(auto& resultItem : resultItems){
                auto& segment = resultItem.second.transformSegment;
                auto& finalTransform = resultItem.second.finalTransform;
                finalTransform = MyFunc::Interpolate(segment.start, segment.end, resultItem.second.t);

                // guizmo
                ImGuiManager::RegisterGuizmoItem(&segment.start);
                ImGuiManager::RegisterGuizmoItem(&segment.end);
                SEED::DrawLine2D(segment.start.translate, segment.end.translate);

                // text
                textBox.text = resultItem.first.c_str();
                textBox.transform.translate = segment.start.translate + Vector2(10.0f, 10.0f);
                textBox.Draw();
                auto& textItems = resultItem.second.textItems;
                for(int i = 0; i < textItems.size(); i++){
                    textItems[i].textBox.transform.translate = finalTransform.translate + textItems[i].offset;
                    textItems[i].textBox.text = DecideText(textItems[i].typeName);
                    textItems[i].textBox.Draw();
                }


                // sprite
                auto& sprite = resultItem.second.backSprite;
                sprite.translate = finalTransform.translate;
                sprite.scale = finalTransform.scale;
                sprite.anchorPoint = { 0.5f, 0.5f };
                if(resultItem.second.isAlphaMove){
                    sprite.color.w = resultItem.second.maxAlpha * resultItem.second.t;
                } else{
                    sprite.color.w = resultItem.second.maxAlpha;
                }
                sprite.Draw();

                if(ImGui::CollapsingHeader(resultItem.first.c_str())){
                    ImGui::Indent();

                    // 媒介変数のスライダーを表示
                    ImGui::SliderFloat("媒介変数", &resultItem.second.t, 0.0f, 1.0f);

                    // 各トランスフォームのGUI編集
                    if(ImGui::CollapsingHeader("Transform")){
                        ImGui::Indent();
                        // Translate
                        ImGui::Text("Translate");
                        ImGui::DragFloat2("始点: Translate", &segment.start.translate.x, 1.0f);
                        ImGui::DragFloat2("終点: Translate", &segment.end.translate.x, 1.0f);

                        // Scale
                        ImGui::Text("Scale");
                        ImGui::DragFloat2("始点: Scale", &segment.start.scale.x, 0.05f);
                        ImGui::DragFloat2("終点: Scale", &segment.end.scale.x, 0.05f);

                        // Rotation
                        ImGui::Text("Rotation");
                        ImGui::DragFloat("始点: Rotation", &segment.start.rotate, 0.1f);
                        ImGui::DragFloat("終点: Rotation", &segment.end.rotate, 0.1f);
                        ImGui::Unindent();
                    }

                    if(ImGui::CollapsingHeader("BackSprite")){
                        ImGui::Indent();

                        // 画像サイズの設定
                        ImGui::Text("Image Size");
                        ImGui::DragFloat2("始点: Image Size", &sprite.size.x, 0.1f);

                        // 画像の色編集
                        ImGui::ColorEdit4("色", &sprite.color.x);

                        // 画像の変更
                        static std::filesystem::path basePath = "Resources/Textures/";
                        std::string selectedFile = ImFunc::FolderView("画像選択", basePath, false, { ".png",".jpg" });
                        if(!selectedFile.empty()){
                            sprite.GH = TextureManager::LoadTexture(selectedFile);
                        }

                        ImGui::Unindent();
                    }

                    if(ImGui::CollapsingHeader("透明度")){
                        ImGui::Indent();
                        // 透明度のスライダーを表示
                        ImGui::SliderFloat("透明度", &resultItem.second.maxAlpha, 0.0f, 1.0f);
                        // 透明度の色編集
                        ImGui::Checkbox("時間に合わせて透明度を変化させる", &resultItem.second.isAlphaMove);
                        ImGui::Unindent();
                    }

                    if(ImGui::CollapsingHeader("テキスト")){
                        ImGui::Indent();

                        if(ImGui::Button("テキストを追加")){
                            textItems.push_back(TextItem());
                        }

                        for(int i = 0; i < textItems.size(); i++){

                            std::string label = "テキスト" + std::to_string(i);
                            if(ImGui::CollapsingHeader(label.c_str())){
                                ImGui::Indent();

                                ImGui::DragFloat2("オフセット", &textItems[i].offset.x, 0.1f);
                                ImFunc::ComboText("テキストタイプ", textItems[i].typeName, ResultTextUtils::textTypes);
                                textItems[i].textBox.Edit();

                                ImGui::Unindent();
                            }
                        }

                        ImGui::Unindent();
                    }


                    ImGui::Unindent();
                }

            }
        }
        ImGui::End();
    }
}


#endif // _DEBUG

// jsonに保存する関数
void ResultDrawer::SaveToJson(){
    static std::string filePath = "Resources/Jsons/Settings/resultDrawerSetting.json";

    nlohmann::ordered_json jsonData;
    for(auto& resultItem : resultItems){
        auto& segment = resultItem.second.transformSegment;
        jsonData["controlPoints"][resultItem.first]["start"] = segment.start;
        jsonData["controlPoints"][resultItem.first]["end"] = segment.end;
    }

    // JSONデータをファイルに保存
    std::ofstream file(filePath);
    if(file.is_open()){
        file << jsonData.dump(4); // 4はインデントのスペース数
        file.close();
    }
}

// jsonから読み込む関数
void ResultDrawer::LoadFromJson(){

    static std::string filePath = "Resources/Jsons/Settings/resultDrawerSetting.json";
    // JSONファイルを読み込む
    std::ifstream file(filePath);

    if(file.is_open()){
        nlohmann::ordered_json jsonData;
        file >> jsonData;
        file.close();
        // 各セグメントの制御点を設定
        for(auto& resultItem : resultItems){
            auto& segment = resultItem.second.transformSegment;
            segment.start = jsonData["controlPoints"][resultItem.first]["start"];
            segment.end = jsonData["controlPoints"][resultItem.first]["end"];
        }
    }
}

// テキストの決定処理
std::string ResultDrawer::DecideText(const std::string& typeName){

    auto& json = result.songData;
    std::string output;

    if(typeName == "None"){
        output = "None";

    } else if(typeName == "Score"){
        //少数第4位以下を切り捨てて文字列に変換
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(4) << result.score;
        output = oss.str();

    } else if(typeName == "Combo"){
        if(json.contains("Combo")){
            output = std::to_string(result.maxCombo);
        }

    } else if(typeName == "Perfect"){
        output = "Perfect";

    } else if(typeName == "PerfectCount"){
        output = std::to_string(result.evalutionCount[0]);

    } else if(typeName == "Great"){
        output = "Great";

    } else if(typeName == "GreatCount"){
        output = std::to_string(result.evalutionCount[1]);

    } else if(typeName == "Good"){
        output = "Good";

    } else if(typeName == "GoodCount"){
        output = std::to_string(result.evalutionCount[2]);

    } else if(typeName == "Miss"){
        output = "Miss";

    } else if(typeName == "MissCount"){
        if(json.contains("Miss")){
            output = std::to_string(result.evalutionCount[3]);
        }

    } else if(typeName == "SongName"){
        if(json.contains("SongName")){
            output = json["SongName"].get<std::string>();
        }

    } else if(typeName == "Difficulty"){
        if(json.contains("difficulty")){
            output = json["difficulty"].get<std::string>();
        }

    } else if(typeName == "Fast"){
        output = "Fast";

    } else if(typeName == "FastCount"){
        output = std::to_string(result.fastCount);

    } else if(typeName == "Late"){
        output = "Late";

    } else if(typeName == "LateCount"){
        output = std::to_string(result.lateCount);
    }

    return output;
}
