#pragma once

#include <cstdint>
#include <unordered_map>
#include <string>
#include <cassert>
#include <fstream>
#include <sstream>

// assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

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
    static void LoadModel(const std::string& filename);

private:
    // Loadに必要な関数
    ModelData* LoadObjFile(const std::string& directoryPath, const std::string& filename);
    MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

private:
    static ModelManager* instance_;
    std::unordered_map<std::string, ModelData*>modelData_;
    static const std::string directoryPath_;

public:
    static ModelData* GetModelData(const std::string& fileName){
        assert(instance_->modelData_.find(fileName) != instance_->modelData_.end());
        return instance_->modelData_[fileName];
    }
};