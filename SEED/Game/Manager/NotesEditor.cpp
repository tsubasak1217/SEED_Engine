#include "NotesEditor.h"
// ノーツのインクルード
#include <Game/Objects/Notes/Note_Tap.h>
#include <Game/Objects/Notes/Note_Hold.h>
#include <Game/Objects/Notes/Note_RectFlick.h>
#include <Game/Objects/Notes/Note_Wheel.h>


NotesEditor::NotesEditor(){
    laneLTPos_ = { 30,30 };
    laneSize_ = { 300,600 };
    textureIDs_["laneField"] = TextureManager::GetImGuiTexture("PlayField/editorLane.png");
    textureIDs_["playIcon"] = TextureManager::GetImGuiTexture("../../SEED/EngineResources/Textures/play2.png");
    textureIDs_["pauseIcon"] = TextureManager::GetImGuiTexture("../../SEED/EngineResources/Textures/play.png");
}

////////////////////////////////////////////////////////////////////////
// 編集処理
////////////////////////////////////////////////////////////////////////
void NotesEditor::Edit(){
#ifdef _DEBUG
    ImFunc::CustomBegin("NotesEditor", MoveOnly_TitleBar);

    // ドローリストの取得
    pDrawList_ = ImGui::GetWindowDrawList();

    // レーンの表示
    DisplayLane();
    // テンポ情報をレーン上に配置して表示
    DisplayTempoData();
    // テンポデータの編集
    ImGui::SetCursorScreenPos(tempoDataDisplayPos_);
    ImGui::BeginChild("テンポ情報一覧", ImVec2(0, 300), true);
    EditTempoData();
    ImGui::EndChild();
    // 中心線の表示
    DisplayLine();
    // ノーツの編集
    ImGui::SetCursorScreenPos(tempoDataDisplayPos_ + ImVec2(0, 320));
    ImGui::BeginChild("ノーツ編集", ImVec2(0, 300), true);
    EditNotes();
    ImGui::EndChild();


    // durationの更新
    if(!tempoDataList_.empty()){
        duration_ = tempoDataList_.back().time + tempoDataList_.back().CalcDuration();
    } else{
        duration_ = 0.0f; // テンポデータがない場合は0に設定
    }

    // 時間の更新
    if(isPlaying_){
        curLaneTime_ += ClockManager::DeltaTime();
        curLaneTime_ = std::clamp(curLaneTime_, 0.0f, duration_); // 範囲外チェック
    }

    ImGui::End();
#endif // _DEBUG
}

////////////////////////////////////////////////////////////////////////
// レーンの表示
////////////////////////////////////////////////////////////////////////
void NotesEditor::DisplayLane(){

    laneBorders_.clear();
    ImVec2 windowPos = ImGui::GetWindowPos();
    worldLaneLTPos_ = windowPos + laneLTPos_;
    tempoDataDisplayPos_ = worldLaneLTPos_ + ImVec2(laneSize_.x + 30.0f, 0.0f);
    ImGui::SetCursorScreenPos(windowPos + laneLTPos_);
    ImGui::Image(textureIDs_["laneField"], laneSize_);

    for(int i = 0; i < 5; i++){
        // レーンの境界線を計算
        float x = worldLaneLTPos_.x + (laneSize_.x / 5.0f) * i;
        laneBorders_.push_back(x); // 境界線の位置を保存
    }


    // 再生/停止ボタンの表示
    ImTextureID playIcon = textureIDs_["pauseIcon"];
    if(isPlaying_){
        playIcon = textureIDs_["playIcon"];
    }

    if(ImGui::ImageButton("playIcon", playIcon, ImVec2(20, 20), ImVec2(0, 0), ImVec2(1, 1))){
        isPlaying_ = !isPlaying_; // 再生/停止の切り替え
    }

    // タイムのスライダー表示
    ImGui::SameLine();
    ImGui::SetNextItemWidth(laneSize_.x);
    ImGui::SliderFloat("時間", &curLaneTime_, 0.0f, duration_, "%.2f");
}

