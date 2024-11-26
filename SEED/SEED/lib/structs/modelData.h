#pragma once
#include <vector>
#include "VertexData.h"
#include "Material.h" 

struct MeshData{
    std::vector<VertexData> vertices; // 頂点データ
    std::vector<uint32_t> indices;   // インデックスデータ
    int materialIndex;               // 使用するマテリアルのインデックス
};

struct ModelData{
    std::vector<MeshData>meshes;
    std::vector<MaterialData> materials;
    std::string modelName;
};