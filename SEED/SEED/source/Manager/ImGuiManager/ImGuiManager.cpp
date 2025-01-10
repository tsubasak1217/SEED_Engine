#include "ImGuiManager.h"
#include "WindowManager.h"
#include "SEED.h"

// シングルトンインスタンス
ImGuiManager* ImGuiManager::instance_ = nullptr;


ImGuiManager* ImGuiManager::GetInstance(){
    if(instance_ == nullptr){
        instance_ = new ImGuiManager();
    }
    return instance_;
}


void ImGuiManager::Initialize(){

    /*===========================================================================================*/
    /*                                        ImGuiの初期化                                       */
    /*===========================================================================================*/

    GetInstance();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(WindowManager::GetHWND(SEED::GetInstance()->windowTitle_));
    ImGui_ImplDX12_Init(
        DxManager::GetInstance()->device.Get(),
        DxManager::GetInstance()->swapChainDesc.BufferCount,
        DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        ViewManager::GetHeap(DESCRIPTOR_HEAP_TYPE::SRV_CBV_UAV),
        ViewManager::GetHeap(DESCRIPTOR_HEAP_TYPE::SRV_CBV_UAV)->GetCPUDescriptorHandleForHeapStart(),
        ViewManager::GetHeap(DESCRIPTOR_HEAP_TYPE::SRV_CBV_UAV)->GetGPUDescriptorHandleForHeapStart()
    );
}


void ImGuiManager::Finalize(){
    // ImGuiの終了処理
#ifdef _DEBUG
    // 後始末
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
#endif
}

void ImGuiManager::Draw(){
    // ImGuiの描画
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), DxManager::GetInstance()->commandList.Get());
}


void ImGuiManager::PreDraw(){
#ifdef _DEBUG
    // ImGuiフレーム開始
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // マウス座標の補正
    ImVec2 mousePos = ImGui::GetIO().MousePos;
    ImGui::GetIO().MousePos.x *= 1.0f / WindowManager::GetWindowScale(SEED::GetInstance()->windowTitle_).x;
    ImGui::GetIO().MousePos.y *= 1.0f / WindowManager::GetWindowScale(SEED::GetInstance()->windowTitle_).y;

#endif
}


void ImGuiManager::PostDraw(){
#ifdef _DEBUG
    // 描画前準備
    ImGui::Render();

    // 描画
    Draw();
#endif
}
