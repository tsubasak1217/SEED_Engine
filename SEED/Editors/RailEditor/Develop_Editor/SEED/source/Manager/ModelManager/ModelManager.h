#pragma once

#include <cstdint>
#include <unordered_map>
#include <string>
#include <cassert>

// local
#include "modelData.h"

class ModelManager{

private:

    // privateコンストラクタ
    ModelManager() = default;

    // コピー禁止
    ModelManager(const ModelManager&) = delete;
    void operator=(const ModelManager&) = delete;

public:

    ~ModelManager();

public:

    static const ModelManager* GetInstance();
    static void Initialize();
    static void StartUpLoad();

public:
    static void LoadModel(const std::string& filename);

private:
    static ModelManager* instance_;
    std::unordered_map<std::string, ModelData>modelData_;
    static const std::string directoryPath_;

public:
    static ModelData* GetModelData(const std::string& fileName){
        assert(instance_->modelData_.find(fileName) != instance_->modelData_.end());
        return &instance_->modelData_[fileName];
    }
};