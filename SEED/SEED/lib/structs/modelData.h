#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <SEED/Lib/Structs/VertexData.h>
#include <SEED/Lib/Structs/Material.h> 
#include <SEED/Lib/Structs/Transform.h>
#include <SEED/Lib/Structs/ModelAnimation.h>

// メッシュデータ構造体
struct MeshData{
    std::vector<VertexData> vertices; // 頂点データ
    std::vector<uint32_t> indices;   // インデックスデータ
    int materialIndex;               // 使用するマテリアルのインデックス
    std::vector<VertexInfluence> vertexInfluences; // 頂点の影響情報
};

// モデルのノード構造体
struct ModelNode{
    QuaternionTransform transform;
    Matrix4x4 localMatrix;
    std::string name;
    std::vector<ModelNode> children;
};

// モデルデータ構造体
struct ModelData{
    std::vector<MeshData>meshes;
    std::vector<ModelMaterialLoadData> materials;
    std::unordered_map<std::string, ModelAnimation> animations;
    std::unordered_map<std::string, JointWeightData> jointWeightData;
    ModelSkeleton defaultSkeleton;
    SkinCluster defaultSkinClusterData;
    ModelNode rootNode;
    std::string modelName;
};