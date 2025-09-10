#include "StageAchievementCollector.h"

//============================================================================
//	include
//============================================================================
#include <SEED/Lib/JsonAdapter/JsonAdapter.h>

// imgui
#include <SEED/Source/Manager/ImGuiManager/ImGuiManager.h>

StageProgressCollector::~StageProgressCollector() {
    // json保存
    SaveJson();
}

void StageProgressCollector::Initialize() {
    // json適応
    ApplyJson();
}

void StageProgressCollector::SetStageClear(int stageID, bool isClear) {
    stageClearMap_[stageID] = isClear;
}

bool StageProgressCollector::IsStageClear(int stageID) const {
    auto it = stageClearMap_.find(stageID);
    if (it != stageClearMap_.end()) {
        return it->second;
    }
    return false; // ステージが存在しない場合はクリアしていないとみなす
}

void StageProgressCollector::Edit() {
#ifdef _DEBUG
    ImFunc::CustomBegin("StageProgressCollector", MoveOnly_TitleBar);
    {
        if (ImGui::Button("Save Json")) {

            SaveJson();
        }
        ImGui::SameLine();

        if (ImGui::CollapsingHeader("StageProgress")) {
            for (auto& [stageID, isClear] : stageClearMap_) {
                std::string label = "Stage " + std::to_string(stageID);
                ImGui::Checkbox(label.c_str(), &isClear);
            }
        }
        ImGui::End();
    }

#endif // _DEBUG
}

void StageProgressCollector::SaveJson() {
    nlohmann::json jsonData;
    nlohmann::json stagesJson;

    for (const auto& [stageID, isClear] : stageClearMap_) {
        stagesJson[std::to_string(stageID)]["isClear"] = isClear;
    }

    jsonData["stages"] = stagesJson;

    JsonAdapter::Save("Progress/StageProgress.json", jsonData);
}

void StageProgressCollector::ApplyJson() {
    nlohmann::json data;
    if (!JsonAdapter::LoadCheck("Progress/StageProgress.json", data)) {
        return;
    }

    stageClearMap_.clear();

    if (!data.contains("stages")) {
        return;
    }

    auto& stages = data["stages"];
    for (auto it = stages.begin(); it != stages.end(); ++it) {
        try {
            int stageID = std::stoi(it.key());
            bool isClear = it.value().at("isClear").get<bool>();
            stageClearMap_[stageID] = isClear;
        }
        catch (...) {
            // JSONフォーマットがおかしい場合はスキップ
            continue;
        }
    }
}