#pragma once

#include <cstdint>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <cassert>
#include <fstream>
#include <sstream>
#include <filesystem>

// assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// local
#include <SEED/Lib/Structs/ModelData.h>
#include <SEED/Lib/Structs/ModelAnimation.h>

class ModelManager{

    friend class Model;
    friend class PolygonManager;

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
    ModelData* LoadModelFile(const std::string& directoryPath, const std::string& filename);
    std::vector<MeshData> ParseMeshes(const aiScene* scene);
    void CreateMeshlet(ModelData* modelData);
    std::vector<ModelMaterialLoadData> ParseMaterials(const aiScene* scene, const std::string& modelName);

    // アニメーション関連
    std::unordered_map<std::string,ModelAnimation> LoadAnimation(const std::string& directoryPath, const std::string& filename);
    ModelNode ReadModelNode(const aiNode* node);
    int32_t CreateJoint(const ModelNode& node,const std::optional<int32_t>& parent,std::vector<ModelJoint>& joints);
    ModelSkeleton CreateSkeleton(const ModelNode& rootNode);
    static void AnimatedSkeleton(const ModelAnimation& modelAnimation,const ModelSkeleton& defaultSkeleton, ModelSkeleton* pSkeleton,float time);
    static ModelSkeleton InterpolateSkeleton(const ModelSkeleton& skeletonA, const ModelSkeleton& skeletonB, float t);
    void UpdateSkeleton(ModelSkeleton* skeleton);
    std::unordered_map<std::string, JointWeightData> CreateJointWeightData(const aiScene* scene);
    void CreateVertexInfluence(const ModelSkeleton& skeleton, ModelData* modelData);

private:
    static ModelManager* instance_;
    std::unordered_map<std::string, ModelData*>modelData_;
    static const std::string directoryPath_;

public:
    static ModelData* GetModelData(const std::string& fileName){
        assert(instance_->modelData_.find(fileName) != instance_->modelData_.end());
        return instance_->modelData_[fileName];
    }

    static std::vector<std::string> GetModelNames(){
        std::vector<std::string> modelNames;
        modelNames.reserve(instance_->modelData_.size());
        for(const auto& pair : instance_->modelData_){
            modelNames.push_back(pair.first);
        }
        return modelNames;
    }
};