////////////////////////////////////////////////////////////////////////
// 中心線の表示
////////////////////////////////////////////////////////////////////////
void NotesEditor::DisplayLine(){
    ImVec2 windowPos = ImGui::GetWindowPos();

    // 中心線の描画
    ImVec2 p1 = worldLaneLTPos_ + ImVec2(0, laneSize_.y * 0.5f);
    ImVec2 p2 = p1 + ImVec2(laneSize_.x, 0);
    pDrawList_->AddLine(p1, p2, IM_COL32(255, 0, 0, 255), 3.0f);
}

////////////////////////////////////////////////////////////////////////
// テンポデータの表示(BPMのライン表示)
////////////////////////////////////////////////////////////////////////
void NotesEditor::DisplayTempoData(){
    upLayerBorders_.clear();
    downLayerBorders_.clear();
    ImVec2 windowPos = ImGui::GetWindowPos();
    ImVec2 p = worldLaneLTPos_ + ImVec2(0, laneSize_.y * 0.5f);

    // 配置される時間の計算
    float curTime = 0.0f;
    float visibleTime = kVisibleTime_ * timeScale_;
    float endTime = curLaneTime_ + visibleTime;
    int idx = 0;

    for(auto& tempoData : tempoDataList_){
        tempoData.time = curTime;
        curTime += tempoData.CalcDuration();
        float dataEndTime = tempoData.time + tempoData.CalcDuration();
        bool isDraw = false; // 描画フラグ

        // 時間が表示範囲内にない場合はスキップ
        if(MyFunc::IsContain({ curLaneTime_,endTime }, tempoData.time) or
            MyFunc::IsContain({ curLaneTime_,endTime }, dataEndTime) or
            (curLaneTime_ > tempoData.time && endTime <= dataEndTime)
            ){
            isDraw = true; // 描画フラグを立てる
        }

        // 時間が表示範囲外ならスキップ
        if(!isDraw){
            idx++;
            continue;
        }

        // 時間を表示する位置を計算
        float t = (tempoData.time - curLaneTime_) / visibleTime;
        float height = (laneSize_.y * 0.5f) * t;
        float beatTime = 60.0f / tempoData.bpm;
        float ratio = 4.0f / tempoData.timeSignature_denominator; // 4分音符を基準にする
        float beatHeight = (laneSize_.y * 0.5f) * (beatTime / visibleTime) * ratio;
        int loopCount = tempoData.barCount * tempoData.timeSignature_numerator;

        for(int i = 0; i < loopCount; i++){

            // 色と太さの設定
            ImColor color;
            float fatness;
            if(i % tempoData.timeSignature_numerator == 0){
                color = IM_COL32(255, 255, 0, 255);// 黄色
                fatness = 3.0f;
            } else{
                color = IM_COL32(255, 255, 255, 255);// 白色
                fatness = 1.0f;
            }

            // 描画位置を計算
            static ImVec2 points[2];
            points[0] = p + ImVec2(0.0f, height + beatHeight * i);
            points[1] = p + ImVec2(laneSize_.x, height + beatHeight * i);
            if(points[0].y < p.y){ continue; }// 範囲外チェック
            if(points[0].y > worldLaneLTPos_.y + laneSize_.y){ break; }// 範囲外チェック

            // 描画(下半分)
            pDrawList_->AddLine(points[0], points[1], color, fatness);

            // 格納データの情報を設定
            DivisionData data;
            data.beatTime = beatTime;
            data.timeOnLane = tempoData.time + beatTime * i;
            data.laneYPosition = points[0].y;
            data.parent = &tempoData;
            data.beatYHeight = beatHeight;

            downLayerBorders_.push_back(data); // 下半分の境界を保存

            // 描画(上半分)
            points[0] = p + ImVec2(0.0f, -(height + beatHeight * i));
            points[1] = p + ImVec2(laneSize_.x, -(height + beatHeight * i));
            pDrawList_->AddLine(points[0], points[1], color, fatness);
            data.laneYPosition = points[0].y; // 上半分の位置はマイナス
            upLayerBorders_.push_back(data); // 上半分の境界を保存

        }

        if(MyFunc::IsContain({ curLaneTime_,endTime }, tempoData.time)){
            // ボタンの表示
            static ImVec2 buttonPos;
            bool isButtonPressed[2];
            bool isColorPushed = false;

            if(selectedTempoData_ == &tempoData){
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.5f, 0.2f, 1.0f)); // 選択中のボタンの色
                isColorPushed = true; // 色を変更したフラグを立てる
            }

            buttonPos = p + ImVec2(laneSize_.x + 10.0f, height - 10.0f);
            ImGui::SetCursorScreenPos(buttonPos);
            std::string label = std::to_string(idx);
            isButtonPressed[0] = ImGui::Button(label.c_str());

            buttonPos = p + ImVec2(laneSize_.x + 10.0f, -height - 10.0f);
            ImGui::SetCursorScreenPos(buttonPos);
            label += "##1";
            isButtonPressed[1] = ImGui::Button(label.c_str());

            // ボタンの押下処理
            if(isButtonPressed[0] || isButtonPressed[1]){
                selectedTempoData_ = &tempoData;// 選択されたテンポデータを保存
            }

            if(isColorPushed){
                ImGui::PopStyleColor(); // 色を元に戻す
            }
        }

        idx++;
    }
}

