#pragma once
#include <string>
#include <unordered_map>
#include <SEED/Lib/Functions/MyFunc/DxFunc.h>
#include <SEED/Source/Manager/DxManager/DxResource.h>
#include <SEED/Source/Manager/DxManager/PSO/PSOManager.h>
#include <json.hpp>

class DxManager;
class SEED;
class IPostProcess;

using PostEffectHandle = int32_t;

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

// 複数のポストプロセスをまとめて扱う構造体
struct PostProcessGroup{
    bool isActive = true;
    bool removeOrder = false;
    std::vector<std::pair<std::unique_ptr<IPostProcess>, bool>> postProcesses;
    PostEffectHandle handle_ = -1;
    void Edit();
    void FromJson(const nlohmann::json& json);
    nlohmann::json ToJson()const;
};


// ポストエフェクト管理クラス
class PostEffect{
    friend class DxManager;// DxManagerからアクセスを許可
    friend class PolygonManager; // PolygonManagerからアクセスを許可
    friend struct ImFunc; // ImGuiManagerからアクセスを許可
    friend IPostProcess; // IPostProcessからアクセスを許可
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

    // PostEffectに必要なリソース
    DxResource postEffectTextureResource[2];// ポストエフェクト画像
    DxResource postEffectResultResource; // ポストエフェクトの結果を格納するリソース

    // 現在のバッファインデックス
    int currentBufferIndex_ = 0;

    // ポストプロセスのリスト
    PostEffectHandle nextHandle_ = 0;
    std::unordered_map<PostEffectHandle, PostProcessGroup> postProcessGroups_;

private:
    void Edit();

public:
    static PostEffectHandle Load(const std::string& fileName);
    static void DeleteAll();
    static void Delete(PostEffectHandle handle);
    static void SetActive(PostEffectHandle handle, bool isActive);
};