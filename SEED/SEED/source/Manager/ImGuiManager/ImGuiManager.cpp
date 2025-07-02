#include <SEED/Source/Manager/ImGuiManager/ImGuiManager.h>
#include <SEED/Source/Manager/WindowManager/WindowManager.h>
#include <SEED/Source/SEED.h>
#include "ImGlyph.h"
#include <DirectXMath.h>
#include <DirectXMathMatrix.inl>
using namespace DirectX;

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
        GlyphRanges::imJpGlyphRanges // 日本語用グリフ範囲
    );
    io.Fonts->Build(); // フォント構築を明示的に実行

    // ドッキング機能の有効化
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // スタイルの変更
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;
    // 白
    colors[ImGuiCol_MenuBarBg] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);// メニューバーの背景色
    // 黒に近い色に変更
    colors[ImGuiCol_WindowBg] = ImVec4(0.01f, 0.01f, 0.01f, 1.0f);// 背景を暗く
    // 深い青色に変更
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.0f, 0.02f, 0.1f, 1.0f);// 選択されたウィンドウのタイトルバー
    //colors[ImGuiCol_HeaderHovered] = ImVec4(0.0f, 0.02f, 0.1f, 1.0f);// ヘッダーのホバー時


    // ウィンドウハンドルの取得
    HWND hwnd = WindowManager::GetHWND(SEED::GetInstance()->windowTitle_);
    instance_->windowTitle_ = SEED::GetInstance()->windowTitle_;
#ifdef USE_SUB_WINDOW
    hwnd = WindowManager::GetHWND(SEED::GetInstance()->systemWindowTitle_);
    instance_->windowTitle_ = SEED::GetInstance()->systemWindowTitle_;
#endif // USE_SUB_WINDOW

    // directX用の初期化
    ImGui_ImplDX12_InvalidateDeviceObjects();
    ImGui_ImplDX12_CreateDeviceObjects();
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX12_Init(
        DxManager::GetInstance()->device.Get(),
        2,
        DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        ViewManager::GetHeap(HEAP_TYPE::SRV_CBV_UAV),
        ViewManager::GetHeap(HEAP_TYPE::SRV_CBV_UAV)->GetCPUDescriptorHandleForHeapStart(),
        ViewManager::GetHeap(HEAP_TYPE::SRV_CBV_UAV)->GetGPUDescriptorHandleForHeapStart()
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

    // guizmoのリストをクリア
    instance_->guizmoInfo3D_.clear();
    instance_->guizmoInfo2D_.clear();

    // ディスプレイサイズの設定
    ImGuiIO& io = ImGui::GetIO();
    Vector2 displaySize = WindowManager::GetCurrentWindowSize(instance_->windowTitle_);
    io.DisplaySize = ImVec2(displaySize.x, displaySize.y);
    static Vector2 preWinScale{};
    Vector2 winScale = WindowManager::GetWindowScale(instance_->windowTitle_);
    if(preWinScale != winScale){
        // ウィンドウのスケールが変わった場合、フォントサイズを更新
        io.FontGlobalScale = 1.0f / winScale.x; // x方向のスケールで補正
        preWinScale = winScale;
    }

    // マウス座標の補正
    if(GetForegroundWindow() == WindowManager::GetHWND(instance_->windowTitle_)){
        // マウス位置を更新
        POINT mousePos;
        GetCursorPos(&mousePos);
        ScreenToClient(WindowManager::GetHWND(SEED::systemWindowTitle_), &mousePos);
        io.AddMousePosEvent(float(mousePos.x) * (1.0f / winScale.x), float(mousePos.y) * (1.0f / winScale.y));

    } else{
        // 非アクティブなウィンドウではマウス入力を無効化
        io.AddMousePosEvent(-FLT_MAX, -FLT_MAX);
    }

    ImGui::NewFrame();
    ImGuizmo::BeginFrame();

#endif
}


