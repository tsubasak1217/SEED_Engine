#pragma once
#include <SEED/Lib/Functions/MyFunc/DxFunc.h>
#include <SEED/Source/Manager/DxManager/DxResource.h>
#include <SEED/Source/Manager/DxManager/PSO/PSOManager.h>

class DxManager;
class SEED;

enum class DEAPTH_TEXTURE_FORMAT {
    NORMAL = 0,
    FOCUS
};

enum class DX_RESOURCE_TYPE {
    TEXTURE2D = 0,
    TEXTURE3D,
    RENDER_TARGET,
    DEPTH_STENCIL,
    BUFFER
};

class PostEffect{
    friend class DxManager;// DxManagerからのみアクセスを許可
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

private:// PostEffectの処理

    // 被写界深度
    void DoF();

    // ResourceのTransition関連
    void BeforeBackBufferDrawTransition();
    void EndTransition();

private:
    // PostEffectに必要なリソース
    float resolutionRate_;
    DxResource blurTextureResource;// ぼかし画像
    DxResource depthTextureResource;// 深度情報の白黒画像
};
