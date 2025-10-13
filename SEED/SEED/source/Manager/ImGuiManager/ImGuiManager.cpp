#include <SEED/Source/Manager/ImGuiManager/ImGuiManager.h>
#include <SEED/Source/Manager/WindowManager/WindowManager.h>
#include <SEED/Source/SEED.h>
#include "ImGlyph.h"
#include <DirectXMath.h>
#include <DirectXMathMatrix.inl>
#include <SEED/Source/Manager/PostEffectSystem/PostEffectSystem.h>
#include <SEED/Source/Manager/InputManager/InputManager.h>

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
    instance_->isInputNow_ = false;

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
    {
        ImGui::Text("mouse position: (%.1f, %.1f)", ImGui::GetMousePos().x, ImGui::GetMousePos().y);

        // ImGuizmoの操作モードを切り替えるコンボボックス
        ImFunc::ComboPair("Guizmoの操作モード", instance_->currentOperation_,
            {
                {"Translate",ImGuizmo::TRANSLATE},
                {"Rotate",ImGuizmo::ROTATE},
                {"Scale",ImGuizmo::SCALE}
            }
        );

        // エクスプローラーメニュー
        if(instance_->explolerMenuOpenOrder_){
            ImGui::OpenPopup(instance_->menuName_);
            instance_->explolerMenuOpenOrder_ = false;
        }
        if(ImGui::BeginPopup(instance_->menuName_)){
            instance_->ExprolerMenu();
            ImGui::EndPopup();
        }

    }
    ImGui::End();

    // デフォルトカメラ画面描画ウインドウ
    ImFunc::SceneWindowBegin("GameWindow");
    {
        ImGui::End();

        // デバッグカメラ視点描画ウインドウ
        ImVec2 imageSize = ImFunc::SceneWindowBegin("DebugCameraWindow", "debug", MoveOnly_TitleBar | SceneManipurate);
        {
            // Guizmo
            ImDrawList* pDrawList = ImGui::GetWindowDrawList();
            ImVec2 imageLeftTop = ImGui::GetItemRectMin();

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
    }
    ImGui::End();


    // 描画前準備
    ImGui::Render();

    // 描画
    Draw();
#endif
}


//////////////////////////////////////////////////////////////////
// エクスプローラーメニュー表示
//////////////////////////////////////////////////////////////////
void ImGuiManager::OpenExplorerMenu(const std::string& itemPath, const std::string& menuName){
    instance_->explorerItemPath_ = itemPath;
    instance_->explolerMenuOpenOrder_ = true;
    instance_->menuName_ = menuName;
}

void ImGuiManager::ExprolerMenu(){

    // explorerItemPathが空なら何もしない
    if(explorerItemPath_.empty()){
        ImGui::EndPopup();
        return;
    }


    if(menuName_ == "ファイル/フォルダ"){
        // 指定したパスをエクスプローラーで開く
        if(ImGui::MenuItem("エクスプローラーで開く")){
            MyFunc::OpenInExplorer(explorerItemPath_);
        }

        // 削除
        if(ImGui::MenuItem("削除")){
            MyFunc::DeleteFileObject(explorerItemPath_);
        }
    
    }else if(menuName_ == "空選択"){
        // 新規作成
        if(ImGui::MenuItem("フォルダを作成")){
            MyFunc::CreateNewFolder(explorerItemPath_, "新規フォルダ");
        }
    }
}


