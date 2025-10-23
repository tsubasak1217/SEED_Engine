#pragma once
#include <string>
#include <unordered_map>
#include <SEED/Lib/Functions/DxFunc.h>
#include <SEED/Source/Manager/DxManager/DxResource.h>
#include <SEED/Source/Manager/DxManager/PSO/PSOManager.h>
#include <json.hpp>
#include "PostEffectContext.h"



/// <summary>
// ポストエフェクトを管理するクラス
/// </summary>
class PostEffectSystem{
    friend class DxManager;// DxManagerからアクセスを許可
    friend class PolygonManager; // PolygonManagerからアクセスを許可
    friend struct ImFunc; // ImGuiManagerからアクセスを許可
    friend IPostProcess; // IPostProcessからアクセスを許可
private:
    PostEffectSystem() = default;
    ~PostEffectSystem();
    PostEffectSystem(PostEffectSystem&) = delete;
    void operator=(PostEffectSystem&) = delete;
    static PostEffectSystem* instance_;
    static PostEffectSystem* GetInstance();
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

    // ポストプロセスの追加
    template<typename T>
    static MinimalPostEffect<T> AddPostProcess();
};



// ポストプロセスの追加(ファイルからではなく直接)
template<typename T>
inline MinimalPostEffect<T> PostEffectSystem::AddPostProcess(){

    // TがISceneTransitionを継承していない
    if constexpr(!std::is_base_of_v<IPostProcess, T>){
        return { nullptr,-1 };
    }

    // グループを追加
    instance_->postProcessGroups_[instance_->nextHandle_] = PostProcessGroup();
    auto& newGroup = instance_->postProcessGroups_[instance_->nextHandle_];
    newGroup.handle_ = instance_->nextHandle_;

    // ポストプロセスを追加
    newGroup.postProcesses.emplace_back(std::make_pair(std::make_unique<T>(), false));
    T* newPostProcess = static_cast<T*>(newGroup.postProcesses.back().first.get());
    newPostProcess->Initialize();

    // 結果を返す
    return { newPostProcess, instance_->nextHandle_++ };
}
