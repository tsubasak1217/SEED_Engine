#pragma once
#include <SEED/Lib/Functions/MyFunc/DxFunc.h>
#include <SEED/Source/Manager/DxManager/DxResource.h>
#include <SEED/Source/Manager/DxManager/DxManager.h>
#include <SEED/Source/Manager/DxManager/DxBuffer.h>
#include <SEED/Source/Manager/DxManager/PSO/PSOManager.h>
#include <SEED/Source/Manager/ImGuiManager/ImGuiManager.h>

class IPostProcess{
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
    bool GetIsActive() const {
        return isActive_;
    }

protected:
    // 自身はfriend classでアクセスができるので、子クラスにも使えるようにする
    ID3D12Device10* GetDevice() {
        return DxManager::GetInstance()->device.Get();
    }
    void Dispatch(const std::string& pipelineName, int32_t gridX = 16, int32_t gridY = 16);

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
};