/////////////////////////////////////////////////////////////////
// カスタムウィンドウの開始関数
/////////////////////////////////////////////////////////////////
using WindowLocationData = std::pair<ImVec2, ImVec2>; // ウィンドウの位置とサイズ
bool ImFunc::CustomBegin(const std::string& name, CustomWindowFlag customFlag, ImGuiWindowFlags flags){

    static std::unordered_map<std::string, WindowLocationData> windowDatas;
    static std::unordered_map<std::string, bool> isDragging;
    const ImGuiIO& io = ImGui::GetIO();
    ImVec2 mousePos = io.MousePos;

    // タイトルバーのみドラッグ可能な場合
    if(customFlag & MoveOnly_TitleBar){
        flags |= ImGuiWindowFlags_NoMove;

        if(windowDatas.find(name) != windowDatas.end()){
            float titlebar_height = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2;
            ImGuiWindow* window = ImGui::FindWindowByName(name.c_str());
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
    bool isDisplayed = ImGui::Begin(name.c_str(), nullptr, flags);

    // ウィンドウの位置とサイズを保存
    ImVec2 window_pos = ImGui::GetWindowPos();
    ImVec2 window_size = ImGui::GetWindowSize();
    windowDatas[name] = { window_pos, window_size };

    // ドラッグ解除
    if(ImGui::IsMouseReleased(ImGuiMouseButton_Left)){
        isDragging[name] = false;
    }

    return isDisplayed; // ウィンドウが表示されているかどうかを返す
}

/////////////////////////////////////////////////////////////////
// シーン描画ウインドウ
/////////////////////////////////////////////////////////////////
ImVec2 ImFunc::SceneWindowBegin(const std::string& label, const std::string& cameraName, CustomWindowFlag customFlags, ImGuiWindowFlags normalFlags){
    ImFunc::CustomBegin(label, customFlags, normalFlags);
    {
        // static変数
        static unordered_map<ImGuiID, float> scales; // スケールを保存するマップ

        // 画面内でホイールを回してスケールを変更
        ImGuiID id = ImGui::GetCurrentWindow()->ID;
        float wheel = ImGui::GetIO().MouseWheel;

        // シーン操作が有効な場合に操作
        if(customFlags & SceneManipurate){
            if(wheel != 0.0f){
                // window範囲内にマウスがあるかチェック
                ImVec2 mousePos = ImGui::GetMousePos();
                ImVec2 windowPos = ImGui::GetWindowPos();
                ImVec2 windowSize = ImGui::GetWindowSize();
                bool isMouseInWindow = (mousePos.x >= windowPos.x && mousePos.x <= windowPos.x + windowSize.x &&
                    mousePos.y >= windowPos.y && mousePos.y <= windowPos.y + windowSize.y);

                if(isMouseInWindow){
                    // マウスがウィンドウ内にある場合のみスケールを変更
                    if(scales.find(id) == scales.end()){
                        scales[id] = 1.0f; // 初期スケール
                    }
                    scales[id] += wheel * 0.05f; // ホイールの値でスケールを調整
                    scales[id] = (std::max)(0.1f, scales[id]); // 最小スケールを0.1に制限
                }
            }
        }


        // サイズの計算
        float scale = scales.find(id) != scales.end() ? scales[id] : 1.0f;
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

        // スケールを適用した最終サイズ
        ImVec2 finalSize = imageSize * scale;
        ImVec2 windowCenter = ImGui::GetCursorPos() + availSize * 0.5f; // ウィンドウの中央位置を計算
        ImVec2 offset = finalSize * 0.5f;
        ImGui::SetCursorPos(windowCenter - offset); // ウィンドウの中央に配置

        // 表示
        if(cameraName == ""){
            if((int)PostEffectSystem::GetInstance()->postProcessGroups_.size() != 0){
                ImGui::Image(TextureManager::GetImGuiTexture("postEffectResult"), finalSize);
            } else{
                ImGui::Image(TextureManager::GetImGuiTexture("offScreen_" + SEED::GetMainCameraName()), finalSize);
            }
        } else{
            ImGui::Image(TextureManager::GetImGuiTexture("offScreen_" + cameraName), finalSize);
        }

        sceneWindowRanges_[label] = {
            {ImGui::GetItemRectMin().x,ImGui::GetItemRectMin().y},
            {ImGui::GetItemRectMin().x + finalSize.x,ImGui::GetItemRectMin().y + finalSize.y}
        };
        return finalSize; // 画像サイズを返す
    }
}


/////////////////////////////////////////////////////////////////
// フォルダビューの開始関数
/////////////////////////////////////////////////////////////////
std::string ImFunc::FolderView(
    const std::string& label,
    std::filesystem::path& currentPath,
    bool isFileNameOnly,
    std::initializer_list<std::string> filterExts,
    std::filesystem::path rootPath,
    bool returnDirectoryName
){
    static bool isLoaded = false;
    static ImTextureID folderIcon;
    static ImTextureID fileIcon;
    static float rowStartX = 0.0f;
    static ImVec2 iconSize = { 64.0f, 64.0f };
    static unordered_map<std::string, std::string> focusedItems;
    static std::string edittingName = "";
    static bool isEditName = false;
    static int frameCount = 0;
    std::string selectedFile = "";
    bool isClickAnyItem = false;
    ImRect folderViewRect = ImRect();

    if(ImGui::CollapsingHeader(label.c_str())){

        // 一度のみ画像読み込み
        if(!isLoaded){
            folderIcon = TextureManager::GetImGuiTexture("../../SEED/EngineResources/Textures/folderIcon.png");
            fileIcon = TextureManager::GetImGuiTexture("../../SEED/EngineResources/Textures/fileIcon.png");
            isLoaded = true;
        }

        // folderViewのmin座標を保存
        folderViewRect.Min = ImGui::GetCursorScreenPos();

        // ディレクトリ表示
        ImGui::Text("Current Path: %s", currentPath.string().c_str());
        if(currentPath.has_parent_path()){
            // rootPathより上の階層にいるときのみBackボタンを表示
            if(rootPath.empty() || currentPath != rootPath){
                if(ImGui::Button("<< Back")){
                    currentPath = currentPath.parent_path();
                    return "";
                }
            }
        }

        // 表示対象のファイル・フォルダを収集
        std::vector<std::filesystem::directory_entry> entries;
        for(const auto& entry : std::filesystem::directory_iterator(currentPath)){
            if(entry.is_directory()){
                entries.push_back(entry); // ディレクトリは常に表示
            } else{
                std::string ext = entry.path().extension().string();
                std::transform(ext.begin(), ext.end(), ext.begin(),
                    [](unsigned char c) -> char{ return static_cast<char>(std::tolower(c)); });

                if(filterExts.size() == 1 && *filterExts.begin() == ""){
                    entries.push_back(entry); // フィルターが空なら全てのファイルを表示
                } else{
                    for(auto filter : filterExts){
                        if(!filter.starts_with(".")){
                            filter = "." + filter;
                        }
                        if(ext == filter){
                            entries.push_back(entry);
                            break;
                        }
                    }
                }
            }
        }

        // 項目表示に必要な変数
        float padding = 16.0f;
        float cellWidth = iconSize.x + padding;
        ImVec2 availRegion = ImGui::GetContentRegionAvail();
        int columns = (std::max)(1, (int)(availRegion.x / cellWidth));
        int currentColumn = 0;

        // 各アイテムの表示
        for(size_t i = 0; i < entries.size(); ++i){
            const auto& entry = entries[i];
            bool focused = false;

            if(currentColumn == 0){
                rowStartX = ImGui::GetCursorPosX();
            } else{
                ImGui::SameLine(rowStartX + currentColumn * cellWidth);
            }

            ImGui::BeginGroup();

            // アイコンの表示
            ImTextureID icon = entry.is_directory() ? folderIcon : fileIcon;
            std::string name = MyFunc::ToString(entry.path().filename().u8string());
            std::string id = "##" + entry.path().string();
            ImVec2 iconPos = ImGui::GetCursorScreenPos();
            ImGui::Image(icon, iconSize);

            // 各種入力の取得
            bool isHovered = ImGui::IsItemHovered();
            bool isLeftDoubleClicked = isHovered && ImGui::IsMouseDoubleClicked(0);
            bool isLeftClicked = isHovered && ImGui::IsMouseClicked(0) && !isLeftDoubleClicked;
            bool isRightClicked = isHovered && ImGui::IsMouseClicked(1);
            isClickAnyItem |= isLeftClicked || isLeftDoubleClicked || isRightClicked;

            // ドラッグ＆ドロップの開始
            std::string fullPathStr = MyFunc::ToString(entry.path().u8string());
            ImFunc::BeginDrag("FILE_PATH", fullPathStr, name.c_str(), ImGuiDragDropFlags_SourceAllowNullID);

            // フォーカスされている場合は枠を表示
            if(focusedItems.find(label) != focusedItems.end() && focusedItems[label] == id){
                focused = true;
                ImGui::GetWindowDrawList()->AddRect(
                    iconPos,
                    ImVec2(iconPos.x + iconSize.x, iconPos.y + iconSize.y),
                    IM_COL32(255, 255, 255, 128), // 黄色の枠
                    0.0f, 0, 3.0f // 線の太さ
                );


            }

            // フォーカスの設定
            if(isLeftClicked){
                // フォーカス済みのものを押していたら名前を編集するようにする
                if(focusedItems[label] == id){
                    isEditName = true;
                    edittingName = MyFunc::ToString(entry.path().filename().u8string());
                    // 拡張子を除去してファイル名のみ編集するように
                    if(!entry.is_directory() && edittingName.find(".") != std::string::npos){
                        edittingName = edittingName.substr(0, edittingName.find_last_of('.'));
                    }

                    // 次のテキスト入力にフォーカスを当てる
                    ImGui::SetKeyboardFocusHere();
                    frameCount = 0;

                } else{// まだフォーカスしていなければフォーカス
                    focusedItems[label] = id;
                    isEditName = false;
                }
            }


            // 中央揃えでテキスト表示
            if(!(isEditName && focusedItems[label] == id)){

                auto lines = WrapTextLines(name, iconSize.x, 2);
                for(const auto& l : lines){
                    float textWidth = ImGui::CalcTextSize(l.c_str()).x;
                    float iconCenterX = iconSize.x * 0.5f;
                    float textPosX = iconPos.x + iconCenterX - textWidth * 0.5f;

                    float minX = ImGui::GetWindowPos().x;
                    float maxX = minX + ImGui::GetWindowContentRegionMax().x;
                    textPosX = std::clamp(textPosX, minX, maxX - textWidth);

                    ImGui::SetCursorScreenPos(ImVec2(textPosX, ImGui::GetCursorScreenPos().y));
                    ImGui::TextUnformatted(l.c_str());
                }

            } else{
                // 名前編集モード
                ImVec2 boxSize = { iconSize.x, ImGui::GetTextLineHeight() * 2 };
                ImFunc::InputTextMultiLine(id, edittingName, boxSize);

                // インプットが有効になったらテキスト入力を終えているので確定
                if(Input::GetIsActive()){
                    if(frameCount > 0){
                        isEditName = false;

                        // ファイル名を変更
                        std::string newName = edittingName;
                        if(!entry.is_directory()){
                            // 拡張子を復元
                            if(entry.path().filename().string().find(".") != std::string::npos){
                                newName += entry.path().extension().string();
                            }
                        }

                        // 変更
                        std::string newPath = MyFunc::ToString(entry.path().parent_path().u8string()) + "/" + newName;
                        MyFunc::RenameFile(MyFunc::ToString(entry.path().u8string()), newPath);
                    }
                }

                // フレーム数をカウント(フォーカス設定用)
                frameCount++;

            }

            // ダブルクリックでディレクトリ移動、ファイル名格納
            if(isLeftDoubleClicked){
                if(entry.is_directory()){
                    currentPath = entry.path();
                    ImGui::EndGroup();
                    return "";
                } else{
                    selectedFile = isFileNameOnly
                        ? entry.path().filename().string()
                        : entry.path().string();

                    // フォルダ名を返す設定なら親フォルダを返す
                    if(returnDirectoryName){
                        currentPath = entry.path().parent_path();
                    }
                }
            }

            // コンテキストメニュー用のアイテムのパスを取得
            if(isRightClicked){
                ImGuiManager::OpenExplorerMenu(MyFunc::ToString(entry.path().u8string()), "ファイル/フォルダ");
            }

            ImGui::EndGroup();

            currentColumn++;
            if(currentColumn >= columns){
                currentColumn = 0;
            }
        }

        // folderViewのmax座標を保存
        float curWindowRight = ImGui::GetWindowPos().x + ImGui::GetWindowSize().x;
        folderViewRect.Max = ImGui::GetCursorScreenPos();
        folderViewRect.Max.x = curWindowRight;

    } else{
        // 閉じたらフォーカス情報をクリア
        focusedItems.erase(label);
    }

    if(ImGui::IsMouseClicked(1)){
        if(ImGuiManager::GetInstance()->explorerItemPath_.empty()){
            ImGuiManager::GetInstance()->explorerItemPath_ = currentPath.string();
        }
    }

    ImGui::Separator();

    // FolderViewの範囲内かつ、何もアイテムをクリックしていないときに右クリックしたかをチェック
    if(folderViewRect.Contains(ImGui::GetIO().MousePos) && !isClickAnyItem){
        if(ImGui::IsMouseClicked(1) && !isClickAnyItem){
            // フォーカス情報をクリア
            focusedItems.erase(label);
            // 空選択メニューを開く
            ImGuiManager::OpenExplorerMenu(MyFunc::ToString(currentPath.u8string()),"空選択");
        }
    }

    return selectedFile;
}


/////////////////////////////////////////////////////////////////
// テキストを2行以内で切り、省略(...)付きで返す
/////////////////////////////////////////////////////////////////
std::vector<std::string> ImFunc::WrapTextLines(const std::string& text, float maxWidth, int maxLines){
    std::vector<std::string> lines;
    std::string line;

    for(size_t i = 0; i < text.size(); ){
        std::string ch;
        unsigned char c = text[i];
        if((c & 0x80) == 0x00){ ch = text.substr(i, 1); i += 1; }      // ASCII
        else if((c & 0xE0) == 0xC0){ ch = text.substr(i, 2); i += 2; } // 2-byte UTF-8
        else if((c & 0xF0) == 0xE0){ ch = text.substr(i, 3); i += 3; } // 3-byte UTF-8
        else if((c & 0xF8) == 0xF0){ ch = text.substr(i, 4); i += 4; } // 4-byte UTF-8
        else{ ch = "?"; i += 1; }

        float width = ImGui::CalcTextSize((line + ch).c_str()).x;
        if(width > maxWidth && !line.empty()){
            lines.push_back(line);
            line.clear();
            if((int)lines.size() >= maxLines) break;
        }

        line += ch;
    }

    if((int)lines.size() < maxLines && !line.empty()){
        lines.push_back(line);
    }

    // 2行目があるかつ、**幅オーバーしているときだけ** 省略する
    if((int)lines.size() == maxLines){
        float lastLineWidth = ImGui::CalcTextSize(lines.back().c_str()).x;
        if(lastLineWidth > maxWidth){
            while(!lines.back().empty()){
                std::string test = lines.back() + "...";
                if(ImGui::CalcTextSize(test.c_str()).x <= maxWidth){
                    lines.back() = test;
                    break;
                }
                lines.back().pop_back();
            }
        }
    }

    return lines;
}

/////////////////////////////////////////////////////////////////
// stringのコンボボックスを作成
/////////////////////////////////////////////////////////////////
bool ImFunc::ComboText(const std::string& label, std::string& str, const std::vector<std::string>& items){
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
    bool changed = ImGui::Combo(label.c_str(), &currentIndex, itemsCStr.data(), size);
    if(changed && currentIndex >= 0 && currentIndex < size){
        str = items[currentIndex];
    }
    return changed;
}


///////////////////////////////////////////////////////////////////
// inputTextに直接stringを渡せるように
///////////////////////////////////////////////////////////////////
bool ImFunc::InputTextMultiLine(const std::string& label, std::string& str, const ImVec2& size){
    static std::array<char, 1024> buffer;
    std::fill(buffer.begin(), buffer.end(), '\0'); // バッファをクリア
    strncpy_s(buffer.data(), buffer.size(), str.c_str(), _TRUNCATE);

    bool changed = ImGui::InputTextMultiline(label.c_str(), buffer.data(), buffer.size(), size, ImGuiInputTextFlags_CtrlEnterForNewLine);
    if(changed){
        str = buffer.data();  // 更新
    }

    // ImGuiの入力中は他の入力を受け付けないようにする
    if(ImGui::IsItemActive()){

        ImGuiManager::SetIsInputNow(true);
        Input::SetIsActive(false);
    } else{
        if(!ImGuiManager::GetIsInputNow()){

            Input::SetIsActive(true);
        }
    }

    return changed;
}

bool ImFunc::InputText(const std::string& label, string& str){
    static std::array<char, 256> buffer;
    std::fill(buffer.begin(), buffer.end(), '\0'); // バッファをクリア
    strncpy_s(buffer.data(), buffer.size(), str.c_str(), _TRUNCATE);

    bool changed = ImGui::InputText(label.c_str(), buffer.data(), buffer.size());
    if(changed){
        str = buffer.data();  // 更新
    }

    // ImGuiの入力中は他の入力を受け付けないようにする
    if(ImGui::IsItemActive()){

        ImGuiManager::SetIsInputNow(true);
        Input::SetIsActive(false);
    } else{
        if(!ImGuiManager::GetIsInputNow()){

            Input::SetIsActive(true);
        }
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
        Transform result;
        result.FromMatrix(worldMat);

        // 操作モードに応じて適切に反映
        if(ImGuiManager::instance_->currentOperation_ == ImGuizmo::SCALE){
            info.transform->scale = result.scale;

        } else if(ImGuiManager::instance_->currentOperation_ == ImGuizmo::TRANSLATE){
            info.transform->translate = result.translate;

        } else if(ImGuiManager::instance_->currentOperation_ == ImGuizmo::ROTATE){
            info.transform->rotate = result.rotate;

        } else{
            *info.transform = result;
        }
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
        Transform2D result;
        result.FromMatrix4x4(modelMat);

        // 操作モードに応じて適切に反映
        if(ImGuiManager::instance_->currentOperation_ == ImGuizmo::SCALE){
            info.transform2D->scale = result.scale;

        } else if(ImGuiManager::instance_->currentOperation_ == ImGuizmo::TRANSLATE){
            info.transform2D->translate = result.translate;

        } else if(ImGuiManager::instance_->currentOperation_ == ImGuizmo::ROTATE){
            info.transform2D->rotate = result.rotate;

        } else{
            *info.transform2D = result;
        }
    }
}

// シーンウィンドウの描画範囲を取得
const Range2D& ImFunc::GetSceneWindowRange(const std::string& label){
    if(sceneWindowRanges_.find(std::string(label)) != sceneWindowRanges_.end()){
        return sceneWindowRanges_[std::string(label)];
    }

    sceneWindowRanges_["emptyRange"] = Range2D{};
    return sceneWindowRanges_["emptyRange"];
}
