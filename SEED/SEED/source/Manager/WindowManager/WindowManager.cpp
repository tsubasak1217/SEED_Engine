#include <SEED/Source/Manager/WindowManager/WindowManager.h>
#include <SEED/Source/SEED.h>
#include <SEED/Source/Manager/DxManager/DxManager.h>

///////////////////////////////////////////////////////////////////////////////////
//                                static変数の初期化
///////////////////////////////////////////////////////////////////////////////////
WindowManager* WindowManager::instance_ = nullptr;


///////////////////////////////////////////////////////////////////////////////////
//                       ウィンドウ作成時のコールバック関数など
///////////////////////////////////////////////////////////////////////////////////

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
    HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam
);

LRESULT CALLBACK WindowProc(
    HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam
);


///////////////////////////////////////////////////////////////////////////////////
//                                初期化と終了処理
///////////////////////////////////////////////////////////////////////////////////

WindowManager* WindowManager::GetInstance(){
    if(!instance_){
        instance_ = new WindowManager();
        // GDIの初期化
        instance_->InitializeGDI();
    }

    return instance_;
}

WindowManager::~WindowManager(){
    Finalize();
    delete instance_;
    instance_ = nullptr;
}

void WindowManager::Initialize(HINSTANCE hInstance, int nCmdShow){
    GetInstance();
    instance_->hInstance_ = hInstance;
    instance_->nCmdShow_ = nCmdShow;
}


void WindowManager::Finalize(){
    for(auto& windowInfo : instance_->windowList_){
        windowInfo.second->Finalize();
    }
}

void WindowManager::InitializeGDI(){

    // GDI+の初期化
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
}

int WindowManager::ProcessMessage(){
    return instance_->msg.message;
}



///////////////////////////////////////////////////////////////////////////////////
//                             ウィンドウを新しく作成
///////////////////////////////////////////////////////////////////////////////////

void WindowManager::Create(const std::wstring& windowName, int32_t width, int32_t height, HWND parentHandle){


    /*--------------------- クライアント領域の決定 -----------------*/

    RECT rect = { 0,0,width,height };
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);

    /*-------------------- ウインドウクラスの登録 -------------------*/

    WNDCLASS wc{};// 空のウインドウクラス
    int32_t windowCount = (int32_t)instance_->windowList_.size();
    std::wstring szAppName = windowName + std::to_wstring(windowCount);// ウィンドウクラスの名前

    // ウインドウクラスの中身を記述
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = instance_->hInstance_;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = szAppName.c_str();

    if(!RegisterClass(&wc)){ return; }

    /*-------------------- ウィンドウの生成 -------------------------*/

    instance_->windowList_[windowName] = std::make_unique<WindowInfo>(CreateWindow(
        szAppName.c_str(),
        windowName.c_str(),
        WS_OVERLAPPEDWINDOW,
        //ウィンドウ左上座標
        CW_USEDEFAULT, CW_USEDEFAULT,
        //ウィンドウ幅と高さ
        rect.right - rect.left,
        rect.bottom - rect.top,
        parentHandle,
        nullptr,
        wc.hInstance,
        nullptr
    ), windowName
    );

    // ウインドウハンドルが取得できなかった場合アサート
    if(!instance_->windowList_[windowName]->GetWindowHandle()){ assert(0); }


    // ウインドウの表示
    ShowWindow(instance_->windowList_[windowName]->GetWindowHandle(), instance_->nCmdShow_);

    // ウインドウの更新
    UpdateWindow(instance_->windowList_[windowName]->GetWindowHandle());
}



///////////////////////////////////////////////////////////////////////////////////
//                                    更新処理
///////////////////////////////////////////////////////////////////////////////////

// ウインドウ関連の更新
void WindowManager::Update(){

    // メッセージ処理
    while(PeekMessage(&instance_->msg, NULL, 0, 0, PM_REMOVE)){
        TranslateMessage(&instance_->msg);
        DispatchMessage(&instance_->msg);
    }

    // ウインドウ情報の更新
    for(auto& windowInfo : instance_->windowList_){
        windowInfo.second->Update();
    }
}


///////////////////////////////////////////////////////////////////////////////////
//                                    コールバック関数
///////////////////////////////////////////////////////////////////////////////////

LRESULT CALLBACK WindowProc(
    HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){

    if(ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam)){
        return true;
    }

    switch(uMsg){

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    default:

        if(uMsg == WM_NCLBUTTONDOWN){
            ClockManager::GetInstance()->Stop();
        }/*else{
            ClockManager::GetInstance()->Restart();
        }*/

        // 標準のメッセージ処理を行う
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

}