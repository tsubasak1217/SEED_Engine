#pragma once
#include "Vector2.h"
#include "WindowInfo.h"
#include <cassert>
#include <unordered_map>
#include <string>

class SEED;

class WindowManager{
private:
    // プライベートコンストラクタ
    WindowManager() = default;
    // コピー禁止
    WindowManager(const WindowManager&) = delete;
    WindowManager& operator=(const WindowManager&) = delete;
    // シングルトン
    static WindowManager* instance_;


public:// 基本的な関数
    static WindowManager* GetInstance();
    ~WindowManager() = default;
    static void Initialize(HINSTANCE hInstance, int nCmdShow);
    static void Create(const std::wstring& windowName,int32_t width,int32_t height);
    static void Finalize();
    static void Update();

public:
    static int ProcessMessage();
    static int ProcessMessage(const HWND& hWnd);

private:// 内部の関数
    void InitializeGDI();

public:// アクセッサ

    static MSG GetMSG(){
        return instance_->msg;
    }

    static HINSTANCE GetHInstance(){
        return instance_->hInstance_;
    }

    const WindowInfo& GetWindowInfo(const std::wstring& windowName){
        if(windowInfomations_.find(windowName) == windowInfomations_.end()){assert(0);}
        return windowInfomations_[windowName];
    };

    static HWND GetHWND(const std::wstring& windowName){
        return instance_->GetWindowInfo(windowName).GetWindowHandle();
    }

    static Vector2 GetCurrentWindowSize(const std::wstring& windowName){
        return instance_->GetWindowInfo(windowName).GetCurrentWindowSize();
    }

    static Vector2 GetOriginalWindowSize(const std::wstring& windowName){
        return instance_->GetWindowInfo(windowName).GetOriginalWindowSize();
    }

    static Vector2 GetWindowScale(const std::wstring& windowName){
        return instance_->GetWindowInfo(windowName).GetWindowScale();
    }

private:
    HINSTANCE hInstance_;
    int nCmdShow_;
    MSG msg;

    std::unordered_map<std::wstring, WindowInfo>windowInfomations_;
};