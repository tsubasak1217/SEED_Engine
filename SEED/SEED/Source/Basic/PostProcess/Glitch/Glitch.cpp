#include "Glitch.h"

//============================================================================
//	include
//============================================================================
#include <SEED/Source/Manager/ClockManager/ClockManager.h>

//============================================================================
//	Glitch classMethods
//============================================================================

Glitch::Glitch() : IPostProcess() {
}

void Glitch::Initialize() {

    // パイプラインの作成
    PSOManager::CreatePipelines("Glitch.pip");
}

void Glitch::Update() {

    // 時間の更新処理
    time_ += ClockManager::DeltaTime();
}

void Glitch::Apply() {

    // バインド処理
    PSOManager::SetBindInfo("Glitch.pip", "inputTexture",
        ViewManager::GetHandleGPU(HEAP_TYPE::SRV_CBV_UAV, GetCurSRVBufferName()));
    PSOManager::SetBindInfo("Glitch.pip", "noiseTexture",
        ViewManager::GetHandleGPU(HEAP_TYPE::SRV_CBV_UAV, GetCurSRVBufferName()));
    PSOManager::SetBindInfo("Glitch.pip", "outputTexture",
        ViewManager::GetHandleGPU(HEAP_TYPE::SRV_CBV_UAV, GetCurUAVBufferName()));
    // CBuffer
    PSOManager::SetBindInfo("Glitch.pip", "time", &time_);
    PSOManager::SetBindInfo("Glitch.pip", "intensity", &intensity_);
    PSOManager::SetBindInfo("Glitch.pip", "blockSize", &blockSize_);
    PSOManager::SetBindInfo("Glitch.pip", "colorOffset", &colorOffset_);
    PSOManager::SetBindInfo("Glitch.pip", "noiseStrength", &noiseStrength_);
    PSOManager::SetBindInfo("Glitch.pip", "lineSpeed", &lineSpeed_);

    // ディスパッチ
    Dispatch("Glitch.pip");
}

void Glitch::Release()
{
}

void Glitch::Edit() {
#ifdef _DEBUG

    static std::string label;
    label = "Glitch_" + std::to_string(id_);
    if (ImGui::CollapsingHeader(label.c_str())) {
        ImGui::Indent();
        {
            IPostProcess::Edit();

            ImGui::Text("currentTime: %.3f", time_);
            ImGui::DragFloat("intensity", &intensity_, 0.01f);
            ImGui::DragFloat("blockSize", &blockSize_, 0.01f);
            ImGui::DragFloat("colorOffset", &colorOffset_, 0.01f);
            ImGui::DragFloat("noiseStrength", &noiseStrength_, 0.01f);
            ImGui::DragFloat("lineSpeed", &lineSpeed_, 0.01f);
        }
        ImGui::Unindent();
    }
#endif // _DEBUG
}

nlohmann::json Glitch::ToJson() {

    nlohmann::json j;

    j["type"] = "Glitch";
    j["intensity_"] = intensity_;
    j["blockSize_"] = blockSize_;
    j["colorOffset_"] = colorOffset_;
    j["noiseStrength_"] = noiseStrength_;
    j["lineSpeed_"] = lineSpeed_;
    return j;
}

void Glitch::FromJson(const nlohmann::json& json) {

    intensity_ = json.value("intensity_", 1.0f);
    blockSize_ = json.value("blockSize_", 1.0f);
    colorOffset_ = json.value("colorOffset_", 1.0f);
    noiseStrength_ = json.value("noiseStrength_", 1.0f);
    lineSpeed_ = json.value("lineSpeed_", 1.0f);
}