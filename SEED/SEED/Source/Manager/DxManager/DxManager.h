#pragma once

// lib
#include <windows.h>
#include <gdiplus.h>
#include <cstdint>
#include <memory>
#include <unordered_map>

#pragma comment (lib,"gdiplus.lib")

// functions
#include <SEED/Lib/Functions/MyFunc/DxFunc.h>
#include <SEED/Lib/Functions/MyFunc/MatrixFunc.h>
// PSO
#include <SEED/Source/Manager/DxManager/PSO/Pipeline.h>
#include <SEED/Source/Manager/DxManager/PSO/MSPipeline.h>
#include <SEED/Source/Manager/DxManager/PSO/RootSignature.h>
// managers
#include <SEED/Source/Manager/DxManager/PolygonManager.h>
#include <SEED/Source/Manager/DxManager/ViewManager.h>
#include <SEED/Source/Manager/DxManager/ShaderDictionary.h>
#include <SEED/Source/Manager/CameraManager/CameraManager.h>
// structs
#include <SEED/Source/Manager/DxManager/DxResource.h>

#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

// imgui
#define USE_IMGUI_API
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>
#include <ImGuizmo.h>

// assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


struct LeakChecker{
    ~LeakChecker();
};

class DxManager{

    friend PolygonManager;
    friend DxResource;
    friend class PostEffect;
    friend class SEED;
    friend class TextureManager;
    friend class TextSystem;
    friend class ImGuiManager;
    friend class PSOManager;
    friend class ViewManager;

public:/*========================== 根幹をなす大枠の関数 ==========================*/
    ~DxManager();
    static void Initialize(SEED* pSEED);
    static void Finalize();
    static DxManager* GetInstance();

private:

    // privateコンストラクタ
    DxManager() = default;

    // コピー禁止
    DxManager(const DxManager&) = delete;
    void operator=(const DxManager&) = delete;

    // インスタンス
    static DxManager* instance_;

private:/*===================== 内部の細かい初期設定を行う関数 ======================*/

    void CreateDebugLayer();
    void CreateDevice();
    void CheckDebugLayer();

    void CreateCommanders();

    // SwapChain,ダブルバッファリングに関わる関数
    void CreateRenderTargets();

    // リソースの初期化に関わる関数
    void StartUpload();

    // PSO
    void InitPSO();

    // viewportとscissorの設定関数
    void SettingViewportAndScissor(float resolutionRate);

    // preDraw,postDrawに関わる関数
    void TransitionResourceState(ID3D12Resource* resource, D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter);
    void ClearViewSettings();

    // GPUとCPUの同期に関わる関数
    void CreateFence();
    void WaitForGPU();

private:/*============================ 描画に関わる関数 ============================*/

    void PreDraw();
    void DrawPolygonAll();
    void DrawGUI();
    void PostDraw();

private:/*==================== アクセッサ以外で外部から呼び出す関数 ====================*/

    // 解像度を変更してRTVやScissorを設定し直す関数
    void ChangeResolutionRate(float resolutionRate);
    void ReCreateResolutionSettings();

private:/*============================ その他の関数 ===============================*/

    // 前フレームの変数値を保存する関数
    void SavePreVariable();

    // リソースの解放を手動で行う関数
    void Release();

private:/*========================= 外部参照のためのポインタ ========================*/

    SEED* pSEED_ = nullptr;

private:/*============================ マネージャー変数 ============================*/

    PolygonManager* polygonManager_ = nullptr;

private:/*============================== オブジェクト =============================*/

    std::unordered_map<std::string, BaseCamera*> cameras_;
    std::string mainCameraName_;
    BaseCamera* mainCamera_;// メインのカメラ(実際に描画するカメラ)

private:/*================================= 定数 =================================*/

    static const int8_t kTopologyCount = 2;
    static const int8_t kCullModeCount = 3;

private:/*============================ パラメーター変数 ============================*/

