#include "ModelManager.h"


// static変数初期化
ModelManager* ModelManager::instance_ = nullptr;
const std::string ModelManager::directoryPath_ = "resources/models";

ModelManager::~ModelManager(){
    // モデルデータの解放
    for(auto& model : modelData_){
        delete model.second;
        model.second = nullptr;
    }

    // インスタンスがあれば解放
    if(instance_){
        delete instance_;
        instance_ = nullptr;
    }
}

const ModelManager* ModelManager::GetInstance(){
    if(!instance_){
        instance_ = new ModelManager();
    }

    return instance_;
}

void ModelManager::Initialize(){
    // インスタンスなければ作成
    GetInstance();

    // 初期化時読み込み
    StartUpLoad();

}

// 起動時に読み込みたいモデルをここで読み込む
void ModelManager::StartUpLoad(){
    LoadModel("Player_result.gltf");
    //LoadModel("ground.obj");
}

void ModelManager::LoadModel(const std::string& filename){
    // すでに読み込み済みのファイルであればreturn
    if(instance_->modelData_.find(filename) != instance_->modelData_.end()){ return; }

    // 読み込み
    instance_->modelData_[filename] = instance_->LoadModelFile(instance_->directoryPath_, filename);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// モデルファイルを読み込む
///////////////////////////////////////////////////////////////////////////////////////////////////////////

ModelData* ModelManager::LoadModelFile(const std::string& directoryPath, const std::string& filename){

    // assinmpのインポート設定
    Assimp::Importer importer;
    std::string filePath = directoryPath + "/" + filename.substr(0, filename.find_last_of('.')) + "/" + filename;
    const aiScene* scene = importer.ReadFile(
        filePath.c_str(),
        // 三角形反転・UV反転・自動三角形化
        aiProcess_FlipWindingOrder | aiProcess_FlipUVs | aiProcess_Triangulate
    );


    assert(scene->HasMeshes());// メッシュがない場合はアサート


    // 結果格納用
    ModelData* modelData = new ModelData();


    // ルートノードの読み込み
    modelData->rootNode = ReadModelNode(scene->mRootNode);
    // メッシュデータの読み込み
    modelData->meshes = ParseMeshes(scene);
    // マテリアルデータの読み込み
    modelData->materials = ParseMaterials(scene);
    modelData->modelName = filename;


    return modelData;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////
// メッシュデータを解析
///////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<MeshData> ModelManager::ParseMeshes(const aiScene* scene){
    std::vector<MeshData> meshes;

    for(uint32_t meshIdx = 0; meshIdx < scene->mNumMeshes; ++meshIdx) {
        aiMesh* mesh = scene->mMeshes[meshIdx];
        MeshData meshData;

        assert(mesh->HasNormals());          // 法線必須
        assert(mesh->HasTextureCoords(0));   // UV座標必須

        // faceを解析する
        for(uint32_t faceIdx = 0; faceIdx < mesh->mNumFaces; ++faceIdx){

            aiFace& face = mesh->mFaces[faceIdx];
            assert(face.mNumIndices == 3);// 三角形以外は

            // vertexを解析する
            for(uint32_t element = 0; element < face.mNumIndices; ++element){

                // 頂点の情報を取得
                uint32_t vertexIdx = face.mIndices[element];
                aiVector3D position = mesh->mVertices[vertexIdx];
                aiVector3D normal = mesh->mNormals[vertexIdx];
                aiVector3D texcoord = mesh->mTextureCoords[0][vertexIdx];

                // 頂点を格納
                VertexData vertex;
                vertex.position_ = { position.x,position.y,position.z,1.0f };
                vertex.normal_ = { normal.x,normal.y,normal.z };
                vertex.texcoord_ = { texcoord.x,texcoord.y };

                // 左手座標系に変換
                vertex.position_.x *= -1.0f;
                vertex.normal_.x *= -1.0f;

                // 頂点を格納
                meshData.vertices.push_back(vertex);
            }


            //// インデックスデータを構築
            for(uint32_t i = 0; i < 3; ++i) {
                meshData.indices.push_back(face.mIndices[i]);
            }

            // マテリアルインデックスを設定
            meshData.materialIndex = mesh->mMaterialIndex;
        }

        // メッシュを追加
        meshes.push_back(meshData);
    }

    return meshes;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// マテリアルデータを解析
///////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<MaterialData> ModelManager::ParseMaterials(const aiScene* scene){
    std::vector<MaterialData> materials;

    // マテリアルを読み込む
    for(uint32_t materialIdx = 0; materialIdx < scene->mNumMaterials; ++materialIdx) {
        aiMaterial* material = scene->mMaterials[materialIdx];
        MaterialData materialData;

        aiString texturePath;
        if(material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS) {
            materialData.textureFilePath_ = std::string(texturePath.C_Str());
        }

        // テクスチャがない場合は白テクスチャを設定
        if(materialData.textureFilePath_ == ""){
            materialData.textureFilePath_ = "white1x1.png";
        }

        materials.push_back(materialData);
    }

    // メッシュごとのマテリアル割り当て
    std::vector<MaterialData> meshMaterials;
    for(uint32_t meshIdx = 0; meshIdx < scene->mNumMeshes; ++meshIdx) {
        aiMesh* mesh = scene->mMeshes[meshIdx];
        uint32_t materialIndex = mesh->mMaterialIndex;

        // マテリアルが存在する場合は対応するものを割り当て
        if(materialIndex < materials.size()) {
            meshMaterials.push_back(materials[materialIndex]);
        } else {
            // マテリアルが対応していない場合はデフォルトマテリアルを割り当て
            MaterialData defaultMaterial;
            defaultMaterial.textureFilePath_ = "white1x1.png";
            meshMaterials.push_back(defaultMaterial);
        }
    }


    // テクスチャのUV変換行列を初期化
    for(auto& material : meshMaterials){
        material.UV_offset_ = { 0.0f,0.0f,0.0f };
        material.UV_scale_ = { 1.0f,1.0f,1.0f };
        material.UV_translate_ = { 0.0f,0.0f,0.0f };
    }

    return meshMaterials;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// ノードデータを解析
///////////////////////////////////////////////////////////////////////////////////////////////////////////

ModelNode ModelManager::ReadModelNode(const aiNode* node){
    ModelNode result;

    // ノードのローカル行列を取得
    aiMatrix4x4 aiLocalMatrix = node->mTransformation;
    aiLocalMatrix.Transpose(); // 転置（Assimpは列優先）

    result.localMatrix = Matrix4x4(
        aiLocalMatrix.a1, aiLocalMatrix.a2, aiLocalMatrix.a3, aiLocalMatrix.a4,
        aiLocalMatrix.b1, aiLocalMatrix.b2, aiLocalMatrix.b3, aiLocalMatrix.b4,
        aiLocalMatrix.c1, aiLocalMatrix.c2, aiLocalMatrix.c3, aiLocalMatrix.c4,
        aiLocalMatrix.d1, aiLocalMatrix.d2, aiLocalMatrix.d3, aiLocalMatrix.d4
    );

    result.name = node->mName.C_Str();

    // 子ノードを再帰的に処理
    for(uint32_t i = 0; i < node->mNumChildren; ++i) {
        result.children.push_back(ReadModelNode(node->mChildren[i]));
    }

    return result;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// マテリアルファイルを読み込む
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//MaterialData ModelManager::LoadMaterialFile(const std::string& directoryPath, const std::string& filename){
//
//    // ファイルを開く
//    std::ifstream file(directoryPath + "/" + filename);
//    assert(file.is_open());// 失敗したらアサート
//
//    MaterialData materialData;
//    std::string line;
//
//    while(std::getline(file, line)){
//
//        // まずobjファイルの行の先頭の識別子を読む
//        std::string identifer;
//        std::istringstream s(line);
//        s >> identifer;
//
//        if(identifer == "map_Kd"){// ファイル名
//
//            std::string textureFilename;
//            Vector3 scale = { 1.0f,1.0f,1.0f };
//            Vector3 offset = { 0.0f,0.0f,0.0f };
//            Vector3 translate = { 0.0f,0.0f,0.0f };
//
//            // ファイル名を格納
//            while(s >> textureFilename) {
//                if(textureFilename[0] == '-') {
//                    std::string option = textureFilename.substr(1);
//                    if(option == "s") {
//                        s >> scale.x >> scale.y >> scale.z;
//                    } else if(option == "o") {
//                        s >> offset.x >> offset.y >> offset.z;
//                    } else if(option == "t") {
//                        s >> translate.x >> translate.y >> translate.z;
//                    }
//                } else {
//                    materialData.textureFilePath_ = textureFilename;
//                }
//            }
//
//            materialData.UV_scale_ = scale;
//            materialData.UV_offset_ = offset;
//            materialData.UV_translate_ = translate;
//        }
//    }
//
//    // テクスチャなしのモデルの場合
//    if(materialData.textureFilePath_ == ""){
//        materialData.textureFilePath_ = "white1x1.png";
//    }
//
//    return materialData;
//}
//
