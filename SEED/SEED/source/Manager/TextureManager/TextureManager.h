#pragma once

// stl
#include <cstdint>
#include <unordered_map>
#include <string>
#include <cassert>
// local
#include <SEED/Lib/Functions/DxFunc.h>
// assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
// imgui
#include <imgui.h>

/// <summary>
/// テクスチャを読み込み管理するクラス
/// </summary>
class TextureManager{

private:
    // privateコンストラクタ
    TextureManager() = default;

    // コピー禁止
    TextureManager(const TextureManager&) = delete;
    void operator=(const TextureManager&) = delete;

public:
    ~TextureManager();

public:
    static const TextureManager* GetInstance();
    static void Initialize();
    static void Release();
    static void StartUpLoad();
    static uint32_t LoadTexture(const std::string& filename,const aiTexture* embeddedTexture = nullptr);

private:
    uint32_t CreateTexture(const std::string& filename, const aiTexture* embeddedTexture = nullptr);

private:
    static TextureManager* instance_;
    std::unordered_map<std::string, uint32_t>graphHandle_;
    // 読み込んだTextureのResource
    std::vector<ComPtr<ID3D12Resource>> textureResources;
    std::vector<ComPtr<ID3D12Resource>> intermediateResources;

public:
    static uint32_t GetGraphHandle(const std::string& fileName);
    static D3D12_GPU_DESCRIPTOR_HANDLE GetHandleGPU(const std::string& fileName);
    static ImTextureID GetImGuiTexture(const std::string& fileName);
};