////////////////////////////////////////////////////////////////////////
// テンポデータ全体の編集処理
////////////////////////////////////////////////////////////////////////
void NotesEditor::EditTempoData(){

    if(ImGui::Button("テンポ情報の追加")){
        // 新しいテンポデータの追加
        tempoDataList_.emplace_back(); // デフォルトコンストラクタで追加
        selectedTempoData_ = &tempoDataList_.back(); // 追加したばかりのテンポデータを選択
    }

    std::string label;
    int idx = 0;

    // 一覧の表示
    ImGui::Separator();
    for(auto it = tempoDataList_.begin(); it != tempoDataList_.end(); ++it){
        label = "テンポ情報_" + std::to_string(idx);
        bool isColorPushed = false;

        if(selectedTempoData_ == &(*it)){
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.5f, 0.2f, 1.0f)); // 選択中のボタンの色
            isColorPushed = true; // 色を変更したフラグを立てる
        }

        if(ImGui::Button(label.c_str())){
            selectedTempoData_ = &(*it); // ボタンが押されたら選択
        }

        ImGui::SameLine(); // ボタンの横にテキストを表示
        ImGui::Text("(%d分%.3f秒~)", int(it->time / 60.0f), std::fmod(it->time, 60.0f));

        if(isColorPushed){
            ImGui::PopStyleColor(); // 色を元に戻す
        }

        // 右クリックでコンテキストメニューを表示
        if(ImGui::BeginPopupContextItem(label.c_str())){
            contextMenuTempoData_ = &(*it); // コンテキストメニューの対象を保存
            if(ImGui::MenuItem("削除")){

                // ポインタのクリア
                if(selectedTempoData_ == &(*it)){
                    selectedTempoData_ = nullptr; // 選択中のテンポデータをクリア
                }
                contextMenuTempoData_ = nullptr; // コンテキストメニューの対象をクリア

                // 削除処理
                tempoDataList_.erase(it); // リストから削除

                ImGui::EndPopup();
                break;// ループを抜ける
            }
            ImGui::EndPopup();
        }

        idx++;
    }

    // 選択中のテンポデータの編集
    if(selectedTempoData_){
        ImGui::Separator();
        ImGui::Text("選択中のテンポデータの編集");
        selectedTempoData_->Edit(); // 選択されたテンポデータの編集
    }
}