void ImGuiManager::PostDraw(){
#ifdef _DEBUG

    ImFunc::CustomBegin("ImGui", MoveOnly_TitleBar);
    ImGui::Text("mouse position: (%.1f, %.1f)", ImGui::GetMousePos().x, ImGui::GetMousePos().y);

    // ImGuizmoの操作モードを切り替えるコンボボックス
    ImFunc::ComboPair("Guizmoの操作モード", instance_->currentOperation_,
        {
            {"Translate",ImGuizmo::TRANSLATE},
            {"Rotate",ImGuizmo::ROTATE},
            {"Scale",ImGuizmo::SCALE}
        }
    );
    ImGui::End();

    // デフォルトカメラ画面描画ウインドウ
    ImFunc::SceneWindowBegin("GameWindow", "default",MoveOnly_TitleBar);
    ImGui::End();

    // デバッグカメラ視点描画ウインドウ
    ImVec2 imageSize = ImFunc::SceneWindowBegin("DebugCameraWindow", "debug", MoveOnly_TitleBar);
    {
        // Guizmo
        ImDrawList* pDrawList = ImGui::GetWindowDrawList();
        ImVec2 imageLeftTop = ImGui::GetCursorScreenPos() - ImVec2(0.0f, imageSize.y);
        Range2D rectRange = {
            {imageLeftTop.x,imageLeftTop.y},
            {imageLeftTop.x + imageSize.x, imageLeftTop.y + imageSize.y}
        };

        // ImGuizmoの操作を行う
        for(auto& info : instance_->guizmoInfo3D_){
            ImFunc::Guizmo3D(info, pDrawList, rectRange);
        }

        // 2D ImGuizmoの操作を行う
        for(auto& info : instance_->guizmoInfo2D_){
            ImFunc::Guizmo2D(info, pDrawList, rectRange);
        }
    }
    ImGui::End();

    // 描画前準備
    ImGui::Render();

    // 描画
    Draw();
#endif
}


/////////////////////////////////////////////////////////////////
// カスタムウィンドウの開始関数
/////////////////////////////////////////////////////////////////
using WindowLocationData = std::pair<ImVec2, ImVec2>; // ウィンドウの位置とサイズ
void ImFunc::CustomBegin(const char* name, CustomImWindowFlag customFlag, ImGuiWindowFlags flags){

    static std::unordered_map<std::string, WindowLocationData> windowDatas;
    static std::unordered_map<std::string, bool> isDragging;
    const ImGuiIO& io = ImGui::GetIO();
    ImVec2 mousePos = io.MousePos;

    // タイトルバーのみドラッグ可能な場合
    if(customFlag & MoveOnly_TitleBar){
        flags |= ImGuiWindowFlags_NoMove;

        if(windowDatas.find(name) != windowDatas.end()){
            float titlebar_height = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2;
            ImGuiWindow* window = ImGui::FindWindowByName(name);
            ImVec2 window_pos = windowDatas[name].first;
            ImVec2 window_size = windowDatas[name].second;
            ImVec2 rectMin{};
            ImVec2 rectMax{};

            // ドッキング中はタブ範囲のみ(LCtrl押してるときは動かせる)
            if(window && window->DockNode && window->DockNode->TabBar){
                // タブバーが存在する場合 → 各タブを走査
                ImGuiTabBar* tabBar = window->DockNode->TabBar;
                for(int i = 0; i < tabBar->Tabs.Size; ++i){
                    const ImGuiTabItem& tab = tabBar->Tabs[i];
                    // タブが自分のものか確認
                    if(tab.Window == window){
                        rectMin = windowDatas[name].first + ImVec2(tab.Offset, 0.0f);
                        rectMax = ImVec2(rectMin.x + tab.Width, rectMin.y + titlebar_height);
                    }
                }
            } else{
                // 通常のウィンドウの場合
                rectMin = window_pos;
                rectMax = ImVec2(window_pos.x + window_size.x, window_pos.y + titlebar_height);
            }

            // 矩形内に入っていればドラッグ可能にする
            if(mousePos.x >= rectMin.x && mousePos.x <= rectMax.x &&
                mousePos.y >= rectMin.y && mousePos.y <= rectMax.y){
                if(ImGui::IsMouseClicked(ImGuiMouseButton_Left)){
                    isDragging[name] = true;
                }
            }

            // ドラッグ中なら継続して移動許可
            if(isDragging[name]){
                flags &= ~ImGuiWindowFlags_NoMove;
            }

            if(Input::IsPressKey(DIK_LCONTROL)){
                flags &= ~ImGuiWindowFlags_NoMove;
            }
        }
    }

    // ImGuiウィンドウを開始
    ImGui::Begin(name, nullptr, flags);

    // ウィンドウの位置とサイズを保存
    ImVec2 window_pos = ImGui::GetWindowPos();
    ImVec2 window_size = ImGui::GetWindowSize();
    windowDatas[name] = { window_pos, window_size };

    // ドラッグ解除
    if(ImGui::IsMouseReleased(ImGuiMouseButton_Left)){
        isDragging[name] = false;
    }
}

