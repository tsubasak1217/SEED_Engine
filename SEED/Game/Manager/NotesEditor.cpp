#include "NotesEditor.h"

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
void NotesEditor::EditNotes(){
#ifdef _DEBUG
    ImFunc::CustomBegin("NotesEditor", MoveOnly_TitleBar);

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

    ImVec2 windowPos = ImGui::GetWindowPos();
    ImVec2 worldLaneLT = windowPos + laneLTPos_;
    tempoDataDisplayPos_ = worldLaneLT + ImVec2(laneSize_.x + 30.0f, 0.0f);
    ImGui::SetCursorScreenPos(windowPos + laneLTPos_);
    ImGui::Image(textureIDs_["laneField"], laneSize_);


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
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 windowPos = ImGui::GetWindowPos();
    ImVec2 worldLaneLT = windowPos + laneLTPos_;

    // 中心線の描画
    ImVec2 p1 = worldLaneLT + ImVec2(0, laneSize_.y * 0.5f);
    ImVec2 p2 = p1 + ImVec2(laneSize_.x, 0);
    drawList->AddLine(p1, p2, IM_COL32(255, 0, 0, 255), 3.0f);
}

////////////////////////////////////////////////////////////////////////
// テンポデータの表示
////////////////////////////////////////////////////////////////////////
void NotesEditor::DisplayTempoData(){
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 windowPos = ImGui::GetWindowPos();
    ImVec2 worldLaneLT = windowPos + laneLTPos_;
    ImVec2 p = worldLaneLT + ImVec2(0, laneSize_.y * 0.5f);

    // 配置される時間の計算
    float curTime = 0.0f;
    float visibleTime = kVisibleTime_ * timeScale_;
    float endTime = curLaneTime_ + visibleTime;
    int idx = 0;

    for(auto& tempoData : tempoDataList_){
        tempoData.time = curTime;
        curTime += tempoData.CalcDuration();
        float dataEndTime = tempoData.time + tempoData.CalcDuration();

        // 時間が表示範囲内にない場合はスキップ
        if(!MyFunc::IsContain({ curLaneTime_,endTime }, tempoData.time) && 
            !MyFunc::IsContain({ curLaneTime_,endTime }, dataEndTime)){
            idx++;
            continue;
        }

        // 時間を表示する位置を計算
        float t = (tempoData.time - curLaneTime_) / visibleTime;
        float height = (laneSize_.y * 0.5f) * t;
        float beatTime = 60.0f / tempoData.bpm;
        float ratio = 4.0f/tempoData.timeSignature_denominator; // 4分音符を基準にする
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
            if(points[0].y > worldLaneLT.y + laneSize_.y){ break; }// 範囲外チェック

            // 描画(上半分)
            drawList->AddLine(points[0], points[1], color, fatness);

            // 描画(下半分)
            points[0] = p + ImVec2(0.0f, -(height + beatHeight * i));
            points[1] = p + ImVec2(laneSize_.x, -(height + beatHeight * i));
            drawList->AddLine(points[0], points[1], color, fatness);

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
        ImGui::Text("(%d分%.3f秒~)", int(it->time/60.0f),std::fmod(it->time,60.0f));

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
    ImGui::DragInt("拍子の分子", &timeSignature_numerator, 0.1f,1,64);
    ImGui::DragInt("拍子の分母", &timeSignature_denominator, 0.1f, 1,64);
    ImGui::DragInt("小節数", &barCount, 0.1f, 1);
#endif // _DEBUG
}