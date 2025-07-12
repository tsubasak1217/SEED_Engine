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

                // guizmo
                ImGuiManager::RegisterGuizmoItem(&segment.start);
                ImGuiManager::RegisterGuizmoItem(&segment.end);
                SEED::DrawLine2D(segment.start.translate, segment.end.translate);

                // text
                textBox.text = resultItem.first.c_str();
                textBox.transform.translate = segment.start.translate + Vector2(10.0f, 10.0f);
                textBox.Draw();

                // sprite
                auto& sprite = resultItem.second.backSprite;
                sprite.translate = segment.end.translate;
                sprite.scale = segment.end.scale;

                if(ImGui::CollapsingHeader(resultItem.first.c_str())){
                    ImGui::Indent();

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
                        std::string selectedFile = ImFunc::FolderView("画像選択", basePath);

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
