#pragma once

#include <cstdint>
#include <unordered_map>
#include <string>
#include <cassert>

// assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

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
    static void StartUpLoad();

public:
    static uint32_t LoadTexture(const std::string& filename,const aiTexture* embeddedTexture = nullptr);

private:
    static TextureManager* instance_;
    std::unordered_map<std::string, uint32_t>graphHandle_;

public:
    static uint32_t GetGraphHandle(const std::string& fileName){
        assert(instance_->graphHandle_.find(fileName) != instance_->graphHandle_.end());
        return instance_->graphHandle_[fileName];
    }
};