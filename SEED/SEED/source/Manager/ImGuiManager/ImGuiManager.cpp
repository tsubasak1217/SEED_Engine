#include <SEED/Source/Manager/ImGuiManager/ImGuiManager.h>
#include <SEED/Source/Manager/WindowManager/WindowManager.h>
#include <SEED/Source/SEED.h>
#include "ImGlyph.h"

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

    // ImGuiのフォント設定
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF(
        "./SEED/EngineResources/Fonts/M_PLUS_Rounded_1c/MPLUSRounded1c-Regular.ttf",
        18.0f,// フォントサイズ
        nullptr,// オプション
        glyphRangesJapanese // 日本語用グリフ範囲
    );
    io.Fonts->Build(); // フォント構築を明示的に実行

    ImGui_ImplDX12_InvalidateDeviceObjects();
    ImGui_ImplDX12_CreateDeviceObjects();

    // ウィンドウハンドルの取得
    HWND hwnd = WindowManager::GetHWND(SEED::GetInstance()->windowTitle_);
#ifdef _DEBUG
    hwnd = WindowManager::GetHWND(SEED::GetInstance()->systemWindowTitle_);
#endif // _DEBUG

    // directX用の初期化
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX12_Init(
        DxManager::GetInstance()->device.Get(),
        2,
        DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        ViewManager::GetHeap(HEAP_TYPE::SRV_CBV_UAV),
        ViewManager::GetHeap(HEAP_TYPE::SRV_CBV_UAV)->GetCPUDescriptorHandleForHeapStart(),
        ViewManager::GetHeap(HEAP_TYPE::SRV_CBV_UAV)->GetGPUDescriptorHandleForHeapStart()
    );


    size_t fontSize = io.Fonts->Fonts.size();
    fontSize;
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
    ImGuiIO& io = ImGui::GetIO();

    if(GetForegroundWindow() == WindowManager::GetHWND(SEED::systemWindowTitle_)) {
        // マウス位置を更新
        POINT mousePos;
        GetCursorPos(&mousePos);
        ScreenToClient(WindowManager::GetHWND(SEED::systemWindowTitle_), &mousePos);
        io.MousePos = ImVec2(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
    } else {
        // 非アクティブなウィンドウではマウス入力を無効化
        io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
    }

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
