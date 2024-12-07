#include "WindowInfo.h"

WindowInfo::WindowInfo(HWND windowHandle){
    this->windowHandle = windowHandle;

    // ウインドウの初期サイズを取得
    RECT clientRect{};
    GetClientRect(windowHandle, &clientRect);
    originalWindowSize.x = (float)clientRect.right;
    originalWindowSize.y = (float)clientRect.bottom;

    currentWindowSize = originalWindowSize;
    windowScale = Vector2(1.0f, 1.0f);
}

void WindowInfo::Update(){

    // ウインドウのサイズスケールを更新
    RECT clientRect{};
    GetClientRect(windowHandle, &clientRect);
    currentWindowSize.x = (float)clientRect.right;
    currentWindowSize.y = (float)clientRect.bottom;

    windowScale.x = currentWindowSize.x / originalWindowSize.x;
    windowScale.y = currentWindowSize.y / originalWindowSize.y;
}
