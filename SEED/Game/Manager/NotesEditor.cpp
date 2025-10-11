#include "NotesEditor.h"
// ノーツのインクルード
#include <Game/Objects/Notes/Note_Tap.h>
#include <Game/Objects/Notes/Note_Hold.h>
#include <Game/Objects/Notes/Note_RectFlick.h>
#include <Game/Objects/Notes/Note_Wheel.h>
#include <Game/Objects/Notes/Note_Warning.h>


NotesEditor::NotesEditor(){

    // オフセット時間
    answerOffsetTime_ = -0.03f;
    metronomeOffsetTime_ = -0.03f;

    laneLTPos_ = { 30,30 };
    laneSize_ = { 300,900 };
    textureIDs_["laneField"] = TextureManager::GetImGuiTexture("PlayField/editorLane.png");
    textureIDs_["playIcon"] = TextureManager::GetImGuiTexture("../../SEED/EngineResources/Textures/play2.png");
    textureIDs_["pauseIcon"] = TextureManager::GetImGuiTexture("../../SEED/EngineResources/Textures/play.png");

    // note
    textureIDs_["tap"] = TextureManager::GetImGuiTexture("Notes/tapNote.png");
    textureIDs_["holdHead"] = TextureManager::GetImGuiTexture("Notes/holdNote_Head.png");
    textureIDs_["holdBody"] = TextureManager::GetImGuiTexture("Notes/holdNote_Body.png");
    textureIDs_["wheel"] = TextureManager::GetImGuiTexture("Notes/wheel_Edit.png");
    textureIDs_["rf_LT"] = TextureManager::GetImGuiTexture("Notes/recFlick_Edit_LT.png");
    textureIDs_["rf_RT"] = TextureManager::GetImGuiTexture("Notes/recFlick_Edit_RT.png");
    textureIDs_["rf_LB"] = TextureManager::GetImGuiTexture("Notes/recFlick_Edit_LB.png");
    textureIDs_["rf_RB"] = TextureManager::GetImGuiTexture("Notes/recFlick_Edit_RB.png");
    textureIDs_["rf_L"] = TextureManager::GetImGuiTexture("Notes/recFlick_Edit_L.png");
    textureIDs_["rf_R"] = TextureManager::GetImGuiTexture("Notes/recFlick_Edit_R.png");
    textureIDs_["rf_U"] = TextureManager::GetImGuiTexture("Notes/recFlick_Edit_U.png");
    textureIDs_["rf_D"] = TextureManager::GetImGuiTexture("Notes/recFlick_Edit_D.png");
    textureIDs_["rf_ALL"] = TextureManager::GetImGuiTexture("Notes/recFlick_Edit_All.png");
    textureIDs_["rf_LT_EX"] = TextureManager::GetImGuiTexture("Notes/recFlick_Edit_LT_EX.png");
    textureIDs_["rf_RT_EX"] = TextureManager::GetImGuiTexture("Notes/recFlick_Edit_RT_EX.png");
    textureIDs_["rf_LB_EX"] = TextureManager::GetImGuiTexture("Notes/recFlick_Edit_LB_EX.png");
    textureIDs_["rf_RB_EX"] = TextureManager::GetImGuiTexture("Notes/recFlick_Edit_RB_EX.png");
    textureIDs_["warning"] = TextureManager::GetImGuiTexture("Notes/warning.png");
    textureIDs_["white"] = TextureManager::GetImGuiTexture("DefaultAssets/white1x1.png");

    // name
    laneTextureNameMap_[LaneBit::RECTFLICK_LT] = "rf_LT";
    laneTextureNameMap_[LaneBit::RECTFLICK_RT] = "rf_RT";
    laneTextureNameMap_[LaneBit::RECTFLICK_LB] = "rf_LB";
    laneTextureNameMap_[LaneBit::RECTFLICK_RB] = "rf_RB";
    laneTextureNameMap_[LaneBit::RECTFLICK_LEFT] = "rf_L";
    laneTextureNameMap_[LaneBit::RECTFLICK_RIGHT] = "rf_R";
    laneTextureNameMap_[LaneBit::RECTFLICK_UP] = "rf_U";
    laneTextureNameMap_[LaneBit::RECTFLICK_DOWN] = "rf_D";
    laneTextureNameMap_[LaneBit::RECTFLICK_LT_EX] = "rf_LT_EX";
    laneTextureNameMap_[LaneBit::RECTFLICK_RT_EX] = "rf_RT_EX";
    laneTextureNameMap_[LaneBit::RECTFLICK_LB_EX] = "rf_LB_EX";
    laneTextureNameMap_[LaneBit::RECTFLICK_RB_EX] = "rf_RB_EX";
    laneTextureNameMap_[LaneBit::RECTFLICK_ALL] = "rf_ALL";

    // アンサー音
    answerSEFileName_ = "SE/answer.mp3";
    metronomeSEFileName_ = "SE/metronome.mp3";
}


////////////////////////////////////////////////////////////////////////
// 初期化処理
////////////////////////////////////////////////////////////////////////
void NotesEditor::Initialize(const std::string& path){
    // ノーツデータを読み込む
    if(std::filesystem::exists(path)){
        std::ifstream file(path);
        nlohmann::json jsonData;
        file >> jsonData;
        LoadFromJson(jsonData); // JSONから譜面データを読み込む
        file.close();
    }
}

////////////////////////////////////////////////////////////////////////
// 編集処理
////////////////////////////////////////////////////////////////////////
void NotesEditor::Edit(){
#ifdef _DEBUG
    // ウィンドウが表示されていない場合は終了
    if(!ImFunc::CustomBegin("NotesEditor", MoveOnly_TitleBar)){
        AudioManager::PauseAudio(audioHandle_); // ウィンドウがフォーカスされていない場合は音声を一時停止
        ImGui::End();
        return;

    } else{
        AudioManager::RestartAudio(audioHandle_); // ウィンドウがフォーカスされている場合は音声を再開
    }



    // ドローリストの取得
    pDrawList_ = ImGui::GetWindowDrawList();

    // タイムスケールの更新処理
    UpdateTimeScale();
    // レーンの表示
    DisplayLane();
    // テンポ情報をレーン上に配置して表示
    DisplayTempoData();
    // テンポデータの編集
    ImGui::SetCursorScreenPos(tempoDataDisplayPos_);
    ImGui::BeginChild("テンポ情報", ImVec2(0, 200), true);
    EditTempoData();
    ImGui::EndChild();
    // 音声データの編集
    ImGui::SetCursorScreenPos(tempoDataDisplayPos_ + ImVec2(0, 220));
    ImGui::BeginChild("楽曲情報", ImVec2(0, 180), true);
    SelectAudioFile();
    ImGui::EndChild();
    // 中心線の表示
    DisplayLine();
    // ノーツの編集
    ImGui::SetCursorScreenPos(tempoDataDisplayPos_ + ImVec2(0, 220) + ImVec2(0, 200));
    ImGui::BeginChild("ノーツ編集", ImVec2(0, 0), true);
    EditNotes();
    ImGui::EndChild();
    // ノーツの表示処理
    DisplayNotes();
    // ドラッグ中のノーツの処理
    DraggingNote();
    // スクロール処理
    ScrollOnLane();
    // 効果音の再生
    PlayMetronome();
    PlayAnswerSE();

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

    // ノーツの削除処理
    EraseCheck();

    ImGui::End();
#endif // _DEBUG
}

