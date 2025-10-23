#pragma once
#include <SEED/Lib/Functions/DxFunc.h>
#include <SEED/Source/Manager/DxManager/DxResource.h>
#include <SEED/Source/Manager/DxManager/DxManager.h>
#include <SEED/Source/Manager/DxManager/DxBuffer.h>
#include <SEED/Source/Manager/DxManager/PSO/PSOManager.h>
#include <SEED/Source/Manager/ImGuiManager/ImGuiManager.h>
#include <Environment/Environment.h>
#include <json.hpp>

/// <summary>
/// ポストプロセスの基底クラス
/// </summary>
class IPostProcess{
    friend class PostEffectSystem;
    friend struct PostProcessGroup;
public:
    IPostProcess();
    virtual ~IPostProcess() = default;

public:
    // ポストプロセスの初期化
    virtual void Initialize() = 0;
    // ポストプロセスの更新
    virtual void Update() = 0;
    // ポストプロセスの適用
    virtual void Apply() = 0;
    // ポストプロセスのリソースを解放
    virtual void Release() = 0;
    // リソースの開始遷移
    virtual void StartTransition() = 0;
    // リソースの終了遷移
    virtual void EndTransition() = 0;


public:
    int GetId() const {
        return id_;
    }
    bool GetIsActive() const {
        return isActive_;
    }

protected:
    // 自身はfriend classでアクセスができるので、子クラスにも使えるようにする
    ID3D12Device10* GetDevice() {
        return DxManager::GetInstance()->device.Get();
    }
    void Dispatch(
        const std::string& pipelineName, int32_t gridX = 16, int32_t gridY = 16,
        int32_t texWidth = kWindowSizeX, int32_t texHeight = kWindowSizeY,
        bool isSwapBuffer = true
    );

    // 代わりにバッファの名前を取得する関数
    std::string GetCurUAVBufferName();
    std::string GetCurSRVBufferName();
    std::string GetCurDepthBufferName();

protected:
    static inline int nextId_ = 0;
    int id_;
    bool isActive_ = true; // ポストプロセスが有効かどうか

public:
    virtual void Edit() = 0;
    virtual nlohmann::json ToJson() = 0;
    virtual void FromJson(const nlohmann::json& json) = 0;
};