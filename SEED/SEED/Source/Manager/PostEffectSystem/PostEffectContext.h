#pragma once
#include <string>
#include <memory>
#include <unordered_map>
#include <json.hpp>
#include "PostEffectContext.h"

// 前方宣言
class DxManager;
class SEED;
class IPostProcess;

// ポストエフェクトハンドル
using PostEffectHandle = int32_t;

// 各種Enum
enum class DEAPTH_TEXTURE_FORMAT{
    NORMAL = 0,
    FOCUS
};

// リソースの種類
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


// ポストプロセスの最小限の情報を持つ構造体
template<typename T>
struct MinimalPostEffect{
    static_assert(std::is_base_of_v<IPostProcess, T>, "T must inherit from IPostProcess");
    T* pPostProcess;
    PostEffectHandle handle;
};