#include "ResultDrawer.h"
#include <SEED/Source/SEED.h>

// リザルトを描画する関数
void ResultDrawer::DrawResult(ResultStep step, float progress){

    // 初期化されていない場合は初期化を行う
    if(!initialized){
        Initialize();
    }

    for(auto& resultItem : resultItems){
        auto& segment = resultItem.second.transformSegment;
        auto& finalTransform = resultItem.second.finalTransform;

        float t = 0.0f;
        if(int(step) > resultItem.second.step){
            // ステップが進んでいる場合は、tを1.0fに設定
            t = 1.0f;
        } else if(int(step) == resultItem.second.step){
            // 現在のステップの場合は、進行度に応じてtを設定
            t = progress;
        } else{
            // それ以外の場合は、tを0.0fに設定
            t = 0.0f;
        }

        finalTransform = MyFunc::Interpolate(segment.start, segment.end, t);

        // guizmo
        ImGuiManager::RegisterGuizmoItem(&segment.start);
        ImGuiManager::RegisterGuizmoItem(&segment.end);
        SEED::DrawLine2D(segment.start.translate, segment.end.translate);

        // text
        auto& textItems = resultItem.second.textItems;
        for(int i = 0; i < textItems.size(); i++){
            textItems[i].textBox.transform.translate = finalTransform.translate + textItems[i].offset;
            textItems[i].textBox.textBoxVisible = false; // テキストボックスの表示はしない
            std::string text = DecideText(textItems[i].typeName);
            textItems[i].textBox.text = text == "" ? textItems[i].textBox.text : text;
            textItems[i].textBox.transform.scale = finalTransform.scale;
            if(resultItem.second.isAlphaMove){
                textItems[i].textBox.color.w = t;
            } else{
                textItems[i].textBox.color.w = 1.0f;
            }
            textItems[i].textBox.Draw();
        }

        // sprite
        auto& sprite = resultItem.second.backSprite;
        sprite.translate = finalTransform.translate;
        sprite.scale = finalTransform.scale;
        sprite.anchorPoint = { 0.5f, 0.5f };
        if(resultItem.second.isAlphaMove){
            sprite.color.w = resultItem.second.maxAlpha * t;
        } else{
            sprite.color.w = resultItem.second.maxAlpha;
        }

        sprite.Draw();
    }
}

