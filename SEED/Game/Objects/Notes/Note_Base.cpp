#include "Note_Base.h"
#include <SEED/Source/SEED.h>

Note_Base::Note_Base(){
    noteID_ = nextNoteID_++;
    noteQuad_ = std::make_unique<Quad>(MakeEqualQuad(1.0f, {1.0f,1.0f,1.0f,1.0f}));
}

void Note_Base::Draw(float currentTime, float appearLength){
    currentTime;
    appearLength;
    SEED::DrawQuad(*noteQuad_.get());
}

#ifdef _DEBUG
void Note_Base::Edit(){
    ImGui::Text("ノーツID: %d", noteID_);
    ImGui::DragFloat("配置時間", &time_, 0.001f, 0.0f,FLT_MAX, "%.3f",ImGuiSliderFlags_AlwaysClamp);
    ImGui::Checkbox("EXノーツかどうか", &isExtraNote_);
}
#endif // _DEBUG
