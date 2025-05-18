#pragma once

// external
#include <windows.h>
#include <gdiplus.h>
#include <cstdint>
#include <memory>

#pragma comment (lib,"gdiplus.lib")

// local
#include <SEED/Lib/Functions/MyFunc/DxFunc.h>
#include <SEED/Source/Manager/DxManager/PSO/Pipeline.h>
#include <SEED/Source/Manager/DxManager/PSO/MSPipeline.h>
#include <SEED/Source/Manager/DxManager/PSO/RootSignature.h>
#include <SEED/Lib/Functions/MyFunc/MatrixFunc.h>
#include <SEED/Lib/Shapes/Sphere.h>
#include <SEED/Lib/Includes/includes.h>
#include <SEED/Source/Manager/DxManager/PolygonManager.h>
#include <SEED/Source/Manager/DxManager/EffectManager.h>
#include <SEED/Source/Object/Camera/BaseCamera.h>
#include <SEED/Source/Manager/CameraManager/CameraManager.h>
#include <SEED/Source/Manager/DxManager/ViewManager.h>
#include <SEED/Source/Manager/DxManager/ShaderDictionary.h>

#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

// imgui
#include <imgui.h>
#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>

// assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class SEED;
class TextureManager;
class ImGuiManager;
class PSOManager;
class ViewManager;


struct LeakChecker{
    ~LeakChecker();
};

class DxManager{

    friend PolygonManager;
    friend EffectManager;
    friend SEED;
    friend TextureManager;
    friend ImGuiManager;
    friend PSOManager;
    friend ViewManager;

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
    void CreateRTV();

    // 
    void InitializeSystemTextures();

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

    // テクスチャを読み込む関数
    uint32_t CreateTexture(std::string filePath,const aiTexture* embeddedTexture = nullptr);

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
    std::unique_ptr<EffectManager> effectManager_ = nullptr;

private:/*============================== オブジェクト =============================*/

    BaseCamera* camera_;
    bool isDebugCameraAvtive_ = false;

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

    // オフスクリーン用
    ComPtr<ID3D12Resource> offScreenResource = nullptr;
    D3D12_CPU_DESCRIPTOR_HANDLE offScreenHandle;
    // その他
    Vector4 clearColor;

    //================================ CSのバッファ ===============================//

    ComPtr<ID3D12Resource> CS_ConstantBuffer = nullptr;

    //================================ テクスチャ ===================================//

    // TextureResource
    std::vector<ComPtr<ID3D12Resource>> textureResource;
    std::vector<ComPtr<ID3D12Resource>> intermediateResource;

    // ポストエフェクト用のテクスチャ
    ComPtr<ID3D12Resource> blurTextureResource;// ぼけた画像
    ComPtr<ID3D12Resource> depthTextureResource;// 深度情報の白黒画像

    //=====================================================================================//

    // DepthStencilTextureResourceの作成
    ComPtr<ID3D12Resource> depthStencilResource = nullptr;
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle;

    // scissor矩形とviewport
    D3D12_VIEWPORT viewport{};
    D3D12_RECT scissorRect{};
    D3D12_VIEWPORT viewport_default{};
    D3D12_RECT scissorRect_default{};


private:/*============================ アクセッサ関数 ============================*/

    BaseCamera* GetCamera()const{ return camera_; }
    void SetCamera(std::string nextCameraName){
        camera_ = CameraManager::GetCamera(nextCameraName);
        //activeなカメラを設定
        CameraManager::SetActiveCamera(nextCameraName);
    }
    void SetCamera(BaseCamera* camera){ camera_ = camera; }
    void SetChangeResolutionFlag(bool flag){ changeResolutionOrder = flag; }
    float GetResolutionRate(){ return resolutionRate_; }
    float GetPreResolutionRate(){ return preResolutionRate_; }

public:
    ID3D12Device* GetDevice()const{ return device.Get(); }
};