void ResultDrawer::Initialize(){

    // 新しい項目を追加
    LoadFromJson();

    if(resultItems.empty()){
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
    }

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

        // 媒介変数のスライダーを表示
        ImGui::SliderFloat("媒介変数", &editT, 0.0f, 1.0f);

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
                finalTransform = MyFunc::Interpolate(segment.start, segment.end, editT);

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
                    std::string text = DecideText(textItems[i].typeName);
                    textItems[i].textBox.text = text == "" ? textItems[i].textBox.text : text;
                    textItems[i].textBox.transform.scale = finalTransform.scale;
                    if(resultItem.second.isAlphaMove){
                        textItems[i].textBox.color.w = editT;
                    } else{
                        textItems[i].textBox.color.w = 1.0f;
                    }
                    textItems[i].textBox.Draw();
                }


                // sprite
                auto& sprite = resultItem.second.backSprite;
                sprite.translate = finalTransform.translate;
                sprite.scale = finalTransform.scale;
                sprite.anchorPoint = { 0.5f, 0.5f };
                if(resultItem.second.isAlphaMove){
                    sprite.color.w = resultItem.second.maxAlpha * editT;
                } else{
                    sprite.color.w = resultItem.second.maxAlpha;
                }
                sprite.Draw();

                static bool isOpenHeader;
                isOpenHeader = ImGui::CollapsingHeader(resultItem.first.c_str());

                // 送信側
                if(ImGui::BeginDragDropSource()){
                    ResultItem* ptr = &resultItem.second;
                    ImGui::SetDragDropPayload("ResultItem", &ptr, sizeof(ResultItem*)); // ← ポインタのアドレスを送る
                    ImGui::EndDragDropSource();
                }

                // 受信側
                if(ImGui::BeginDragDropTarget()){
                    if(const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ResultItem")){
                        if(payload->DataSize == sizeof(ResultItem*)){
                            auto droppedPtr = *static_cast<ResultItem* const*>(payload->Data); // ← 二重ポインタの逆キャスト
                            resultItem.second = *droppedPtr; // コピー
                        }
                    }
                    ImGui::EndDragDropTarget();
                }

                if(isOpenHeader){
                    ImGui::Indent();
                    ImGui::InputInt("動く段階", &resultItem.second.step);

                    // 各トランスフォームのGUI編集
                    if(ImGui::CollapsingHeader("Transform")){
                        ImGui::Indent();
                        // Translate
                        ImGui::Text("Translate");
                        ImGui::DragFloat2("始点: Translate", &segment.start.translate.x, 1.0f);
                        ImGui::DragFloat2("終点: Translate", &segment.end.translate.x, 1.0f);

                        // Scale
                        ImGui::Text("Scale");
                        ImGui::DragFloat2("始点: Scale", &segment.start.scale.x, 0.1f);
                        ImGui::DragFloat2("終点: Scale", &segment.end.scale.x, 0.1f);

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
                        ImGui::DragFloat2("始点: Image Size", &sprite.size.x, 1.0f);

                        // 画像の色編集
                        ImGui::ColorEdit4("色", &sprite.color.x);

                        // 画像の変更
                        static std::filesystem::path basePath = "Resources/Textures/";
                        std::string selectedFile = ImFunc::FolderView("画像選択", basePath, false, { ".png",".jpg" });
                        if(!selectedFile.empty()){
                            sprite.SetTexture(selectedFile);
                        }

                        ImGui::Unindent();
                    }

                    if(ImGui::CollapsingHeader("透明度")){
                        ImGui::Indent();
                        // 透明度のスライダーを表示
                        ImGui::SliderFloat("透明度スライダー", &resultItem.second.maxAlpha, 0.0f, 1.0f);
                        // 透明度の色編集
                        ImGui::Checkbox("時間に合わせて透明度を変化させる", &resultItem.second.isAlphaMove);
                        ImGui::Unindent();
                    }

                    if(ImGui::CollapsingHeader("テキスト一覧")){
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
        auto& itemJson = jsonData["items"][resultItem.first];
        auto& segment = resultItem.second.transformSegment;
        itemJson["controlPoints"]["start"] = segment.start;
        itemJson["controlPoints"]["end"] = segment.end;
        itemJson["step"] = resultItem.second.step;

        // textItemsの情報をjsonに追加
        for(const auto& textItem : resultItem.second.textItems){
            itemJson["textItems"].emplace_back();
            auto& textJson = itemJson["textItems"].back();
            textJson["offset"] = textItem.offset;
            textJson["typeName"] = textItem.typeName;
            textJson["textBox"].push_back(textItem.textBox.GetJsonData());
        }

        // spriteの情報をjsonに追加
        itemJson["sprite"]["size"] = resultItem.second.backSprite.size;
        itemJson["sprite"]["color"] = resultItem.second.backSprite.color;
        itemJson["sprite"]["texturePath"] = resultItem.second.backSprite.texturePath;

        // 透明度の情報をjsonに追加
        itemJson["alpha"]["maxAlpha"] = resultItem.second.maxAlpha;
        itemJson["alpha"]["isAlphaMove"] = resultItem.second.isAlphaMove;
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

        if(!jsonData.contains("items")){
            return;
        }

        // 各アイテムを追加
        for(auto& resultItem : jsonData["items"].items()){

            auto& itemData = resultItem.value();

            // アイテムを追加
            resultItems.emplace(resultItem.key(), ResultItem());
            auto& item = resultItems[resultItem.key()];

            // セグメントの制御点を設定
            auto& segment = item.transformSegment;
            segment.start = itemData["controlPoints"]["start"];
            segment.end = itemData["controlPoints"]["end"];

            // ステップを設定
            if(itemData.contains("step")){
                item.step = itemData["step"];
            } else{
                item.step = 0; // デフォルト値
            }

            // テキストアイテムの情報を読み込む
            if(itemData.contains("textItems")){
                for(const auto& textJson : itemData["textItems"]){
                    TextItem textItem;
                    textItem.offset = textJson["offset"];
                    textItem.typeName = textJson["typeName"];
                    textItem.textBox.LoadFromJson(textJson["textBox"].back());
                    item.textItems.push_back(textItem);
                }
            }

            // スプライトの情報を読み込む
            if(itemData.contains("sprite")){
                auto& spriteJson = itemData["sprite"];
                item.backSprite.size = spriteJson["size"];
                item.backSprite.color = spriteJson["color"];
                if(spriteJson["texturePath"] != ""){
                    item.backSprite.SetTexture(spriteJson["texturePath"]);
                }
            }

            // 透明度の情報を読み込む
            if(itemData.contains("alpha")){
                auto& alphaJson = itemData["alpha"];
                item.maxAlpha = alphaJson["maxAlpha"];
                item.isAlphaMove = alphaJson["isAlphaMove"];
            }
        }
    }
}

// テキストの決定処理
std::string ResultDrawer::DecideText(const std::string& typeName){

    auto& json = result.songData;
    std::string output = "";

    if(typeName == "Score"){
        //少数第4位以下を切り捨てて文字列に変換
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(4) << result.score;
        output = oss.str();

    } else if(typeName == "Combo"){
        output = std::to_string(result.maxCombo) + "/" + std::to_string(result.totalCombo);

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
        output = std::to_string(result.evalutionCount[3]);

    } else if(typeName == "SongName"){
        if(json.contains("songName")){
            output = json["songName"].get<std::string>();
        } else{
            output = "Unnamed";
        }

    } else if(typeName == "Difficulty"){
        if(json.contains("difficulty")){
            output = std::to_string(json["difficulty"].get<int>());
        } else{
            output = "0";
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
