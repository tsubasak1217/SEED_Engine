#include "IPostProcess.h"
// local
#include <SEED/Source/Manager/DxManager/DxManager.h>
#include <SEED/Source/Manager/DxManager/PostEffect.h>
#include <SEED/Source/SEED.h>
#include <SEED/Lib/Functions/MyFunc/MyMath.h>
#include <SEED/Lib/Structs/CS_Buffers.h>
#include <SEED/Source/Manager/DxManager/PSO/PSOManager.h>

// external
#include <cmath>

IPostProcess::IPostProcess(){
    // IDの設定
    id_ = nextId_++;
}

// 共通のdispatch関数
void IPostProcess::Dispatch(const std::string& pipelineName, int32_t gridX, int32_t gridY){

    // PSOの切り替え
    auto* pso = PSOManager::GetPSO_Compute(pipelineName);
    auto& commandList = DxManager::GetInstance()->commandList;
    commandList->SetComputeRootSignature(pso->rootSignature.get()->rootSignature.Get());
    commandList->SetPipelineState(pso->pipeline.get()->pipeline_.Get());

    // リソースのバインド
    pso->rootSignature->BindAll(commandList.Get(), true);

    // 画面をグリッドに分割して、縦横それぞれのGroup数を計算
    UINT dispatchX = (SEED::GetInstance()->kClientWidth_ + (gridX - 1)) / gridX;
    UINT dispatchY = (SEED::GetInstance()->kClientHeight_ + (gridY - 1)) / gridY;

    // 実行
    commandList->Dispatch(dispatchX, dispatchY, 1);

    // バッファインデックスを更新
    PostEffect::instance_->ChangeBufferIndex();

    // リソースの状態を更新
    PostEffect::instance_->StartTransition();
}

// 名前取得関数
std::string IPostProcess::GetCurUAVBufferName(){
    return PostEffect::GetCurUAVBufferName();
}

// 名前取得関数
std::string IPostProcess::GetCurSRVBufferName(){
    return PostEffect::GetCurSRVBufferName();
}

// 深度バッファ名取得関数
std::string IPostProcess::GetCurDepthBufferName(){
    return DxManager::instance_->depthStencilNames[SEED::GetMainCameraName()];
}


// ポストプロセスの編集用関数
void IPostProcess::Edit(){
#ifdef _DEBUG

    ImGui::Checkbox("有効", &isActive_);

#endif // _DEBUG
}