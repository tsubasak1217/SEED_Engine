#include "Note_Base.h"
#include <SEED/Source/SEED.h>

Note_Base::Note_Base(){
    noteID_ = nextNoteID_++;
    noteQuad_ = std::make_unique<SEED::Topology::Quad>(SEED::Methods::Shape::MakeEqualQuad(1.0f, {1.0f,1.0f,1.0f,1.0f}));
}

void Note_Base::Draw(float currentTime, float appearLength){
    currentTime;
    appearLength;
   SEED::Instance::DrawQuad(*noteQuad_.get());
}

nlohmann::json Note_Base::ToJson() {
    nlohmann::json j;
    j["lane"] = lane_;
    j["layer"] = (int)layer_;
    j["laneBit"] = (int)laneBit_;
    j["time"] = time_;
    j["isExtraNote"] = isExtraNote_;
    return j;
}

void Note_Base::FromJson(const nlohmann::json& j) {
    lane_ = j["lane"];
    layer_ = (SEED::GeneralEnum::UpDown)j["layer"];
    laneBit_ = (LaneBit)j["laneBit"];
    time_ = j["time"];
    isExtraNote_ = j["isExtraNote"];
}


#ifdef _DEBUG
void Note_Base::Edit(){
    ImGui::Text("ノーツID: %d", noteID_);
    ImGui::DragFloat("配置時間", &time_, 0.001f, 0.0f,FLT_MAX, "%.3f",ImGuiSliderFlags_AlwaysClamp);
    ImGui::Checkbox("EXノーツかどうか", &isExtraNote_);
}
#endif // _DEBUG