/////////////////////////////////////////////////////////////////
// シーン描画ウインドウ
/////////////////////////////////////////////////////////////////
ImVec2 ImFunc::SceneWindowBegin(const char* label, const std::string& cameraName, CustomImWindowFlag customFlags, ImGuiWindowFlags normalFlags){
    ImFunc::CustomBegin(label, customFlags,normalFlags);
    {
        // サイズの計算
        Vector2 displaySize = WindowManager::GetCurrentWindowSize(ImGuiManager::GetWindowName());
        ImVec2 availSize = ImGui::GetContentRegionAvail();
        ImVec2 imageSize;
        float ratioX[2] = {
        availSize.x / availSize.y,
        displaySize.x / displaySize.y
        };

        // 狭いほうに合わせる
        if(ratioX[0] > ratioX[1]){
            imageSize = { availSize.y * ratioX[1],availSize.y };
        } else{
            imageSize = { availSize.x,availSize.x * (displaySize.y / displaySize.x) };
        }

        ImGui::Image(TextureManager::GetImGuiTexture("offScreen_" + cameraName), imageSize);
        return imageSize; // 画像サイズを返す
    }
}


/////////////////////////////////////////////////////////////////
// stringのコンボボックスを作成
/////////////////////////////////////////////////////////////////
bool ImFunc::ComboText(const char* label, std::string& str, const std::vector<std::string>& items){
    int currentIndex = -1;
    int size = static_cast<int>(items.size()); // padding分を除外
    for(int i = 0; i < size; ++i){
        if(items[i] == str){
            currentIndex = i;
            break;
        }
    }
    // ImGui::Comboはconst char*の配列を受け取るので、std::vectorから変換
    std::vector<const char*> itemsCStr;
    itemsCStr.reserve(size);
    for(const auto& item : items){
        itemsCStr.push_back(item.c_str());
    }

    // Comboの実行
    bool changed = ImGui::Combo(label, &currentIndex, itemsCStr.data(), size);
    if(changed && currentIndex >= 0 && currentIndex < size){
        str = items[currentIndex];
    }
    return changed;
}


///////////////////////////////////////////////////////////////////
// inputTextに直接stringを渡せるように
///////////////////////////////////////////////////////////////////
bool ImFunc::InputTextMultiLine(const char* label, std::string& str){
    static std::array<char, 1024> buffer;
    std::fill(buffer.begin(), buffer.end(), '\0'); // バッファをクリア
    strncpy_s(buffer.data(), buffer.size(), str.c_str(), _TRUNCATE);

    bool changed = ImGui::InputTextMultiline(label, buffer.data(), buffer.size());
    if(changed){
        str = buffer.data();  // 更新
    }
    return changed;
}

bool ImFunc::InputText(const char* label, string& str){
    static std::array<char, 256> buffer;
    std::fill(buffer.begin(), buffer.end(), '\0'); // バッファをクリア
    strncpy_s(buffer.data(), buffer.size(), str.c_str(), _TRUNCATE);

    bool changed = ImGui::InputText(label, buffer.data(), buffer.size());
    if(changed){
        str = buffer.data();  // 更新
    }
    return changed;
}


