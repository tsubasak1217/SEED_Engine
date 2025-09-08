#include "Bloom.h"

//============================================================================
//	include
//============================================================================
#include <SEED/Source/Manager/DxManager/PostEffect.h>
#include <Environment/Environment.h>
#include <SEED/Source/SEED.h>

//============================================================================
//	Bloom classMethods
//============================================================================

Bloom::Bloom() : IPostProcess() {
}

void Bloom::Initialize() {

    // パイプラインの作成
    PSOManager::CreatePipelines("Bloom.pip");
}

void Bloom::Update() {


}

void Bloom::Apply() {

    // バインド処理
    PSOManager::SetBindInfo("Bloom.pip", "inputTexture",
        ViewManager::GetHandleGPU(HEAP_TYPE::SRV_CBV_UAV, GetCurSRVBufferName()));
    PSOManager::SetBindInfo("Bloom.pip", "outputTexture",
        ViewManager::GetHandleGPU(HEAP_TYPE::SRV_CBV_UAV, GetCurUAVBufferName()));
    // CBuffer
    PSOManager::SetBindInfo("Bloom.pip", "threshold", &threshold_);
    PSOManager::SetBindInfo("Bloom.pip", "radius", &radius_);
    PSOManager::SetBindInfo("Bloom.pip", "sigma", &sigma_);

    // ディスパッチ
    Dispatch("Bloom.pip");
}

void Bloom::Release() {
}

void Bloom::Edit() {
#ifdef _DEBUG

    static std::string label;
    label = "Bloom_" + std::to_string(id_);
    if (ImGui::CollapsingHeader(label.c_str())) {
        ImGui::Indent();
        {
            IPostProcess::Edit();

            ImGui::DragFloat("threshold", &threshold_, 0.01f);
            ImGui::DragInt("radius", &radius_, 1, 0, 8);
            ImGui::DragFloat("sigma", &sigma_, 0.01f);
        }
        ImGui::Unindent();
    }
#endif // _DEBUG
}

nlohmann::json Bloom::ToJson() {

    nlohmann::json j;

    j["type"] = "Bloom";
    j["threshold_"] = threshold_;
    j["radius_"] = radius_;
    j["sigma_"] = sigma_;
    return j;
}

void Bloom::FromJson(const nlohmann::json& json) {

    threshold_ = json.value("threshold_", 1.0f);
    radius_ = json.value("radius_", 2);
    sigma_ = json.value("sigma_", 64.0f);
}