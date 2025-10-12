#include "CurveEditor.h"
#include <SEED/Lib/Functions/MyFunc/MyFunc.h>

/////////////////////////////////////////////////////////////////////////////////////////////////
// 初期化
/////////////////////////////////////////////////////////////////////////////////////////////////
void CurveEditor::Initialize(){
    // 初期ポイントの設定
    for(auto& curve : curves_){
        curve.points.clear();
        curve.points.push_back(ImVec2(0.0f, 0.0f));
        curve.points.push_back(ImVec2(1.0f, 1.0f));
    }

    // カーブの色設定
    curves_[0].curveColor_ = ImColor(255, 0, 0, 255);
    curves_[1].curveColor_ = ImColor(0, 255, 0, 255);
    curves_[2].curveColor_ = ImColor(0, 0, 255, 255);
    curves_[3].curveColor_ = ImColor(255, 0, 255, 255);

    // カーブ設定の初期化
    curveChannel_ = CurveChannel::FLOAT;
    curveType_ = Easing::Type::InOutSine;

    // tagの初期化
    tag_ = "##" + MyFunc::PtrToStr(this);

    // 初期化済みにする
    isInitialized_ = true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////
// GUI編集関数
/////////////////////////////////////////////////////////////////////////////////////////////////
void CurveEditor::EditGUI(){

    // 一度のみ初期化
    if(!isInitialized_){
        Initialize();
    }

    // 編集ウィンドウの開始
    ImFunc::CustomBegin("Curve Editor" + tag_, MoveOnly_TitleBar);
    {
        // カーブの編集
        EditCurves();
    }
    ImGui::End();
}




/////////////////////////////////////////////////////////////////////////////////////////////////
// カーブ編集処理
/////////////////////////////////////////////////////////////////////////////////////////////////
void CurveEditor::EditCurves(){

    // drawListの取得
    auto* drawList = ImGui::GetWindowDrawList();

    // チャンネル数の取得
    int channelCount = static_cast<int>(curveChannel_) + 1;

    // カーブ編集エリアの描画
    DrawBG();

    // 描画座標の計算
    for(auto& curve : curves_){
        curve.drawPoints_ = curve.points;
        for(auto& p : curve.drawPoints_){
            p.x = MyMath::Lerp(rect_.Min.x, rect_.Max.x, p.x);
            p.y = MyMath::Lerp(rect_.Min.y, rect_.Max.y, 1.0f - p.y);
        }
    }

    // カーブの描画
    for(size_t i = 0; i < channelCount; ++i){
        ImColor curveColor = curves_[i].curveColor_;
        if(i != curveIdx_){
            curveColor.Value.w *= 0.3f;
        }
        DrawCurve(curves_[i], curveColor);
    }

    // ポイントの描画
    ImColor pointColor = ImColor(255, 255, 0, 255);
    for(size_t i = 0; i < curves_[curveIdx_].points.size(); ++i){
        if(static_cast<int>(i) == selectedPointIndex_){
            pointColor.Value.w = 1.0f;
        } else{
            pointColor.Value.w = 0.3f;
        }
        drawList->AddCircleFilled(curves_[curveIdx_].drawPoints_[i], pointRadius, pointColor);
    }

    // 操作の決定
    DecideOperation();

    // ポイントのドラッグ処理
    DragPoint();

    // コンテキストメニュー内の処理
    if(ImGui::BeginPopup("PointContextMenu" + tag_)){
        ContextMenuSelect();
        ImGui::EndPopup();
    }

    // 一番下にカーソルを移動
    ImGui::SetCursorScreenPos(bottomCursorPos_);

    // カーブの種類やタイプ選択
    static const char* channelStrs[] = { "x","y","z","w" };
    ImFunc::RadioButton("編集中のチャンネル", curveIdx_, channelStrs, channelCount);
    ImFunc::Combo("カーブの種類" + tag_, curveType_, Easing::names, IM_ARRAYSIZE(Easing::names));
    if(ImFunc::Combo("チャンネル数" + tag_, curveChannel_, { "Float","Vector2","Vector3","Vector4" })){
        curveIdx_ = 0;
    }

    // フォルダ表示,読み込み
    static std::filesystem::path folderPath = "Resources/Jsons/Curves";
    std::string selectedFile = ImFunc::FolderView("フォルダ表示" + tag_, folderPath, false, { "curve" }, "Resources/Jsons/Curves");
    if(!selectedFile.empty()){
        FromJson(MyFunc::GetJson(selectedFile));
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// 背景類の描画
/////////////////////////////////////////////////////////////////////////////////////////////////
void CurveEditor::DrawBG(){

    // cursorPosから右下までの範囲をrectにする
    ImVec2 cursorPos = ImGui::GetCursorScreenPos();
    ImVec2 availSize = ImGui::GetContentRegionAvail();
    ImVec2 space = { 10.0f,140.0f };
    ImVec2 maxPos = ImVec2(cursorPos.x + availSize.x - space.x, cursorPos.y + availSize.y - space.y);

    // 最低のサイズ以下の場合は最小サイズにする
    ImVec2 minSize = ImVec2(220.0f, 220.0f);
    ImVec2 maxSize = ImVec2(600.0f, 600.0f);
    if(availSize.x < minSize.x){ maxPos.x = cursorPos.x + minSize.x - space.x; }
    if(availSize.y < minSize.y){ maxPos.y = cursorPos.y + minSize.y - space.y; }
    if(availSize.x > maxSize.x){ maxPos.x = cursorPos.x + maxSize.x - space.x; }
    if(availSize.y > maxSize.y){ maxPos.y = cursorPos.y + maxSize.y - space.y; }

    // rectにscaleを適用
    rect_ = ImRect(cursorPos, maxPos);
    ImVec2 center = rect_.GetCenter();
    rect_.Min -= center;
    rect_.Max -= center;
    rect_.Min *= editorScale_;
    rect_.Max *= editorScale_;
    rect_.Min += center;
    rect_.Max += center;


    // 背景
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImColor bgColor = ImColor(10, 10, 10, 255);
    drawList->AddRectFilled(rect_.Min, rect_.Max, bgColor);

    // jsonがdropされたら読み込む
    if(auto droppedPath = ImFunc::GetDroppedData<std::string>("FILE_PATH", rect_)){
        FromJson(MyFunc::GetJson(*droppedPath));
    }

    // 補助線の描画
    int lineCount = 10;
    ImVec2 size_ = rect_.GetSize();
    for(int i = 1; i < lineCount; ++i){
        float t = static_cast<float>(i) / static_cast<float>(lineCount);
        // 横線
        ImVec2 hp1 = ImVec2(rect_.Min.x, MyMath::Lerp(rect_.Min.y, rect_.Max.y, t));
        ImVec2 hp2 = ImVec2(rect_.Max.x, MyMath::Lerp(rect_.Min.y, rect_.Max.y, t));
        // 縦線
        ImVec2 vp1 = ImVec2(MyMath::Lerp(rect_.Min.x, rect_.Max.x, t), rect_.Min.y);
        ImVec2 vp2 = ImVec2(MyMath::Lerp(rect_.Min.x, rect_.Max.x, t), rect_.Max.y);

        // 0.5の時だけ少し太くする
        if(i == lineCount / 2){
            ImColor lineColor = ImColor(160, 160, 160, 255);
            drawList->AddLine(hp1, hp2, lineColor, 2.0f);
            drawList->AddLine(vp1, vp2, lineColor, 2.0f);
        } else{
            ImColor lineColor = ImColor(40, 40, 40, 255);
            drawList->AddLine(hp1, hp2, lineColor, 1.0f);
            drawList->AddLine(vp1, vp2, lineColor, 1.0f);
        }

        // 線の描画
    }

    // bottomPosの保存
    bottomCursorPos_ = ImVec2(cursorPos.x, maxPos.y);
}


/////////////////////////////////////////////////////////////////////////////////////////////////
// 操作の決定
/////////////////////////////////////////////////////////////////////////////////////////////////
void CurveEditor::DecideOperation(){

    // どれかしらのポイントにホバーしているか格納する変数
    bool hoverAnyPoint = false;
    ImVec2 mousePos = ImGui::GetIO().MousePos;

    if(ImGui::IsMouseClicked(ImGuiMouseButton_Left)){
        hoverAnyPoint = false;
        mousePos = ImGui::GetIO().MousePos;
    }

    for(size_t i = 0; i < curves_[curveIdx_].points.size(); ++i){
        // 選択領域の作成
        ImVec2 p = curves_[curveIdx_].drawPoints_[i];
        float rectSize = pointRadius * 5.0f;

        // 一意なIDを安全に生成
        std::string id = "point" + std::to_string(i) + tag_;
        ImGui::SetCursorScreenPos(ImVec2(p.x - rectSize * 0.5f, p.y - rectSize * 0.5f));
        ImGui::InvisibleButton(id.c_str(), ImVec2(rectSize, rectSize));

        // ホバーしている場合
        if(ImGui::IsItemHovered()){
            selectedPointIndex_ = static_cast<int>(i);
            hoverAnyPoint = true;

            // クリック状況に応じて操作を決定
            if(ImGui::IsMouseClicked(ImGuiMouseButton_Left)){
                clickedMousePos_ = ImGui::GetIO().MousePos;
                originalPointPos_ = curves_[curveIdx_].points[i];
                operation_ = Operation::Drag;
                break;
            } else if(ImGui::IsMouseClicked(ImGuiMouseButton_Right)){
                ImGui::OpenPopup(("PointContextMenu" + tag_).c_str());
                operation_ = Operation::SelectContextMenu;
                break;
            }
        } else{

            // ポイントの範囲外かつエディタ範囲内の場合
            if(ImGui::IsMouseHoveringRect(rect_.Min, rect_.Max)){

                if(ImGui::IsMouseClicked(ImGuiMouseButton_Right)){
                    // 右クリックされたらコンテキストメニュー表示
                    ImGui::OpenPopup("PointContextMenu" + tag_);
                    operation_ = Operation::SelectContextMenu;
                    break;
                }
            }
        }
    }

    // どのポイントにもホバーしていない場合
    if(!hoverAnyPoint){
        // なにもしていない場合は選択idxを0に戻す
        if(operation_ == Operation::None){
            selectedPointIndex_ = -1;
        }
    }
}


/////////////////////////////////////////////////////////////////////////////////////////////////
// コンテキストメニュー
/////////////////////////////////////////////////////////////////////////////////////////////////
void CurveEditor::ContextMenuSelect(){

    // ポイントを選択している場合のみ削除メニューを表示
    if(selectedPointIndex_ > 0 && selectedPointIndex_ < curves_[curveIdx_].points.size() - 1){
        if(ImGui::MenuItem("削除")){
            curves_[curveIdx_].points.erase(curves_[curveIdx_].points.begin() + selectedPointIndex_);
            selectedPointIndex_ = -1;
            operation_ = Operation::None;
            ImGui::CloseCurrentPopup();
        }
    }

    // ポイント追加メニュー
    if(ImGui::MenuItem("追加")){
        AddPoint();
        operation_ = Operation::None;
        ImGui::CloseCurrentPopup();
    }

    // 初期化
    if(ImGui::MenuItem("リセット")){
        Initialize();
        operation_ = Operation::None;
        ImGui::CloseCurrentPopup();
    }

    // 出力
    if(ImGui::MenuItem("ファイル出力")){
        operation_ = Operation::None;
        outputFilename_ = MyFunc::OpenSaveFileDialog("Resources/Jsons/Curves", ".curve", outputFilename_);
        if(!outputFilename_.empty()){
            MyFunc::CreateJsonFile(outputFilename_, ToJson());
            // outputFilename_をファイル名のみに戻す
            outputFilename_ = std::filesystem::path(outputFilename_).filename().stem().string();
        }
        ImGui::CloseCurrentPopup();
    }
}


/////////////////////////////////////////////////////////////////////////////////////////////////
// ポイントのドラッグ処理
/////////////////////////////////////////////////////////////////////////////////////////////////
void CurveEditor::DragPoint(){
    if(operation_ != Operation::Drag){ return; }
    auto& points = curves_[curveIdx_].points;

    // マウスを離していたらドラッグ終了
    if(!ImGui::IsMouseDown(ImGuiMouseButton_Left)){
        selectedPointIndex_ = -1;
        operation_ = Operation::None;

    } else{// ドラッグ中

        // マウスの移動量を計算
        ImVec2 mousePos = ImGui::GetIO().MousePos;
        ImVec2 delta = mousePos - clickedMousePos_;

        // 移動量を正規化してポイントに反映
        ImVec2 size_ = rect_.GetSize();
        points[selectedPointIndex_].x = originalPointPos_.x + delta.x / size_.x;
        points[selectedPointIndex_].y = originalPointPos_.y - delta.y / size_.y;

        // clamp座標の計算
        float clampMax;
        float clampMin;
        if(selectedPointIndex_ != points.size() - 1 && selectedPointIndex_ != 0){
            clampMax = points[selectedPointIndex_ + 1].x;
            clampMin = points[selectedPointIndex_ - 1].x;

        } else{
            // 両端のポイントはx軸固定
            if(selectedPointIndex_ == 0){
                clampMax = 0.0f;
                clampMin = 0.0f;
            } else{
                clampMax = 1.0f;
                clampMin = 1.0f;
            }
        }

        // クランプ
        points[selectedPointIndex_].x = std::clamp(points[selectedPointIndex_].x, clampMin, clampMax);
        points[selectedPointIndex_].y = std::clamp(points[selectedPointIndex_].y, 0.0f, 1.0f);
    }
}


/////////////////////////////////////////////////////////////////////////////////////////////////
// ポイントの追加処理
/////////////////////////////////////////////////////////////////////////////////////////////////
void CurveEditor::AddPoint(){

    // マウス位置を正規化して追加
    ImVec2 size_ = rect_.GetSize();
    ImVec2 mousePos = ImGui::GetIO().MousePos;
    ImVec2 newPoint;
    auto& points = curves_[curveIdx_].points;
    newPoint.x = (mousePos.x - rect_.Min.x) / size_.x;
    newPoint.y = 1.0f - (mousePos.y - rect_.Min.y) / size_.y;
    newPoint.x = std::clamp(newPoint.x, 0.0f, 1.0f);
    newPoint.y = std::clamp(newPoint.y, 0.0f, 1.0f);
    points.push_back(newPoint);

    // 並び替える
    std::sort(points.begin(), points.end(), [](const ImVec2& a, const ImVec2& b){ return a.x < b.x; });
}


/////////////////////////////////////////////////////////////////////////////////////////////////
// カーブの描画処理
/////////////////////////////////////////////////////////////////////////////////////////////////
void CurveEditor::DrawCurve(const EditorCurve& curve, const ImColor& color){

    auto io = ImGui::GetIO();
    auto* draw_list = ImGui::GetWindowDrawList();
    auto& drawPoints = curve.drawPoints_;

    for(size_t p = 0; p < drawPoints.size() - 1; p++){
        const ImVec2 p1 = drawPoints[p];
        const ImVec2 p2 = drawPoints[p + 1];

        // 関数を取得
        auto interpolationFunc = Easing::Ease[curveType_];


        // 分割数分ループ
        int subdivision = 20;
        std::optional<float> prevT = std::nullopt;
        for(int substep = 0; substep < subdivision; substep++){
            float t = float(substep) / float(subdivision - 1);

            // 線の描画
            if(prevT != std::nullopt){
                ImVec2 sp1 = { MyMath::Lerp(p1.x, p2.x, prevT.value()), MyMath::Lerp(p1.y, p2.y, interpolationFunc(prevT.value())) };
                ImVec2 sp2 = { MyMath::Lerp(p1.x, p2.x, t), MyMath::Lerp(p1.y, p2.y, interpolationFunc(t)) };
                draw_list->AddLine(sp1, sp2, color, 1.3f);
            }

            // 値を保存
            prevT = t;
        }
    }
}


/////////////////////////////////////////////////////////////////////////////////////////////////
// スプライン曲線の描画
/////////////////////////////////////////////////////////////////////////////////////////////////
void CurveEditor::DrawSpline(const std::vector<ImVec2>& points, const ImColor& color, int32_t subdivision){

    // 点が2つ未満の場合は描画しない
    if(points.size() < 2){ return; }

    // ImVec2をVector2に変換
    std::vector<Vector2> vector2Points;
    for(const auto& p : points){
        vector2Points.push_back(Vector2(p.x, p.y));
    }

    // 必要な変数を用意
    auto* drawList = ImGui::GetWindowDrawList();
    float t = 0;
    uint32_t totalSubdivision = uint32_t(points.size() - 1) * subdivision;
    std::optional<Vector2> previous = std::nullopt;

    // スプライン曲線の描画
    for(uint32_t i = 0; i <= totalSubdivision; i++){

        // 現在の位置を求める
        t = float(i) / totalSubdivision;

        // 現在の区間の点を求める
        Vector2 p = MyMath::CatmullRomPosition(vector2Points, t);

        // 線を描画
        if(previous != std::nullopt){
            drawList->AddLine(ImVec2(previous->x, previous->y), ImVec2(p.x, p.y), color, 1.3f);
        }

        // 現在の点を保存
        previous = p;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// ベジェ曲線の描画
/////////////////////////////////////////////////////////////////////////////////////////////////
void CurveEditor::DrawBezier(const std::vector<ImVec2>& points, const ImColor& color, int32_t subdivision){
    // 点が2つ未満の場合は描画しない
    if(points.size() < 2){ return; }

    // ImVec2をVector2に変換
    std::vector<Vector2> vector2Points;
    for(const auto& p : points){
        vector2Points.push_back(Vector2(p.x, p.y));
    }

    // 必要な変数を用意
    auto* drawList = ImGui::GetWindowDrawList();
    float t = 0;
    uint32_t totalSubdivision = uint32_t(points.size() - 1) * subdivision;
    std::optional<Vector2> previous = std::nullopt;

    // ベジェ曲線の描画
    for(uint32_t i = 0; i <= totalSubdivision; i++){
        // 現在の位置を求める
        t = float(i) / totalSubdivision;
        // 現在の区間の点を求める
        Vector2 p = MyMath::Bezier(vector2Points, t);
        // 線を描画
        if(previous != std::nullopt){
            drawList->AddLine(ImVec2(previous->x, previous->y), ImVec2(p.x, p.y), color, 1.3f);
        }
        // 現在の点を保存
        previous = p;
    }
}


//////////////////////////////////////////////////////////////////////////////////////////////////
// Jsonに変換
//////////////////////////////////////////////////////////////////////////////////////////////////
nlohmann::json CurveEditor::ToJson() const{
    nlohmann::json j;
    // チャンネル数
    j["channel"] = static_cast<int>(curveChannel_);
    // カーブタイプ
    j["type"] = static_cast<int>(curveType_);
    // カーブのポイントを出力
    int channelCount = static_cast<int>(curveChannel_) + 1;
    for(int i = 0; i < channelCount; ++i){
        nlohmann::json pointArray = nlohmann::json::array();
        for(const auto& p : curves_[i].points){
            nlohmann::json pointJson;
            pointJson["x"] = p.x;
            pointJson["y"] = p.y;
            pointArray.push_back(pointJson);
        }
        j["curves"].push_back(pointArray);
    }
    return j;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Jsonから読み込み
//////////////////////////////////////////////////////////////////////////////////////////////////
void CurveEditor::FromJson(const nlohmann::json& json){
    // jsonが空の場合は何もしない
    if(json.is_null() || json.empty()){ return; }

    // チャンネル数
    if(json.contains("channel")){
        curveChannel_ = static_cast<CurveChannel>(json["channel"].get<int>());
    }
    // カーブタイプ
    if(json.contains("type")){
        curveType_ = static_cast<Easing::Type>(json["type"].get<int>());
    }
    // カーブのポイントを読み込み
    if(json.contains("curves")){
        int channelCount = static_cast<int>(curveChannel_) + 1;
        for(int i = 0; i < channelCount; ++i){
            if(i >= json["curves"].size()){ break; }
            curves_[i].points.clear();
            for(const auto& pointJson : json["curves"][i]){
                if(pointJson.contains("x") && pointJson.contains("y")){
                    ImVec2 p;
                    p.x = pointJson["x"].get<float>();
                    p.y = pointJson["y"].get<float>();
                    curves_[i].points.push_back(p);
                }
            }
        }
    }
}