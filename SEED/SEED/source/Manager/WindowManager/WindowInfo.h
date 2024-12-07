#pragma once
#include <windows.h>
#include "Vector2.h"

class WindowInfo{
public:
    WindowInfo() = default;
    WindowInfo(HWND windowHandle);
    void Update();

public:
    HWND GetWindowHandle()const{return windowHandle;}
    Vector2 GetOriginalWindowSize()const{ return originalWindowSize; }
    Vector2 GetCurrentWindowSize()const{ return currentWindowSize; }
    Vector2 GetWindowScale()const{ return windowScale; }

private:
    HWND windowHandle;
    Vector2 originalWindowSize;
    Vector2 currentWindowSize;
    Vector2 windowScale;
};