////////////////////////////////////////////////////////////////////////
// テンポデータ単体の編集処理
////////////////////////////////////////////////////////////////////////
void TempoData::Edit(){
#ifdef _DEBUG
    ImGui::DragFloat("BPM", &bpm, 0.1f, 10.0f, 300.0f);
    ImGui::DragInt("拍子の分子", &timeSignature_numerator, 0.1f, 1, 64);
    ImGui::DragInt("拍子の分母", &timeSignature_denominator, 0.1f, 1, 64);
    ImGui::DragInt("小節数", &barCount, 0.1f, 1);
#endif // _DEBUG
}

////////////////////////////////////////////////////////////////////////
// ノーツの編集処理
////////////////////////////////////////////////////////////////////////
void NotesEditor::EditNotes(){

    // 編集ノーツ種の選択
    ImFunc::Combo("編集ノーツタイプ", editNoteType_, { "タップ","ホールド","レクトフリック","ホイール" });

    // 小節の分割モードの選択
    if(ImFunc::Combo("分数", divisionMode_, { "1/4","1/6","1/8","1/12","1/16","1/24","1/32","1/48","1/64" })){
        switch(divisionMode_){
        case Div_4: division_ = 4.0f; break; // 1/4
        case Div_6: division_ = 6.0f; break; // 1/6
        case Div_8: division_ = 8.0f; break; // 1/8
        case Div_12: division_ = 12.0f; break; // 1/12
        case Div_16: division_ = 16.0f; break; // 1/16
        case Div_24: division_ = 24.0f; break; // 1/24
        case Div_32: division_ = 32.0f; break; // 1/32
        case Div_48: division_ = 48.0f; break; // 1/48
        case Div_64: division_ = 64.0f; break; // 1/64
        }
    }


    // レーン上にノーツを作成する処理
    CreateNoteOnLane();
}

