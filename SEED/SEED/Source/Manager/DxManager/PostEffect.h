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

class PostEffect{
    friend class DxManager;// DxManagerからアクセスを許可
    friend class PolygonManager; // PolygonManagerからアクセスを許可
    friend class IPostProcess; // IPostProcessからアクセスを許可
    friend struct ImFunc; // ImGuiManagerからアクセスを許可
private:
    PostEffect() = default;
    ~PostEffect();
    PostEffect(PostEffect&) = delete;
    void operator=(PostEffect&) = delete;
    static PostEffect* instance_;
    static PostEffect* GetInstance();
    static void Update();

private:
    void InitPSO();
    void CreateResources();
    void Initialize();
    void Release();
    void CopyOffScreen();

private:// PostEffectの処理

    // 有効なポストエフェクトを適用する関数
    void PostProcess();

    // ResourceのTransition関連
    void EndTransition();
    void StartTransition();

    // バッファ関連の関数
    void ChangeBufferIndex(){
        currentBufferIndex_ = (currentBufferIndex_ + 1) % 2;
    }

    static std::string GetCurSRVBufferName(){
        return instance_->currentBufferIndex_ == 0 ? "postEffect_0" : "postEffect_1";
    }

    static std::string GetCurUAVBufferName(){
        return instance_->currentBufferIndex_ == 0 ? "postEffect_1_UAV" : "postEffect_0_UAV";
    }


private:
    // ポストエフェクトに使用するパラメータなど
    float resolutionRate_;
    float time_ = 0.0f; // 時間の経過を記録する変数

    // PostEffectに必要なリソース
    DxResource postEffectTextureResource[2];// ポストエフェクト画像
    DxResource postEffectResultResource; // ポストエフェクトの結果を格納するリソース

    // 
    int currentBufferIndex_ = 0; // 現在のバッファインデックス

    // ポストプロセスのリスト
    std::list<std::unique_ptr<IPostProcess>> postProcesses_;

private:
    void Edit();
};
