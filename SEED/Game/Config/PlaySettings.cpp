#include "PlaySettings.h"
#include <fstream>
#include <string>
#include <SEED/Source/Manager/ImGuiManager/ImGuiManager.h>

PlaySettings::PlaySettings(){
    // jsonから読み込み
    std::string filePath = "Resources/Jsons/Settings/PlaySettings.json";
    std::ifstream file(filePath);

    if (file.is_open()) {
        nlohmann::json jsonData;
        file >> jsonData;
        kNoteAppearTime = jsonData["kNoteAppearTime"];
        laneNoteSpeedRate_ = jsonData["laneNoteSpeedRate"];
        offset_judge_ = jsonData["offset_judge"];
        offset_view_ = jsonData["offset_view"];
        offset_answerSE_ = jsonData["offset_answerSE"];
        isReverseLR_ = jsonData["isReverseLR"];
        isReverseUD_ = jsonData["isReverseUD"];
        file.close();
    }
}

// デストラクタ
PlaySettings::~PlaySettings(){

    // jsonに保存
    std::string filePath = "Resources/Jsons/Settings/PlaySettings.json";

    // ファイルを作成
    nlohmann::json jsonData;
    jsonData["kNoteAppearTime"] = kNoteAppearTime;
    jsonData["laneNoteSpeedRate"] = laneNoteSpeedRate_;
    jsonData["offset_judge"] = offset_judge_;
    jsonData["offset_view"] = offset_view_;
    jsonData["offset_answerSE"] = offset_answerSE_;
    jsonData["isReverseLR"] = isReverseLR_;
    jsonData["isReverseUD"] = isReverseUD_;

    // ファイルに書き込む
    std::ofstream file(filePath);
    file << jsonData.dump(4); // JSONデータをファイルに書き出し
    file.close();
}

PlaySettings* PlaySettings::GetInstance(){
    if(instance_ == nullptr){
        instance_ = std::unique_ptr<PlaySettings>(new PlaySettings());
    }
    return instance_.get();
}


#ifdef _DEBUG

void PlaySettings::Edit(){
    ImFunc::CustomBegin("PlaySettings", MoveOnly_TitleBar);

    ImGui::DragFloat("kNoteAppearTime", &kNoteAppearTime, 0.001f, 0.0f, 10.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
    ImGui::DragFloat("laneNoteSpeedRate", &laneNoteSpeedRate_, 0.001f, 0.0f, 10.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
    ImGui::DragFloat("offset_judge", &offset_judge_, 0.001f, -10.0f, 10.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
    ImGui::DragFloat("offset_view", &offset_view_, 0.001f, -10.0f, 10.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
    ImGui::DragFloat("offset_answerSE", &offset_answerSE_, 0.001f, -10.0f, 10.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
    ImGui::Checkbox("isReverseLR", &isReverseLR_);
    ImGui::Checkbox("isReverseUD", &isReverseUD_);

    ImGui::End();
}

#endif // _DEBUG