////////////////////////////////////////////////////////////////////////
// 音声ファイルの選択
////////////////////////////////////////////////////////////////////////
void NotesEditor::SelectAudioFile(){

    // 音声ファイルを取得
    static std::filesystem::path audioFileBasePath = "Resources/NoteDatas"; // 音声ファイル名の
    static std::string nextPathName{};
    std::string selectedFile = ImFunc::FolderView("音声ファイル選択", audioFileBasePath); // 音声ファイルの選択

    if(!selectedFile.empty()){

        // "NoteDatas"以降の階層にあるかチェック
        if(selectedFile.find("NoteDatas") != std::string::npos){

            // ".wav", ".mp3"の拡張子を持つファイル名かチェック
            if(selectedFile.ends_with(".wav") || selectedFile.ends_with(".mp3")){
                nextPathName = "../../" + selectedFile; // パスを設定
                ImGui::OpenPopup("音声ファイルの変更確認"); // ポップアップを開く
            }
        }
    }

    // ポップアップの表示
    if(ImGui::BeginPopupModal("音声ファイルの変更確認", nullptr, ImGuiWindowFlags_AlwaysAutoResize)){
        ImGui::Text("音声ファイルを変更しますか？\n %s", nextPathName.c_str());
        if(ImGui::Button("はい")){
            audioFilePath_ = nextPathName; // 音声ファイル名を設定
            // 音声の変更
            if(isPlaying_){
                AudioManager::EndAudio(audioHandle_); // 再生中なら停止
                audioHandle_ = AudioManager::PlayAudio(audioFilePath_, false, 1.0f, curLaneTime_); // 音声を再生
            }

            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();
        if(ImGui::Button("いいえ")){
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    ImGui::DragFloat("アンサー音のオフセット", &answerOffsetTime_, 0.01f);
    ImGui::DragFloat("メトロノームのオフセット", &metronomeOffsetTime_, 0.01f);
}

////////////////////////////////////////////////////////////////////////
// タイムスケールの更新処理
////////////////////////////////////////////////////////////////////////
void NotesEditor::UpdateTimeScale(){
    ImVec2 windowPos = ImGui::GetWindowPos();
    worldLaneLTPos_ = windowPos + laneLTPos_;
    ImVec2 mousePos = ImGui::GetMousePos();

    // マウスの位置がレーンの範囲内にあるかチェック
    if(mousePos.x >= worldLaneLTPos_.x && mousePos.x <= worldLaneLTPos_.x + laneSize_.x &&
        mousePos.y >= worldLaneLTPos_.y && mousePos.y <= worldLaneLTPos_.y + laneSize_.y){
        // マウスホイールの回転でタイムスケールを変更
        float wheel = -ImGui::GetIO().MouseWheel;
        if(wheel != 0.0f){
            timeScale_ += wheel * 10.0f * ClockManager::DeltaTime(); // スケールを調整
            timeScale_ = (std::max)(timeScale_, 0.1f); // 最小値を設定
        }
    }
}

////////////////////////////////////////////////////////////////////////
// レーンの表示
////////////////////////////////////////////////////////////////////////
void NotesEditor::DisplayLane(){

    laneBorders_.clear();
    ImVec2 windowPos = ImGui::GetWindowPos();
    tempoDataDisplayPos_ = worldLaneLTPos_ + ImVec2(laneSize_.x + 30.0f, 0.0f);
    ImGui::SetCursorScreenPos(windowPos + laneLTPos_);
    ImGui::Image(textureIDs_["laneField"], laneSize_);

    // ドラッグしたアイテムをドロップで受け付ける
    if(auto droppedPath = ImFunc::GetDroppedData<std::string>("FILE_PATH")){
        // ポップアップが開いていない場合は自動で開く
        ImGui::OpenPopup("ノーツデータの読み込み確認");
        loadFileName_ = *droppedPath; // 読み込むファイル名を保存
    }
    //if(ImGui::BeginDragDropTarget()){
    //    if(const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_PATH")){
    //        const char* path = static_cast<const char*>(payload->Data);
    //        std::string droppedPath(path);

    //        if(!droppedPath.empty()){

    //            // "NoteDatas/"以降の階層にあるかチェック
    //            if(droppedPath.find("NoteDatas") != std::string::npos){
    //                // ポップアップが開いていない場合は自動で開く
    //                ImGui::OpenPopup("ノーツデータの読み込み確認");
    //                loadFileName_ = droppedPath; // 読み込むファイル名を保存
    //            }
    //        }
    //    }
    //    ImGui::EndDragDropTarget();
    //}

    // 読み込むかどうか確認するポップアップを出す
    if(ImGui::BeginPopupModal("ノーツデータの読み込み確認", nullptr, ImGuiWindowFlags_AlwaysAutoResize)){

        std::replace(loadFileName_.begin(), loadFileName_.end(), '/', '\\'); // パスの区切り文字を変換
        ImGui::Text("ノーツデータを読み込みますか？\n%s", loadFileName_.c_str());

        if(ImGui::Button("はい")){
            // ノーツデータを読み込む
            if(std::filesystem::exists(loadFileName_)){
                nlohmann::json jsonData = MyFunc::GetJson(loadFileName_);
                LoadFromJson(jsonData); // JSONから譜面データを読み込む
            }
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();
        if(ImGui::Button("いいえ")){
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }


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
        if(isPlaying_){
            if(!audioFilePath_.empty()){
                audioHandle_ = AudioManager::PlayAudio(audioFilePath_, false, 1.0f, curLaneTime_); // 音声を再生
            }
        } else{
            AudioManager::EndAudio(audioHandle_); // 音声を停止
        }
    }

    // タイムのスライダー表示
    ImGui::SameLine();
    ImGui::SetNextItemWidth(laneSize_.x);
    static bool isSliderActive = false; // スライダーがアクティブかどうかのフラグ
    if(ImGui::SliderFloat("時間", &curLaneTime_, 0.0f, duration_, "%.2f")){
        isSliderActive = true;
        AudioManager::EndAudio(audioHandle_); // 音声を停止
    } else{
        if(isSliderActive){
            isSliderActive = false;
            if(isPlaying_){
                if(!audioFilePath_.empty()){
                    audioHandle_ = AudioManager::PlayAudio(audioFilePath_, false, 1.0f, curLaneTime_); // 音声を再生
                }
            }
        }
    }
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
        float ratio = 4.0f / tempoData.timeSignature_denominator; // 4分音符を基準にする
        float beatTime = (60.0f / tempoData.bpm) * ratio;
        float beatDuration = (laneSize_.y * 0.5f) * (beatTime / visibleTime);
        int loopCount = tempoData.barCount * tempoData.timeSignature_numerator;

        for(int i = 0; i < loopCount; i++){

            // 情報を格納する用の構造体
            DivisionData data;

            // 色と太さの設定
            ImColor color;
            float fatness;
            if(i % tempoData.timeSignature_numerator == 0){
                color = IM_COL32(255, 255, 0, 128);// 黄色
                fatness = 3.0f;
                data.isStartOfSignature = true;
            } else{
                color = IM_COL32(255, 255, 255, 128);// 白色
                fatness = 1.0f;
            }

            // 描画位置を計算
            static ImVec2 points[2];
            points[0] = p + ImVec2(0.0f, height + beatDuration * i);
            points[1] = p + ImVec2(laneSize_.x, height + beatDuration * i);
            if(points[0].y < p.y){ continue; }// 範囲外チェック
            if(points[0].y > worldLaneLTPos_.y + laneSize_.y){ break; }// 範囲外チェック

            // 描画(下半分)
            pDrawList_->AddLine(points[0], points[1], color, fatness);

            // 格納データの情報を設定
            data.beatDuration = beatTime;
            data.timeOnLane = tempoData.time + beatTime * i;
            data.timeOfStartSignature = tempoData.time + beatTime * (i / tempoData.timeSignature_numerator) * tempoData.timeSignature_numerator;
            data.laneYPosition = points[0].y;
            data.heightOfStartSignature = p.y + height + beatDuration * (i / tempoData.timeSignature_numerator) * tempoData.timeSignature_numerator;
            data.parent = &tempoData;
            data.beatYHeight = beatDuration;

            downLayerBorders_.push_back(data); // 下半分の境界を保存

            // 描画(上半分)
            points[0] = p + ImVec2(0.0f, -(height + beatDuration * i));
            points[1] = p + ImVec2(laneSize_.x, -(height + beatDuration * i));
            pDrawList_->AddLine(points[0], points[1], color, fatness);
            data.laneYPosition = points[0].y; // 上半分の位置はマイナス
            data.heightOfStartSignature = p.y - (height + beatDuration * (i / tempoData.timeSignature_numerator) * tempoData.timeSignature_numerator);
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
    if(ImGui::Button("削除")){
        removeFlag = true;
    }
#endif // _DEBUG
}

////////////////////////////////////////////////////////////////////////
// ノーツの編集処理
////////////////////////////////////////////////////////////////////////
void NotesEditor::EditNotes(){

#ifdef _DEBUG

    // 編集ノーツ種の選択
    ImGui::Checkbox("レーン編集", &isEditOnLane_);
    ImFunc::Combo("編集ノーツタイプ", editNoteType_, { "タップ","ホールド","レクトフリック","ホイール","警告"});

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

    // ノーツの編集処理
    if(edittingNote_){
        ImGui::SeparatorText("ノーツ編集");
        edittingNote_->Edit(); // ノーツの編集

        // ノーツの削除ボタン
        ImGui::Separator();
        if(ImGui::Button("削除")){
            edittingNote_->isEnd_ = true;
            if(draggingNote_ == edittingNote_){
                draggingNote_ = nullptr; // ドラッグ中のノーツもクリア
            }
            edittingNote_ = nullptr; // 編集中のノーツをクリア
        }
    }

    ImGui::SeparatorText("ファイル入出力");
    FileControl();

#endif // _DEBUG
}

////////////////////////////////////////////////////////////////////////
// レーン上に直接ノーツを作成する処理
////////////////////////////////////////////////////////////////////////
void NotesEditor::CreateNoteOnLane(){

    if(!isEditOnLane_){
        return; // レーン編集モードでないなら何もしない
    } else{
        if(isHoveringNote_ or draggingNote_){
            return;
        }
    }

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
    float clampedX = std::clamp(mousePos.x, worldLaneLTPos_.x, worldLaneRB.x);
    for(int i = 0; i < laneBorders_.size(); i++){
        if(clampedX >= laneBorders_[i]){
            laneIndex = i; // レーンのインデックスを取得
        }
    }

    // レーン上のどこに配置するかを計算
    float timeLocation = 0.0f;
    float divisionRate = division_ / 4.0f;
    float navLineDrawPosY = 0.0f;
    bool isContained = false;

    if(layer == UpDown::UP){
        for(int i = 0; i < upLayerBorders_.size(); i++){
            int nextIdx = std::clamp(i + 1, 0, int(upLayerBorders_.size()) - 1);

            if(MyFunc::IsContain(
                { upLayerBorders_[nextIdx].laneYPosition,upLayerBorders_[i].laneYPosition },
                mousePos.y)){

                // さらに分割数に応じて位置を調整
                float dividedHeight = upLayerBorders_[i].beatYHeight / divisionRate;
                // マウスのY座標から分割位置を計算
                float offsetY = mousePos.y - upLayerBorders_[i].heightOfStartSignature;
                // 分割位置を計算
                timeLocation = upLayerBorders_[i].timeOfStartSignature
                    + std::fabsf(float(int(offsetY / dividedHeight))) * (upLayerBorders_[i].beatDuration / divisionRate);
                // ノーツの配置予定座標の計算
                navLineDrawPosY = upLayerBorders_[i].heightOfStartSignature
                    + float(int(offsetY / dividedHeight)) * (upLayerBorders_[i].beatYHeight / divisionRate);

                isContained = true; // 範囲内にあるフラグを立てる
                break;
            }
        }

        if(!isContained){
            if(upLayerBorders_.size() != 0){
                if(mousePos.y > upLayerBorders_[0].laneYPosition){
                    timeLocation = upLayerBorders_[0].timeOnLane;
                    navLineDrawPosY = upLayerBorders_[0].laneYPosition;
                } else{
                    timeLocation = upLayerBorders_.back().timeOnLane;
                    navLineDrawPosY = upLayerBorders_.back().laneYPosition;
                }
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
                float offsetY = mousePos.y - downLayerBorders_[i].heightOfStartSignature;
                // 分割位置を計算
                timeLocation = downLayerBorders_[i].timeOfStartSignature
                    + std::fabsf(float(int(offsetY / dividedHeight))) * (downLayerBorders_[i].beatDuration / divisionRate);
                // ノーツの配置予定座標の計算
                navLineDrawPosY = downLayerBorders_[i].heightOfStartSignature
                    + float(int(offsetY / dividedHeight)) * (downLayerBorders_[i].beatYHeight / divisionRate);

                isContained = true;
                break;
            }
        }

        if(!isContained){
            if(downLayerBorders_.size() != 0){
                if(mousePos.y < downLayerBorders_[0].laneYPosition){
                    timeLocation = downLayerBorders_[0].timeOnLane;
                    navLineDrawPosY = downLayerBorders_[0].laneYPosition;
                } else{
                    timeLocation = downLayerBorders_.back().timeOnLane;
                    navLineDrawPosY = downLayerBorders_.back().laneYPosition;
                }
            }
        }
    }

    if(ImGui::IsMouseDown(0)){
        // マウスを押している間はノーツの配置予定位置を表示
        ImVec2 p1 = ImVec2(laneBorders_[laneIndex], navLineDrawPosY);
        ImVec2 p2 = p1 + ImVec2(laneBorders_[1] - laneBorders_[0], 0.0f); // ノーツの配置予定位置のY座標を少し下げる
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
            // ホールドノーツの作成
            notes_.emplace_back(std::make_unique<Note_RectFlick>());
            if(Note_RectFlick* note = dynamic_cast<Note_RectFlick*>(notes_.back().get())){
                note->layer_ = layer; // レイヤーを設定
                note->laneBit_ = LaneBit::RECTFLICK_RT;
                note->time_ = timeLocation; // 時間を設定
            }
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
        case Warning:
        {
            // 警告ノーツの作成
            notes_.emplace_back(std::make_unique<Note_Warning>());
            if(Note_Warning* note = dynamic_cast<Note_Warning*>(notes_.back().get())){
                note->layer_ = layer; // レイヤーを設定
                note->lane_ = laneIndex; // レーンを設定
                note->time_ = timeLocation; // 時間を設定
                note->laneBit_ = (LaneBit)(1 << laneIndex);
                note->duration_ = 0.5f; // 初期表示時間
            }
            break;
        }
        default:
            break;
        }

        // 作成したノーツを編集状態にする
        edittingNote_ = notes_.back().get();

        // ノーツを時間でソートする
        notes_.sort([](const std::unique_ptr<Note_Base>& a, const std::unique_ptr<Note_Base>& b){
            return a->time_ < b->time_;
        });
    }
}

////////////////////////////////////////////////////////////////////////
// ノーツの表示処理
////////////////////////////////////////////////////////////////////////
void NotesEditor::DisplayNotes(){
#ifdef _DEBUG

    isHoveringNote_ = false;
    float judgeLineY = worldLaneLTPos_.y + laneSize_.y * 0.5f; // ジャッジラインのY座標
    float laneWidth = laneSize_.x / 5.0f; // レーンの幅
    float noteHeight = 20.0f;
    int noteIdx = 0; // ノーツのID
    std::string label;

    for(auto& note : notes_){

        float t = (note->time_ - curLaneTime_) / (kVisibleTime_ * timeScale_);

        // スキップ判定
        if(note->noteType_ != NoteType::Hold){
            if(t < 0.0f || t > 1.0f){ continue; } // 表示範囲外ならスキップ
        } else{
            Note_Hold* holdNote = dynamic_cast<Note_Hold*>(note.get());
            float t2 = (holdNote->time_ + holdNote->kHoldTime_ - curLaneTime_) / (kVisibleTime_ * timeScale_);
            if(t > 1.0f){ continue; } // 表示範囲外ならスキップ
            if(t2 < 0.0f){ continue; } // 表示範囲外ならスキップ
        }

        // 描画位置を計算
        float displayY = judgeLineY + (laneSize_.y * 0.5f) * std::clamp(t, 0.0f, 1.0f) * (note->layer_ == UpDown::UP ? -1.0f : 1.0f);
        float laneX = laneBorders_[note->lane_]; // レーンのX座標

        if(note->noteType_ == NoteType::Tap){
            // ノーツ画像を描画
            ImVec2 p1 = ImVec2(laneX, displayY - noteHeight * 0.5f);
            ImVec2 p2 = p1 + ImVec2(laneWidth, noteHeight); // ノーツのサイズを設定
            int alpha = 128;

            // ノーツにボタン判定を作成
            ImGui::SetCursorScreenPos(p1);
            label = "tapNote##" + std::to_string(noteIdx);

            // クリックしたらノーツを編集状態にする
            if(ImGui::InvisibleButton(label.c_str(), ImVec2(laneWidth, noteHeight))){
                edittingNote_ = note.get();
            }

            // ボタンを押している間ドラッグ状態にする
            if(ImGui::IsItemActive()){
                if(Input::IsMouseMoved()){
                    note->isDragging_ = true; // ドラッグ中にする
                    draggingNote_ = note.get(); // ドラッグ中のノーツを保存
                }
            }

            // ホバー時は不透明にする
            if(ImGui::IsItemHovered()){
                isHoveringNote_ = true;
                alpha = 255;
            }

            // 編集中のノーツは不透明にする
            if(edittingNote_ == note.get()){
                alpha = 255;
            }

            pDrawList_->AddImage(textureIDs_["tap"], p1, p2, ImVec2(0, 0), ImVec2(1, 1), IM_COL32(255, 255, 255, alpha));

        } else if(note->noteType_ == NoteType::Hold){
            Note_Hold* holdNote = dynamic_cast<Note_Hold*>(note.get());
            float t2 = (holdNote->time_ + holdNote->kHoldTime_ - curLaneTime_) / (kVisibleTime_ * timeScale_);
            float displayY2 = judgeLineY + (laneSize_.y * 0.5f) * std::clamp(t2, 0.0f, 1.0f) * (holdNote->layer_ == UpDown::UP ? -1.0f : 1.0f);
            if(displayY2 == displayY){ displayY2 += 1.0f; }

            /*-------------- ボディの描画 -------------*/
            {
                ImVec2 p1 = ImVec2(laneX, displayY);
                ImVec2 p2 = ImVec2(laneX + laneWidth, displayY2);
                int alpha = 128;

                // ノーツにボタン判定を作成
                label = "holdBody##" + std::to_string(noteIdx);
                // クリックしたらノーツを編集状態にする
                if(note->layer_ == UpDown::UP){
                    ImGui::SetCursorScreenPos(ImVec2(p1.x, p2.y) + ImVec2(0.0f, noteHeight * 0.5f));
                    ImVec2 buttonSize = ImVec2(p2.x, p1.y) - ImVec2(p1.x, p2.y) - ImVec2(0.0f, noteHeight);
                    if(ImGui::InvisibleButton(label.c_str(), buttonSize)){
                        edittingNote_ = note.get();
                    }
                } else{
                    ImGui::SetCursorScreenPos(p1 + ImVec2(0.0f, noteHeight * 0.5f));
                    if(ImGui::InvisibleButton(label.c_str(), p2 - p1 - ImVec2(0.0f, noteHeight))){
                        edittingNote_ = note.get();
                    }
                }

                // ボタンを押している間ドラッグ状態にする
                if(ImGui::IsItemActive()){
                    if(Input::IsMouseMoved()){
                        note->isDragging_ = true; // ドラッグ中にする
                        holdNote->isDraggingHoldEnd_ = false;
                        holdNote->isDraggingHoldStart_ = false;
                        draggingNote_ = note.get(); // ドラッグ中のノーツを保存
                        edittingNote_ = note.get();
                    }
                }

                // ホバー時は不透明にする
                if(ImGui::IsItemHovered()){
                    isHoveringNote_ = true;
                    alpha = 255;
                }

                // 編集中のノーツは不透明にする
                if(edittingNote_ == note.get()){
                    alpha = 255;
                }

                pDrawList_->AddImage(textureIDs_["holdBody"], p1, p2, ImVec2(0, 0), ImVec2(1, 1), IM_COL32(255, 255, 255, alpha));
            }

            /*--------------- 先頭を描画 ----------------*/
            if(t >= 0.0f && t <= 1.0f){
                ImVec2 p1 = ImVec2(laneX, displayY - noteHeight * 0.5f);
                ImVec2 p2 = p1 + ImVec2(laneWidth, noteHeight); // ノーツのサイズを設定
                int alpha = 128;

                // ノーツにボタン判定を作成
                ImGui::SetCursorScreenPos(p1);
                label = "holdHead##" + std::to_string(noteIdx);

                // クリックしたらノーツを編集状態にする
                if(ImGui::InvisibleButton(label.c_str(), ImVec2(laneWidth, noteHeight))){
                    edittingNote_ = note.get();
                }

                // ボタンを押している間ドラッグ状態にする
                if(ImGui::IsItemActive()){
                    if(Input::IsMouseMoved()){
                        note->isDragging_ = true; // ドラッグ中にする
                        holdNote->isDraggingHoldStart_ = true; // ホールドの先頭をドラッグ中にする
                        holdNote->isDraggingHoldEnd_ = false; // ホールドの末尾はドラッグ中でない
                        draggingNote_ = note.get(); // ドラッグ中のノーツを保存
                        edittingNote_ = note.get();
                    }
                }

                // ホバー時は不透明にする
                if(ImGui::IsItemHovered()){
                    isHoveringNote_ = true;
                    alpha = 255;
                }

                // 編集中のノーツは不透明にする
                if(edittingNote_ == note.get()){
                    alpha = 255;
                }

                pDrawList_->AddImage(textureIDs_["holdHead"], p1, p2, ImVec2(0, 0), ImVec2(1, 1), IM_COL32(255, 255, 255, alpha));
            }

            /*--------------- 終端を描画 ----------------*/
            if(t2 >= 0.0f && t2 <= 1.0f){
                // ホールドの終端を描画
                ImVec2 p1 = ImVec2(laneX, displayY2 - noteHeight * 0.5f);
                ImVec2 p2 = p1 + ImVec2(laneWidth, noteHeight); // ノーツのサイズを設定
                int alpha = 128;
                // ノーツにボタン判定を作成
                ImGui::SetCursorScreenPos(p1);
                label = "holdEnd##" + std::to_string(noteIdx);

                // クリックしたらノーツを編集状態にする
                if(ImGui::InvisibleButton(label.c_str(), ImVec2(laneWidth, noteHeight))){
                    edittingNote_ = note.get();
                }

                // ボタンを押している間ドラッグ状態にする
                if(ImGui::IsItemActive()){
                    if(Input::IsMouseMoved()){
                        note->isDragging_ = true; // ドラッグ中にする
                        holdNote->isDraggingHoldEnd_ = true; // ホールドの終端をドラッグ中にする
                        holdNote->isDraggingHoldStart_ = false;
                        draggingNote_ = note.get(); // ドラッグ中のノーツを保存
                        edittingNote_ = note.get();
                    }
                }

                // ホバー時は不透明にする
                if(ImGui::IsItemHovered()){
                    isHoveringNote_ = true;
                    alpha = 255;
                }

                // 編集中のノーツは不透明にする
                if(edittingNote_ == note.get()){
                    alpha = 255;
                }

                pDrawList_->AddImage(textureIDs_["holdHead"], p1, p2, ImVec2(0, 0), ImVec2(1, 1), IM_COL32(255, 255, 255, alpha));
            }


        } else if(note->noteType_ == NoteType::RectFlick){
            Note_RectFlick* rectFlickNote = dynamic_cast<Note_RectFlick*>(note.get());
            ImVec2 p1 = ImVec2(laneX, displayY - noteHeight * 0.5f);
            ImVec2 p2 = p1 + ImVec2(laneSize_.x, noteHeight); // ノーツのサイズを設定

            // ノーツにボタン判定を作成
            ImGui::SetCursorScreenPos(p1);
            label = "rectFlickNote##" + std::to_string(noteIdx);
            // クリックしたらノーツを編集状態にする
            if(ImGui::InvisibleButton(label.c_str(), ImVec2(laneSize_.x, noteHeight))){
                edittingNote_ = note.get();
            }
            // ボタンを押している間ドラッグ状態にする
            if(ImGui::IsItemActive()){
                if(Input::IsMouseMoved()){
                    note->isDragging_ = true; // ドラッグ中にする
                    draggingNote_ = note.get(); // ドラッグ中のノーツを保存
                    edittingNote_ = note.get();
                }
            }

            // ホバー時は不透明にする
            int alpha = 128;
            if(ImGui::IsItemHovered()){
                isHoveringNote_ = true;
                alpha = 255;
            }

            // 編集中のノーツは不透明にする
            if(edittingNote_ == note.get()){
                alpha = 255;
            }

            ImGuiCol color = IM_COL32(255, 255, 0, alpha); // 上向きはピンク、下向きは水色
            std::string textureName = laneTextureNameMap_[rectFlickNote->laneBit_]; // レーンに応じたテクスチャ名を取得
            pDrawList_->AddImage(textureIDs_[textureName], p1, p2, ImVec2(0, 0), ImVec2(1, 1), color);

        } else if(note->noteType_ == NoteType::Wheel){
            Note_Wheel* wheelNote = dynamic_cast<Note_Wheel*>(note.get());
            ImVec2 p1 = ImVec2(laneX, displayY - noteHeight * 0.5f);
            ImVec2 p2 = p1 + ImVec2(laneSize_.x, noteHeight); // ノーツのサイズを設定

            // ノーツにボタン判定を作成
            ImGui::SetCursorScreenPos(p1);
            label = "rectFlickNote##" + std::to_string(noteIdx);

            // クリックしたらノーツを編集状態にする
            if(ImGui::InvisibleButton(label.c_str(), ImVec2(laneSize_.x, noteHeight))){
                edittingNote_ = note.get();
            }

            // ボタンを押している間ドラッグ状態にする
            if(ImGui::IsItemActive()){
                if(Input::IsMouseMoved()){
                    note->isDragging_ = true; // ドラッグ中にする
                    draggingNote_ = note.get(); // ドラッグ中のノーツを保存
                    edittingNote_ = note.get();
                }
            }

            // ホバー時は不透明にする
            int alpha = 128;
            if(ImGui::IsItemHovered()){
                isHoveringNote_ = true;
                alpha = 255;
            }

            // 編集中のノーツは不透明にする
            if(edittingNote_ == note.get()){
                alpha = 255;
            }

            float direction = wheelNote->direction_ == UpDown::UP ? 1.0f : -1.0f; // 上向きなら1.0、下向きなら-1.0
            ImGuiCol color = direction == 1.0f ? IM_COL32(255, 0, 255, alpha) : IM_COL32(0, 255, 255, alpha); // 上向きはピンク、下向きは水色
            pDrawList_->AddImage(textureIDs_["wheel"], p1, p2, ImVec2(0, 0), ImVec2(direction, direction), color);
        

        }else if(note->noteType_ == NoteType::Warning){

            // 終点
            Note_Warning* warning = dynamic_cast<Note_Warning*>(note.get());
            float t2 = (warning->time_ + warning->duration_ - curLaneTime_) / (kVisibleTime_ * timeScale_);
            float displayY2 = judgeLineY + (laneSize_.y * 0.5f) * std::clamp(t2, 0.0f, 1.0f) * (warning->layer_ == UpDown::UP ? -1.0f : 1.0f);
            if(displayY2 == displayY){ displayY2 += 1.0f; }

            /*-------------- ボディの描画 -------------*/
            {
                ImVec2 p1 = ImVec2(laneX, displayY);
                ImVec2 p2 = ImVec2(laneX + laneWidth, displayY2);
                int alpha = 64;

                // ノーツにボタン判定を作成
                label = "warningNoteBody##" + std::to_string(noteIdx);
                // クリックしたらノーツを編集状態にする
                if(note->layer_ == UpDown::UP){
                    ImGui::SetCursorScreenPos(ImVec2(p1.x, p2.y) + ImVec2(0.0f, noteHeight * 0.5f));
                    ImVec2 buttonSize = ImVec2(p2.x, p1.y) - ImVec2(p1.x, p2.y) - ImVec2(0.0f, noteHeight);
                    if(ImGui::InvisibleButton(label.c_str(), buttonSize)){
                        edittingNote_ = note.get();
                    }
                } else{
                    ImGui::SetCursorScreenPos(p1 + ImVec2(0.0f, noteHeight * 0.5f));
                    if(ImGui::InvisibleButton(label.c_str(), p2 - p1 - ImVec2(0.0f, noteHeight))){
                        edittingNote_ = note.get();
                    }
                }

                // ボタンを押している間ドラッグ状態にする
                if(ImGui::IsItemActive()){
                    if(Input::IsMouseMoved()){
                        note->isDragging_ = true; // ドラッグ中にする
                        draggingNote_ = note.get(); // ドラッグ中のノーツを保存
                        edittingNote_ = note.get();
                    }
                }

                // ホバー時は不透明にする
                if(ImGui::IsItemHovered()){
                    isHoveringNote_ = true;
                    alpha = 128;
                }

                // 編集中のノーツは不透明にする
                if(edittingNote_ == note.get()){
                    alpha = 128;
                }

                pDrawList_->AddImage(textureIDs_["white"], p1, p2, ImVec2(0, 0), ImVec2(1, 1), IM_COL32(255, 0, 0, alpha));
            }


            {
                // ノーツ画像を描画
                ImVec2 p1 = ImVec2(laneX, displayY - noteHeight * 0.5f);
                ImVec2 p2 = p1 + ImVec2(laneWidth, noteHeight); // ノーツのサイズを設定
                int alpha = 128;

                // ノーツにボタン判定を作成
                ImGui::SetCursorScreenPos(p1);
                label = "warningNoteHead##" + std::to_string(noteIdx);

                // クリックしたらノーツを編集状態にする
                if(ImGui::InvisibleButton(label.c_str(), ImVec2(laneWidth, noteHeight))){
                    edittingNote_ = note.get();
                }

                // ボタンを押している間ドラッグ状態にする
                if(ImGui::IsItemActive()){
                    if(Input::IsMouseMoved()){
                        note->isDragging_ = true; // ドラッグ中にする
                        draggingNote_ = note.get(); // ドラッグ中のノーツを保存
                    }
                }

                // ホバー時は不透明にする
                if(ImGui::IsItemHovered()){
                    isHoveringNote_ = true;
                    alpha = 255;
                }

                // 編集中のノーツは不透明にする
                if(edittingNote_ == note.get()){
                    alpha = 255;
                }


                pDrawList_->AddImage(textureIDs_["warning"], p1, p2, ImVec2(0, 0), ImVec2(1, 1), IM_COL32(255, 255, 255, alpha));
            }
        }

        noteIdx++; // ノーツのIDをインクリメント
    }
#endif // _DEBUG
}

////////////////////////////////////////////////////////////////////////
// ドラッグ中のノーツの処理
////////////////////////////////////////////////////////////////////////
void NotesEditor::DraggingNote(){
#ifdef _DEBUG
    if(!draggingNote_){
        return; // ドラッグ中のノーツがないなら何もしない
    }

    // マウス座標がレーンの範囲外なら何もしない
    ImVec2 mousePos = ImGui::GetMousePos();
    ImVec2 windowPos = ImGui::GetWindowPos();
    ImVec2 worldLaneRB = worldLaneLTPos_ + laneSize_;
    bool isOutOfLane = false; // レーン外フラグ


    // マウスを押していなければ何もしない
    if(!ImGui::IsMouseDown(0) && !ImGui::IsMouseReleased(0)){
        return;
    }

    // レーンの範囲外か確認
    if(mousePos.x < worldLaneLTPos_.x || mousePos.x > worldLaneRB.x ||
        mousePos.y < worldLaneLTPos_.y || mousePos.y > worldLaneRB.y){
        isOutOfLane = true; // レーン外フラグを立てる
    }

    // レーン外ならドラッグ中のノーツをクリア
    if(ImGui::IsMouseClicked(0)){
        if(isOutOfLane){
            draggingNote_ = nullptr;
            return;
        }
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
    float clampedX = std::clamp(mousePos.x, worldLaneLTPos_.x, worldLaneRB.x);
    for(int i = 0; i < laneBorders_.size(); i++){
        if(clampedX >= laneBorders_[i]){
            laneIndex = i; // レーンのインデックスを取得
        }
    }

    // レーン上のどこに配置するかを計算
    float timeLocation = 0.0f;
    float divisionRate = division_ / 4.0f;
    bool isContained = false;

    if(layer == UpDown::UP){
        for(int i = 0; i < upLayerBorders_.size(); i++){
            int nextIdx = std::clamp(i + 1, 0, int(upLayerBorders_.size()) - 1);

            if(MyFunc::IsContain(
                { upLayerBorders_[nextIdx].laneYPosition,upLayerBorders_[i].laneYPosition },
                mousePos.y)){

                // さらに分割数に応じて位置を調整
                float dividedHeight = upLayerBorders_[i].beatYHeight / divisionRate;
                // マウスのY座標から分割位置を計算
                float offsetY = mousePos.y - upLayerBorders_[i].heightOfStartSignature;
                // 分割位置を計算
                timeLocation = upLayerBorders_[i].timeOfStartSignature
                    + std::fabsf(float(int(offsetY / dividedHeight))) * (upLayerBorders_[i].beatDuration / divisionRate);

                isContained = true;
                break;
            }
        }

        if(!isContained){
            if(upLayerBorders_.size() != 0){
                if(mousePos.y > upLayerBorders_[0].laneYPosition){
                    timeLocation = upLayerBorders_[0].timeOnLane;
                } else{
                    timeLocation = upLayerBorders_.back().timeOnLane;
                }
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
                float offsetY = mousePos.y - downLayerBorders_[i].heightOfStartSignature;
                // 分割位置を計算
                timeLocation = downLayerBorders_[i].timeOfStartSignature
                    + std::fabsf(float(int(offsetY / dividedHeight))) * (downLayerBorders_[i].beatDuration / divisionRate);

                isContained = true;
                break;
            }
        }

        if(!isContained){
            if(downLayerBorders_.size() != 0){
                if(mousePos.y < downLayerBorders_[0].laneYPosition){
                    timeLocation = downLayerBorders_[0].timeOnLane;
                } else{
                    timeLocation = downLayerBorders_.back().timeOnLane;
                }
            }
        }
    }

    if(ImGui::IsMouseDown(0)){

        // ノーツの更新
        switch(draggingNote_->noteType_){
        case NoteType::Tap:
        {
            // タップノーツの移動
            if(Note_Tap* note = dynamic_cast<Note_Tap*>(draggingNote_)){
                note->layer_ = layer; // レイヤーを設定
                note->lane_ = laneIndex; // レーンを設定
                note->time_ = timeLocation; // 時間を設定
                note->laneBit_ = (LaneBit)(1 << laneIndex);
            }
            break;
        }
        case NoteType::Hold:
        {
            // ホールドノーツの移動・編集
            if(Note_Hold* note = dynamic_cast<Note_Hold*>(draggingNote_)){

                // 両端をドラッグしながら上下に出たら、時間をずらす
                if(note->isDraggingHoldEnd_ || note->isDraggingHoldStart_){
                    if(layer == UpDown::UP){
                        if(mousePos.y < worldLaneLTPos_.y){
                            curLaneTime_ += ClockManager::DeltaTime() * timeScale_; // 上にドラッグしたら時間を進める
                            curLaneTime_ = std::clamp(curLaneTime_, 0.0f, FLT_MAX); // 時間を0以上に制限
                            timeLocation = curLaneTime_ + kVisibleTime_ * timeScale_;
                        }
                    } else{
                        if(mousePos.y > worldLaneLTPos_.y + laneSize_.y){
                            curLaneTime_ += ClockManager::DeltaTime() * timeScale_; // 下にドラッグしたら時間を進める
                            curLaneTime_ = std::clamp(curLaneTime_, 0.0f, FLT_MAX); // 時間を0以上に制限
                            timeLocation = curLaneTime_ + kVisibleTime_ * timeScale_;
                        }
                    }
                }

                if(note->isDraggingHoldEnd_){
                    if(layer != note->layer_){
                        curLaneTime_ -= ClockManager::DeltaTime() * timeScale_;
                        curLaneTime_ = std::clamp(curLaneTime_, 0.0f, FLT_MAX); // 時間を0以上に制限
                        timeLocation = curLaneTime_;
                    }
                    note->kHoldTime_ = std::clamp(timeLocation - note->time_, 0.1f, FLT_MAX); // ホールド時間を更新

                } else if(note->isDraggingHoldStart_){
                    if(layer != note->layer_){
                        curLaneTime_ -= ClockManager::DeltaTime() * timeScale_;
                        curLaneTime_ = std::clamp(curLaneTime_, 0.0f, FLT_MAX); // 時間を0以上に制限
                        timeLocation = curLaneTime_; // レイヤーが変わったら時間を現在の時間に戻す
                    }
                    note->kHoldTime_ -= timeLocation - note->time_;
                    note->kHoldTime_ = std::clamp(note->kHoldTime_, 0.1f, FLT_MAX);
                    timeLocation = std::clamp(timeLocation, 0.0f, note->time_ + note->kHoldTime_);
                    note->time_ = timeLocation; // ホールドの開始時間を更新

                } else{
                    note->layer_ = layer; // レイヤーを設定
                    note->lane_ = laneIndex; // レーンを設定
                    note->laneBit_ = (LaneBit)(1 << laneIndex);
                    note->time_ = timeLocation; // 時間を設定
                }

            }

            break;
        }
        case NoteType::RectFlick:
        {
            // ホイールノーツの作成
            if(Note_RectFlick* note = dynamic_cast<Note_RectFlick*>(draggingNote_)){
                note->layer_ = layer; // レイヤーを設定
                note->time_ = timeLocation; // 時間を設定
            }
            break;
        }
        case NoteType::Wheel:
        {
            // ホイールノーツの作成
            if(Note_Wheel* note = dynamic_cast<Note_Wheel*>(draggingNote_)){
                note->layer_ = layer; // レイヤーを設定
                note->time_ = timeLocation; // 時間を設定
            }
            break;
        }
        case NoteType::Warning:
        {
            // タップノーツの移動
            if(Note_Warning* note = dynamic_cast<Note_Warning*>(draggingNote_)){
                note->layer_ = layer; // レイヤーを設定
                note->lane_ = laneIndex; // レーンを設定
                note->time_ = timeLocation; // 時間を設定
                note->laneBit_ = (LaneBit)(1 << laneIndex);
            }

            break;
        }
        default:
            break;
        }
    }

    // マウスを離したら
    if(ImGui::IsMouseReleased(0)){
        draggingNote_->isDragging_ = false; // ドラッグ中フラグを解除
        draggingNote_ = nullptr; // ドラッグ中のノーツをクリア

        if(Note_Hold* holdNote = dynamic_cast<Note_Hold*>(draggingNote_)){
            holdNote->isDraggingHoldStart_ = false; // ホールドの先頭ドラッグフラグを解除
            holdNote->isDraggingHoldEnd_ = false; // ホールドの終端ドラッグフラグを解除
        }
    }

#endif // _DEBUG
}

// 画面をスクロールする処理
void NotesEditor::ScrollOnLane(){

    // マウス座標がレーンの範囲外なら何もしない
    ImVec2 mousePos = ImGui::GetMousePos();
    ImVec2 windowPos = ImGui::GetWindowPos();
    ImVec2 worldLaneRB = worldLaneLTPos_ + laneSize_;
    float laneCenterY = worldLaneLTPos_.y + laneSize_.y * 0.5f;
    static bool isScrolling = false; // スクロール中フラグ
    static bool isPause = false;
    static UpDown scrollLayer = UpDown::NONE;
    static ImVec2 prevMousePos; // 前回のマウス座標
    static float savedTimeVelocity;

    if(ImGui::IsMouseClicked(1)){

        // レーンの範囲外なら終了
        if(mousePos.x < worldLaneLTPos_.x || mousePos.x > worldLaneRB.x ||
            mousePos.y < worldLaneLTPos_.y || mousePos.y > worldLaneRB.y){
            return;
        }

        // 再生の停止
        AudioManager::EndAudio(audioHandle_);

        if(isPlaying_){
            isPause = true; // 一時停止フラグを立てる
            isPlaying_ = false; // 再生中フラグを解除
        }

        // 上下どちらにマウスがあるかを判定
        if(mousePos.y < worldLaneLTPos_.y + laneSize_.y * 0.5f){
            scrollLayer = UpDown::UP; // 上半分
        } else{
            scrollLayer = UpDown::DOWN; // 下半分
        }

        isScrolling = true;

    } else if(ImGui::IsMouseDown(1)){

        if(!isScrolling){ return; }
        ImVec2 dif = mousePos - prevMousePos;
        float t[2]{};

        if(scrollLayer == UpDown::DOWN){
            t[0] = (prevMousePos.y - laneCenterY) / (laneSize_.y * 0.5f);
            t[1] = (mousePos.y - laneCenterY) / (laneSize_.y * 0.5f);
        } else{
            t[0] = (prevMousePos.y - laneCenterY) / (-laneSize_.y * 0.5f);
            t[1] = (mousePos.y - laneCenterY) / (-laneSize_.y * 0.5f);
        }

        float difT = t[0] - t[1];
        float timeVelocity = kVisibleTime_ * timeScale_ * difT;
        curLaneTime_ += timeVelocity;
        savedTimeVelocity = timeVelocity;

    } else{
        isScrolling = false;
        savedTimeVelocity *= 0.9f;
        curLaneTime_ += savedTimeVelocity;

        // 動きが小さくなったら再生を再開
        if(isPause){
            if(std::fabsf(savedTimeVelocity) < 0.001f){
                if(!audioFilePath_.empty()){
                    audioHandle_ = AudioManager::PlayAudio(audioFilePath_, false, 1.0f, curLaneTime_); // 音声を再生
                }
                isPause = false; // 再生中フラグを設定
                isPlaying_ = true; // 再生中フラグを設定
            }
        }
    }
    prevMousePos = mousePos;
    curLaneTime_ = std::clamp(curLaneTime_, 0.0f, duration_);
}


/////////////////////////////////////////////////////////////////////////
// 選択中のノーツの編集処理
/////////////////////////////////////////////////////////////////////////
void NotesEditor::EditSelectNote(){
}

/////////////////////////////////////////////////////////////////////////
// ノーツの削除処理
/////////////////////////////////////////////////////////////////////////
void NotesEditor::EraseCheck(){
    notes_.remove_if([](const std::unique_ptr<Note_Base>& note){
        return note->isEnd_;
    });

    tempoDataList_.remove_if([](const TempoData& data){return data.removeFlag; });
}

/////////////////////////////////////////////////////////////////////////
// メトロノームの再生処理
/////////////////////////////////////////////////////////////////////////
void NotesEditor::PlayMetronome(){

    static float preLanetime;

    for(const TempoData& tempoData : tempoDataList_){

        float borderTime = tempoData.time + metronomeOffsetTime_;

        // テンポデータの範囲内にいるか確認
        if(borderTime <= curLaneTime_ && borderTime + tempoData.CalcDuration() > curLaneTime_){

            // データが切り替わった瞬間の場合
            if(preLanetime < tempoData.time){
                AudioManager::PlayAudio(metronomeSEFileName_, false, 2.0f); // メトロノーム音を再生
            } else{
                float beatDuration = 60.0f / tempoData.bpm; // 一拍の長さを計算
                int beatCount[2] = {
                    int((curLaneTime_ - borderTime) / beatDuration), // 現在のビート数を計算
                    int((preLanetime - borderTime) / beatDuration) // 前回のビート数を計算
                };

                // ビート数が変わった場合のみメトロノーム音を再生
                if(beatCount[0] != beatCount[1]){
                    if(beatCount[0] % tempoData.timeSignature_numerator == 0){
                        AudioManager::PlayAudio(metronomeSEFileName_, false, 2.0f); // メトロノーム音を再生
                    } else{
                        AudioManager::PlayAudio(metronomeSEFileName_, false, 1.0f); // メトロノーム音を再生（弱拍）
                    }
                }
            }
        }
    }

    preLanetime = curLaneTime_;
}

void NotesEditor::PlayAnswerSE(){

    static float preLaneTime;

    // 2分探索で近いノーツを探す
    auto it = std::lower_bound(notes_.begin(), notes_.end(), curLaneTime_, [&](const std::unique_ptr<Note_Base>& note, float time){
        return note->time_ + answerOffsetTime_ < time;
    });

    // 通り過ぎたばかりの判定のノーツの音を鳴らす
    while(true){
        if(it != notes_.begin()){
            --it; // 1つ前のノーツを取得
            if(it->get()->time_ + answerOffsetTime_ > preLaneTime){
                AudioManager::PlayAudio(answerSEFileName_, false, 1.0f); // 判定音を再生
            } else{
                break;
            }
        } else{
            break;
        }
    }

    preLaneTime = curLaneTime_; // 前回のレーン時間を更新
}

///////////////////////////////////////////////////////////////////////
// ファイル操作の処理
///////////////////////////////////////////////////////////////////////
void NotesEditor::FileControl(){

    static bool isOverwrite = false; // 上書き確認フラグ
    static std::string directoryPath{};
    static nlohmann::json jsonData{};

    if(ImGui::Button("保存")){
        ImGui::OpenPopup("SaveFilePopup");
        isEditOnLane_ = false; // レーン上の編集を無効化

        // 拡張子、ディレクトリを除去して曲名を取得
        std::string songName = audioFilePath_;
        if(!songName.empty()){
            size_t lastSlash = songName.find_last_of("/\\");
            if(lastSlash != std::string::npos){
                songName = songName.substr(lastSlash + 1); // ディレクトリを除去
            }
            size_t lastDot = songName.find_last_of('.');
            if(lastDot != std::string::npos){
                songName = songName.substr(0, lastDot); // 拡張子を除去
            }
        }

        if(saveDifficultyName_.empty()){
            saveSongName_ = songName; // 曲名を保存
        }
    }


    static std::filesystem::path loadFilePath = "Resources/NoteDatas/"; // 読み込み先のパスを設定
    std::string selectedFile = ImFunc::FolderView("譜面データ一覧", loadFilePath);
    selectedFile;


    if(ImGui::BeginPopup("SaveFilePopup")){

        ImGui::SliderInt("難易度", &difficulty_, 1, 15);
        ImFunc::InputText("曲名", saveSongName_);
        ImFunc::ComboText("譜面難易度の選択", saveDifficultyName_, { "Basic","Expert","Master","Parallel", });
        ImFunc::InputText("アーティスト名", artistName_);
        ImFunc::InputText("譜面制作者名", notesDesignerName_);
        ImFunc::Combo("ジャンル", songGenre_, { "Original", "GameMusic" });

        if(ImGui::Button("保存")){

            // 拡張子、ディレクトリを除去して曲名を取得
            std::string songName = audioFilePath_;
            if(!songName.empty()){
                size_t lastSlash = songName.find_last_of("/\\");
                if(lastSlash != std::string::npos){
                    songName = songName.substr(lastSlash + 1); // ディレクトリを除去
                }
                size_t lastDot = songName.find_last_of('.');
                if(lastDot != std::string::npos){
                    songName = songName.substr(0, lastDot); // 拡張子を除去
                }
            }

            // 保存先のパスを設定
            directoryPath = "Resources/NoteDatas/" + songName + "/";
            jsonFilePath_ = directoryPath + saveDifficultyName_ + ".json";
            jsonData = ToJson(); // JSONデータを取得

            // 同じデータのファイルが存在するか確認
            if(std::filesystem::exists(jsonFilePath_)){
                // ファイルが存在する場合、上書き確認ダイアログを表示
                isOverwrite = true;

            } else{
                // ファイルが存在しない場合はそのまま保存
                std::filesystem::create_directories(directoryPath); // ディレクトリを作成
                std::ofstream file(jsonFilePath_);
                file << jsonData.dump(4); // JSONデータをファイルに書き出し
                file.close();
                ImGui::CloseCurrentPopup();
            }
        }

        // キャンセルボタン
        ImGui::SameLine();
        if(ImGui::Button("キャンセル")){
            ImGui::CloseCurrentPopup(); // ポップアップを閉じる
        }

        ImGui::EndPopup();


    }

    // 上書き確認ダイアログの表示
    if(isOverwrite){
        ImGui::OpenPopup("上書き確認");
        isOverwrite = false; // フラグをリセット
    }

    if(ImGui::BeginPopupModal("上書き確認", nullptr, ImGuiWindowFlags_AlwaysAutoResize)){
        ImGui::Text("同じ名前のファイルが存在します。上書きしますか？");
        if(ImGui::Button("はい")){
            std::ofstream file(jsonFilePath_);
            file << jsonData.dump(4); // JSONデータをファイルに書き出し
            file.close();
            ImGui::CloseCurrentPopup(); // ポップアップを閉じる
        }
        ImGui::SameLine();
        if(ImGui::Button("いいえ")){
            ImGui::CloseCurrentPopup(); // ポップアップを閉じる
        }
        ImGui::EndPopup();
    }
}


///////////////////////////////////////////////////////////////////////
// JSONから譜面データを読み込む処理
///////////////////////////////////////////////////////////////////////
void NotesEditor::LoadFromJson(const nlohmann::json& jsonData){

    notes_.clear(); // 既存のノーツデータをクリア
    tempoDataList_.clear();
    float timeOffset = 0.0f;

    // ノーツデータの読み込み
    if(jsonData.contains("notes")){
        for(const auto& noteJson : jsonData["notes"]){
            std::string noteType = noteJson["noteType"];

            if(noteType == "tap"){
                Note_Tap* note = new Note_Tap();
                note->FromJson(noteJson);
                notes_.emplace_back(note);
            } else if(noteType == "hold" or noteType == "Hold"){
                Note_Hold* note = new Note_Hold();
                note->FromJson(noteJson);
                notes_.emplace_back(note);
            } else if(noteType == "rectFlick" or noteType == "RectFlick"){
                Note_RectFlick* note = new Note_RectFlick();
                note->FromJson(noteJson);
                notes_.emplace_back(note);
            } else if(noteType == "wheel"){
                Note_Wheel* note = new Note_Wheel();
                note->FromJson(noteJson);
                notes_.emplace_back(note);
            } else if(noteType == "warning"){
                Note_Warning* note = new Note_Warning();
                note->FromJson(noteJson);
                notes_.emplace_back(note);
            } else{
                // 未知のノーツタイプは無視
            }
        }
    }
    // テンポデータの読み込み
    if(jsonData.contains("tempoData")){
        for(const auto& tempoJson : jsonData["tempoData"]){
            TempoData tempoData;
            tempoData.FromJson(tempoJson);
            tempoDataList_.push_back(tempoData);
        }
    }

    // その他のデータの読み込み
    if(jsonData.contains("jsonFilePath")){
        jsonFilePath_ = jsonData["jsonFilePath"];
    }

    if(jsonData.contains("songName")){
        saveSongName_ = jsonData["songName"];
    }
    if(jsonData.contains("audioPath")){
        audioFilePath_ = jsonData["audioPath"];
    }

    if(jsonData.contains("difficultyName")){
        saveDifficultyName_ = jsonData["difficultyName"];
    }

    difficulty_ = jsonData["difficulty"];
    artistName_ = jsonData["artist"];
    notesDesignerName_ = jsonData["notesDesigner"];
    songGenre_ = (SongGenre)jsonData["genre"];


    // 譜面開始前の待機時間を削除(編集時には不要)
    if(!tempoDataList_.empty()){
        timeOffset = tempoDataList_.front().CalcDuration();
        tempoDataList_.pop_front();

        for(auto& note : notes_){
            note->time_ -= timeOffset;
        }

        for(auto& tempoData : tempoDataList_){
            tempoData.time -= timeOffset;
        }
    }

    // 各種ポインタのクリア
    edittingNote_ = nullptr;
    draggingNote_ = nullptr;
    selectedTempoData_ = nullptr;
    contextMenuTempoData_ = nullptr;
}

///////////////////////////////////////////////////////////////////////
// JSONに譜面データを書き出す処理
///////////////////////////////////////////////////////////////////////
nlohmann::json NotesEditor::ToJson(){
    nlohmann::json jsonData;
    float timeOffset = 0.0f;

    // 譜面開始前の待機時間を追加
    if(!tempoDataList_.empty()){
        TempoData standbyTimeData = tempoDataList_.front();
        standbyTimeData.barCount = 1;
        standbyTimeData.time = 0.0f;
        tempoDataList_.emplace_front(standbyTimeData);
        timeOffset = standbyTimeData.CalcDuration();
    }

    // ノーツデータの書き出し
    for(auto& note : notes_){
        note->time_ += timeOffset;
        jsonData["notes"].push_back(note->ToJson());
        note->time_ -= timeOffset;
    }

    // テンポデータの書き出し
    for(auto& tempoData : tempoDataList_){
        tempoData.time += timeOffset;
        jsonData["tempoData"].push_back(tempoData.ToJson());
        tempoData.time -= timeOffset;
    }

    tempoDataList_.pop_front(); // 譜面開始前の待機時間データを削除

    // テンポ情報からもっとも時間割合の大きいBPMを取得
    std::unordered_map<float, float> bpmDurationMap;
    for(const TempoData& tempoData : tempoDataList_){
        bpmDurationMap[tempoData.bpm] += tempoData.CalcDuration();
    }

    // 最も時間割合の大きいBPMを取得(secondが最大値のキー)
    float bpm = 0.0f;
    float maxDuration = 0.0f;
    for(const auto& pair : bpmDurationMap){
        if(pair.second > maxDuration){
            maxDuration = pair.second;
            // 小数点以下を切り捨ててBPMを取得
            bpm = float(int(pair.first));
        }
    }

    // コンボ数の計算
    int comboCount = 0;
    for(const auto& note : notes_){
        if(note->noteType_ != NoteType::Hold){
            if(note->noteType_ == NoteType::Warning){
                continue; // ワーニングノーツはコンボに含めない
            }
            comboCount++; // タップノーツとホールドノーツの数をカウント
        } else{
            comboCount += 2; // ホールドノーツは開始と終了で2つカウント
        }
    }

    // その他のデータの書き出し
    jsonData["jsonFilePath"] = jsonFilePath_;
    jsonData["songName"] = saveSongName_;
    jsonData["audioPath"] = audioFilePath_;
    jsonData["difficulty"] = difficulty_;
    jsonData["difficultyName"] = saveDifficultyName_;
    jsonData["artist"] = artistName_;
    jsonData["notesDesigner"] = notesDesignerName_;
    jsonData["bpm"] = bpm;
    jsonData["maxCombo"] = comboCount; // コンボ数を保存
    jsonData["genre"] = (int32_t)songGenre_; // ジャンルを保存

    return jsonData;
}
