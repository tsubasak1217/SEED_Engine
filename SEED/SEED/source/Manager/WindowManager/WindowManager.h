#pragma once
#include <SEED/Lib/Tensor/Vector2.h>
#include "WindowInfo.h"
#include <cassert>
#include <unordered_map>
#include <string>
#include <d3dx12.h>
#include <d3d12.h>

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
    ~WindowManager();
    static void Initialize(HINSTANCE hInstance, int nCmdShow);
    static void Create(const std::wstring& windowName,int32_t width,int32_t height, HWND parentHandle = nullptr);
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

    static WindowInfo* GetWindow(const std::wstring& windowName){
        if(instance_->windowList_.find(windowName) == instance_->windowList_.end()){assert(0);}
        return instance_->windowList_[windowName].get();
    };

    static HWND GetHWND(const std::wstring& windowName){
        return instance_->GetWindow(windowName)->GetWindowHandle();
    }

    static std::vector<HWND> GetAllHWNDs(){
        std::vector<HWND> hwnds;
        for(const auto& windowInfo : instance_->windowList_){
            hwnds.push_back(windowInfo.second->GetWindowHandle());
        }
        return hwnds;
    }

    static Vector2 GetCurrentWindowSize(const std::wstring& windowName){
        return instance_->GetWindow(windowName)->GetCurrentWindowSize();
    }

    static Vector2 GetOriginalWindowSize(const std::wstring& windowName){
        return instance_->GetWindow(windowName)->GetOriginalWindowSize();
    }

    static Vector2 GetWindowScale(const std::wstring& windowName){
        return instance_->GetWindow(windowName)->GetWindowScale();
    }

    static ID3D12Resource* GetBackBuffer(const std::wstring& windowName){
        return instance_->GetWindow(windowName)->GetBackBuffer();
    }

    static const D3D12_CPU_DESCRIPTOR_HANDLE& GetRtvHandle(const std::wstring& windowName){
        return instance_->GetWindow(windowName)->GetRtvHandle();
    }

    static void Present(const std::wstring& windowName, UINT syncInterval, UINT flags){
        instance_->GetWindow(windowName)->Present(syncInterval, flags);
    }

private:
    HINSTANCE hInstance_;
    int nCmdShow_;
    MSG msg;

    std::unordered_map<std::wstring, std::unique_ptr<WindowInfo>>windowList_;
};