////////////////////////////////////////////////////////////////////////
// レーン上に直接ノーツを作成する処理
////////////////////////////////////////////////////////////////////////
void NotesEditor::CreateNoteOnLane(){

    // マウス座標がレーンの範囲外なら何もしない
    ImVec2 mousePos = ImGui::GetMousePos();
    ImVec2 windowPos = ImGui::GetWindowPos();
    ImVec2 worldLaneRB = worldLaneLTPos_ + laneSize_;

    // レーンの範囲外なら終了
    if(mousePos.x < worldLaneLTPos_.x || mousePos.x > worldLaneRB.x ||
        mousePos.y < worldLaneLTPos_.y || mousePos.y > worldLaneRB.y){
        return;
    }

    // マウスを押していなければ何もしない
    if(!ImGui::IsMouseDown(0) && !ImGui::IsMouseReleased(0)){
        return;
    }

    // 上下どちらにマウスがあるかを判定
    UpDown layer;
    if(mousePos.y < worldLaneLTPos_.y + laneSize_.y * 0.5f){
        layer = UpDown::UP; // 上半分
    } else{
        layer = UpDown::DOWN; // 下半分
    }

    // どこのレーンに配置するかを判定
    int laneIndex = -1;
    for(int i = 0; i < laneBorders_.size(); i++){
        if(mousePos.x >= laneBorders_[i]){
            laneIndex = i; // レーンのインデックスを取得
        }
    }

    // レーン上のどこに配置するかを計算
    float timeLocation = 0.0f;
    float divisionRate = division_ / 4.0f;
    float navLineDrawPosY = 0.0f;

    if(layer == UpDown::UP){
        for(int i = 0; i < upLayerBorders_.size(); i++){
            int nextIdx = std::clamp(i + 1, 0, int(upLayerBorders_.size()) - 1);

            if(MyFunc::IsContain(
                { upLayerBorders_[nextIdx].laneYPosition,upLayerBorders_[i].laneYPosition },
                mousePos.y)){

                // さらに分割数に応じて位置を調整
                float dividedHeight = upLayerBorders_[i].beatYHeight / divisionRate;
                // マウスのY座標から分割位置を計算
                float offsetY = mousePos.y - upLayerBorders_[i].laneYPosition;
                // 分割位置を計算
                timeLocation = upLayerBorders_[i].timeOnLane 
                    + std::fabsf(float(int(offsetY / dividedHeight))) * (upLayerBorders_[i].beatTime/ divisionRate);
                // ノーツの配置予定座標の計算
                navLineDrawPosY = upLayerBorders_[i].laneYPosition
                    + float(int(offsetY / dividedHeight)) * (upLayerBorders_[i].beatYHeight / divisionRate);

                break;
            }
        }

    } else{
        for(int i = 0; i < downLayerBorders_.size(); i++){
            int nextIdx = std::clamp(i + 1, 0, int(downLayerBorders_.size()) - 1);
            if(MyFunc::IsContain(
                { downLayerBorders_[nextIdx].laneYPosition,downLayerBorders_[i].laneYPosition },
                mousePos.y)){

                // さらに分割数に応じて位置を調整
                float dividedHeight = downLayerBorders_[i].beatYHeight / divisionRate;
                // マウスのY座標から分割位置を計算
                float offsetY = mousePos.y - downLayerBorders_[i].laneYPosition;
                // 分割位置を計算
                timeLocation = downLayerBorders_[i].timeOnLane 
                    + std::fabsf(float(int(offsetY / dividedHeight))) * (upLayerBorders_[i].beatTime / divisionRate);
                // ノーツの配置予定座標の計算
                navLineDrawPosY = downLayerBorders_[i].laneYPosition
                    + float(int(offsetY / dividedHeight)) * (downLayerBorders_[i].beatYHeight / divisionRate);

                break;
            }
        }
    }

    if(ImGui::IsMouseDown(0)){
        // マウスを押している間はノーツの配置予定位置を表示
        ImVec2 p1 = ImVec2(laneBorders_[laneIndex], navLineDrawPosY);
        ImVec2 p2 = p1 + ImVec2(laneBorders_[1] - laneBorders_[0],0.0f); // ノーツの配置予定位置のY座標を少し下げる
        pDrawList_->AddLine(p1, p2, IM_COL32(255, 0, 0, 255), 3.0f); // 緑色のラインを表示
        ImGui::Text("true");
    }
    // マウスを離したら
    else if(ImGui::IsMouseReleased(0)){
        // ノーツの作成
        switch(editNoteType_){
        case Tap:
        {
            // タップノーツの作成
            notes_.emplace_back(std::make_unique<Note_Tap>());
            if(Note_Tap* note = dynamic_cast<Note_Tap*>(notes_.back().get())){
                note->layer_ = layer; // レイヤーを設定
                note->lane_ = laneIndex; // レーンを設定
                note->time_ = timeLocation; // 時間を設定
                note->laneBit_ = (LaneBit)(1 << laneIndex);
            }
            break;
        }
        case Hold:
        {
            // ホールドノーツの作成
            notes_.emplace_back(std::make_unique<Note_Hold>());
            if(Note_Hold* note = dynamic_cast<Note_Hold*>(notes_.back().get())){
                note->layer_ = layer; // レイヤーを設定
                note->lane_ = laneIndex; // レーンを設定
                note->laneBit_ = (LaneBit)(1 << laneIndex);
                note->time_ = timeLocation; // 時間を設定
                note->kHoldTime_ = 0.5f; // 初期ホールド時間
            }

            break;
        }
        case RectFlick:
        {

            break;
        }
        case Wheel:
        {
            // ホイールノーツの作成
            notes_.emplace_back(std::make_unique<Note_Wheel>());
            if(Note_Wheel* note = dynamic_cast<Note_Wheel*>(notes_.back().get())){
                note->layer_ = layer; // レイヤーを設定
                note->direction_ = UpDown::UP;
                note->laneBit_ = LaneBit::WHEEL_UP;
                note->time_ = timeLocation; // 時間を設定
            }
            break;
        }
        default:
            break;
        }
    }
}

////////////////////////////////////////////////////////////////////////
// ノーツの表示処理
////////////////////////////////////////////////////////////////////////
void NotesEditor::DisplayNotes(){

}