    float resolutionRate_ = 1.0f;
    float preResolutionRate_ = resolutionRate_;
    bool changeResolutionOrder = false;


private:/*======================== DirectXの設定に必要な変数 ========================*/

    // いろんなとこで実行結果を格納してくれる変数
    HRESULT hr;

    // CPU-GPUの同期のための変数
    ComPtr<ID3D12Fence> fence = nullptr;
    HANDLE fenceEvent;
    uint64_t fenceValue;

    // DirectX すべてのはじまり編
    ComPtr<ID3D12Debug1> debugController = nullptr;
    ComPtr<IDXGIFactory7> dxgiFactory = nullptr;
    ComPtr<IDXGIAdapter4> useAdapter = nullptr;// アダプタを格納する変数
    ComPtr<ID3D12Device10> device = nullptr;// 生成したデバイスを格納する変数

    // commandList類
    ComPtr<ID3D12InfoQueue> infoQueue = nullptr;
    ComPtr<ID3D12CommandQueue> commandQueue = nullptr;
    ComPtr<ID3D12CommandAllocator> commandAllocator = nullptr;// コマンドアロケータを格納する変数
    ComPtr<ID3D12GraphicsCommandList6> commandList = nullptr;// コマンドリストを格納する変数

    /*====================== レンダーターゲット関係 ========================*/

    // カメラごとにオフスクリーンリソースを格納するマップ
    std::unordered_map<std::string, DxResource> offScreenResources;
    std::unordered_map<std::string, D3D12_CPU_DESCRIPTOR_HANDLE> offScreenHandles;
    std::unordered_map<std::string, std::string> offScreenNames; // オフスクリーンの名前を格納するマップ
    // その他
    Vector4 clearColor;

    //=====================================================================================//

    // DepthStencilTextureResource(これもカメラごとに)
    std::unordered_map<std::string, DxResource> depthStencilResources;
    std::unordered_map<std::string, D3D12_CPU_DESCRIPTOR_HANDLE> dsvHandles;
    std::unordered_map<std::string, std::string> depthStencilNames;

    // scissor矩形とviewport
    D3D12_VIEWPORT viewport{};
    D3D12_RECT scissorRect{};
    D3D12_VIEWPORT viewport_default{};
    D3D12_RECT scissorRect_default{};


private:/*============================ アクセッサ関数 ============================*/

    BaseCamera* GetCamera(const std::string& name){
        if(cameras_.find(name) == cameras_.end()){
            assert(false);
        }
        return cameras_[name];
    }
    void RegisterCamera(std::string name,BaseCamera* pCamera){
        CameraManager::RegisterCamera(name,pCamera);
        cameras_[name] = pCamera; // カメラのポインタを登録
        CreateRenderResource(name); // レンダリング用リソースを作成(なければ)
        //activeなカメラとして設定
        CameraManager::SetIsCameraActive(name, true);
    }
    // 実際の画面に表示するメインのカメラを設定
    void SetMainCamera(const std::string& name){
        if(cameras_.find(name) != cameras_.end()){
            mainCameraName_ = name;
            mainCamera_ = cameras_[name];
        }
    }
    std::string GetMainCameraName() const {
        return mainCameraName_;
    }
    void RemoveCamera(const std::string& name){
        CameraManager::RemoveCamera(name);
        if(cameras_.find(name) != cameras_.end()){
            cameras_.erase(name);
        }
    }
    void SetIsCameraActive(const std::string& name, bool isActive){
        CameraManager::SetIsCameraActive(name, isActive);
    }
    void SetChangeResolutionFlag(bool flag){ changeResolutionOrder = flag; }
    float GetResolutionRate(){ return resolutionRate_; }
    float GetPreResolutionRate(){ return preResolutionRate_; }
    void CreateRenderResource(const std::string& cameraName);

public:
    ID3D12Device* GetDevice()const{ return device.Get(); }
};