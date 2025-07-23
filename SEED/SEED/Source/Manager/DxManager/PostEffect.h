#pragma once
#include <SEED/Lib/Functions/MyFunc/DxFunc.h>
#include <SEED/Source/Manager/DxManager/DxResource.h>
#include <SEED/Source/Manager/DxManager/PSO/PSOManager.h>

class DxManager;
class SEED;

enum class DEAPTH_TEXTURE_FORMAT{
    NORMAL = 0,
    FOCUS
};

enum class DX_RESOURCE_TYPE{
    TEXTURE2D = 0,
    TEXTURE3D,
    RENDER_TARGET,
    DEPTH_STENCIL,
    BUFFER
};

enum class PostEffectBit{
    None = 0,
    Grayscale = 1 << 0, // グレースケール
    DoF = 1 << 1, // 被写界深度
};

class PostEffect{
    friend class DxManager;// DxManagerからアクセスを許可
    friend class PolygonManager; // PolygonManagerからアクセスを許可
    friend struct ImFunc; // ImGuiManagerからアクセスを許可
private:
    PostEffect() = default;
    ~PostEffect();
    PostEffect(PostEffect&) = delete;
    void operator=(PostEffect&) = delete;
    static PostEffect* instance_;
    static PostEffect* GetInstance();

private:
    void InitPSO();
    void MapOnce();
    void SetBindInfo();
    void CreateResources();
    void Initialize();
    void Release();
    void Dispatch(std::string pipelineName, int32_t gridX = 16, int32_t gridY = 16);
    void CopyOffScreen();

private:// PostEffectの処理

    // 有効なポストエフェクトを適用する関数
    void PostProcess();

    // グレースケール
    void Grayscale();

    // 被写界深度
    void DoF();

    // ResourceのTransition関連
    void EndTransition();
    void StartTransition();

    std::string GetCurrentBufferName() const{
        return "postEffect_" + std::to_string(currentBufferIndex_);
    }

private:
    // PostEffectに必要なリソース
    float resolutionRate_;
    DxResource postEffectTextureResource[2];// ポストエフェクト画像
    DxResource depthTextureResource;// 深度情報の白黒画像
    DxResource postEffectResultResource; // ポストエフェクトの結果を格納するリソース

    // 
    int currentBufferIndex_ = 0; // 現在のバッファインデックス
    PostEffectBit postEffectBit_ = PostEffectBit::None; // 適��するポストエフェクトのビットフラグ
};
