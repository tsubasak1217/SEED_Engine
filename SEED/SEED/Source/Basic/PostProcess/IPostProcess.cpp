#include "IPostProcess.h"
// local
#include <SEED/Source/Manager/DxManager/DxManager.h>
#include <SEED/Source/Manager/PostEffectSystem/PostEffectSystem.h>
#include <SEED/Source/SEED.h>
#include <SEED/Lib/Functions/MyMath.h>
#include <SEED/Lib/Structs/CS_Buffers.h>
#include <SEED/Source/Manager/DxManager/PSO/PSOManager.h>

// external
#include <cmath>

IPostProcess::IPostProcess(){
    // IDの設定
    id_ = nextId_++;
}

// 共通のdispatch関数
void IPostProcess::Dispatch(
    const std::string& pipelineName, 
    int32_t gridX, int32_t gridY, 
    int32_t texWidth, int32_t texHeight,
    bool isSwapBuffer
){

    // PSOの切り替え
    auto* pso = PSOManager::GetPSO_Compute(pipelineName);
    auto& commandList = DxManager::GetInstance()->commandList;
    commandList->SetComputeRootSignature(pso->rootSignature.get()->rootSignature.Get());
    commandList->SetPipelineState(pso->pipeline.get()->pipeline_.Get());

    // リソースのバインド
    pso->rootSignature->BindAll(commandList.Get(), true);

    // 画面をグリッドに分割して、縦横それぞれのGroup数を計算
    UINT dispatchX = (texWidth + (gridX - 1)) / gridX;
    UINT dispatchY = (texHeight + (gridY - 1)) / gridY;

    // 実行
    commandList->Dispatch(dispatchX, dispatchY, 1);

    if(isSwapBuffer){
        // バッファインデックスを更新
        PostEffectSystem::instance_->ChangeBufferIndex();

        // リソースの状態を更新
        PostEffectSystem::instance_->StartTransition();
    }
}

// 名前取得関数
std::string IPostProcess::GetCurUAVBufferName(){
    return PostEffectSystem::GetCurUAVBufferName();
}

// 名前取得関数
std::string IPostProcess::GetCurSRVBufferName(){
    return PostEffectSystem::GetCurSRVBufferName();
}

// 深度バッファ名取得関数
std::string IPostProcess::GetCurDepthBufferName(){
    return DxManager::instance_->depthStencilNames[SEED::GetMainCameraName()];
}


// ポストプロセスの編集用関数
void IPostProcess::Edit(){
#ifdef _DEBUG

    static std::string label;
    
    label = "有効##" + std::to_string(id_);
    ImGui::Checkbox(label.c_str(), &isActive_);

#endif // _DEBUG
}