/////////////////////////////////////////////////////////////////
// ImGuizmoで操作をしてトランスフォームに反映する関数
/////////////////////////////////////////////////////////////////
void ImFunc::Guizmo3D(const GuizmoInfo& info, ImDrawList* pDrawList, Range2D rectRange){

    if(!info.transform){
        return; // transformがnullptrの場合は何もしない
    }

    // 必要な行列の用意
    BaseCamera* camera = SEED::GetCamera("debug");
    Matrix4x4 viewMat = camera->GetViewMat();
    Matrix4x4 projMat = camera->GetProjectionMat();
    Matrix4x4 modelMat = info.transform->ToMatrix();
    Matrix4x4 worldMat = modelMat * info.parentMat;
    Matrix4x4 deltaMat = IdentityMat4(); // 変化量の行列

    // IDの設定(ポインタをIDとして仕様)
    uint32_t id = (uint32_t)reinterpret_cast<uintptr_t>(info.transform);
    ImGuizmo::SetID(id);

    // Rectの設定
    ImGuizmo::SetRect(
        rectRange.min.x, rectRange.min.y,
        rectRange.max.x - rectRange.min.x,
        rectRange.max.y - rectRange.min.y
    );

    // DrawListの設定
    ImGuizmo::SetDrawlist(pDrawList);

    // ImGuizmoの操作
    ImGuizmo::MODE mode = ImGuizmo::LOCAL; // ローカル座標系で操作
    bool isManipulated = ImGuizmo::Manipulate(
        viewMat.m[0], projMat.m[0], ImGuiManager::instance_->currentOperation_, mode,
        worldMat.m[0], deltaMat.m[0], nullptr, nullptr, nullptr
    );

    // 変化量をトランスフォームに適用
    if(isManipulated){
        Matrix4x4 invParentMat = InverseMatrix(info.parentMat);
        worldMat *= invParentMat; // 親の逆行列を掛けてローカル座標系に変換
        info.transform->FromMatrix(worldMat);
    }
}

// 2D用のImGuizmo操作関数
void ImFunc::Guizmo2D(const GuizmoInfo& info, ImDrawList* pDrawList, Range2D rectRange){

    if(!info.transform2D){
        return; // transformがnullptrの場合は何もしない
    }

    // 必要な行列の用意
    BaseCamera* camera = SEED::GetCamera("debug");
    Matrix4x4 viewMat = TranslateMatrix(Vector3(0.0f, 0.0f, camera->GetZNear() + 1.0f));
    Matrix4x4 projMat = camera->GetProjectionMat2D();
    Matrix4x4 modelMat = info.transform2D->ToMatrix4x4() * info.parentMat;
    Matrix4x4 deltaMat = IdentityMat4(); // 変化量の行列

    // IDの設定(ポインタをIDとして仕様)
    uint32_t id = (uint32_t)reinterpret_cast<uintptr_t>(info.transform2D);
    ImGuizmo::SetID(id);

    // Rectの設定
    ImGuizmo::SetRect(
        rectRange.min.x, rectRange.min.y,
        rectRange.max.x - rectRange.min.x,
        rectRange.max.y - rectRange.min.y
    );

    // DrawListの設定
    ImGuizmo::SetDrawlist(pDrawList);
    // ImGuizmoの操作
    ImGuizmo::MODE mode = ImGuizmo::LOCAL; // ローカル座標系で操作
    ImGuizmo::OPERATION operation = ImGuiManager::instance_->currentOperation_;
    if(operation == ImGuizmo::ROTATE){// 2Dでは回転はZ軸のみ
        operation = ImGuizmo::ROTATE_Z;
    }
    bool isManipulated = ImGuizmo::Manipulate(
        viewMat.m[0], projMat.m[0], operation, mode,
        modelMat.m[0], deltaMat.m[0], nullptr, nullptr, nullptr
    );

    // 変化量をトランスフォームに適用
    if(isManipulated){
        Matrix4x4 invParentMat = InverseMatrix(info.parentMat);
        modelMat *= invParentMat; // 親の逆行列を掛けてローカル座標系に変換
        info.transform2D->FromMatrix4x4(modelMat);
    }
}
