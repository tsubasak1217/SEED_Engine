#pragma once
#include <vector>
#include "VertexData.h"
#include "Material.h" 

// メッシュデータ構造体
struct MeshData{
    std::vector<VertexData> vertices; // 頂点データ
    std::vector<uint32_t> indices;   // インデックスデータ
    int materialIndex;               // 使用するマテリアルのインデックス
};

// モデルのノード構造体
struct ModelNode{
    Matrix4x4 localMatrix;
    std::string name;
    std::vector<ModelNode> children;
};

// モデルデータ構造体
struct ModelData{
    std::vector<MeshData>meshes;
    std::vector<MaterialData> materials;
    ModelNode rootNode;
    std::string modelName;
};