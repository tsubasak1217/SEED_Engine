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
        aiProcess_FlipWindingOrder | aiProcess_FlipUVs | aiProcess_Triangulate | aiProcess_GenNormals
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

// 頂点データの比較用にハッシュ関数と等価演算子を定義
struct VertexHash{
    size_t operator()(const VertexData& vertex) const{
        size_t hash = 0;
        hash ^= std::hash<float>()(vertex.position_.x) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        hash ^= std::hash<float>()(vertex.position_.y) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        hash ^= std::hash<float>()(vertex.position_.z) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        hash ^= std::hash<float>()(vertex.texcoord_.x) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        hash ^= std::hash<float>()(vertex.texcoord_.y) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        return hash;
    }
};

struct VertexEqual{
    bool operator()(const VertexData& lhs, const VertexData& rhs) const{
        return lhs.position_ == rhs.position_ &&
            lhs.normal_ == rhs.normal_ &&
            lhs.texcoord_ == rhs.texcoord_;
    }
};


std::vector<MeshData> ModelManager::ParseMeshes(const aiScene* scene){
    std::vector<MeshData> meshes;

    for(uint32_t meshIdx = 0; meshIdx < scene->mNumMeshes; ++meshIdx) {
        aiMesh* mesh = scene->mMeshes[meshIdx];
        MeshData meshData;

        assert(mesh->HasTextureCoords(0)); // UV座標必須

        // 頂点を一意に管理するためのマップ
        std::unordered_map<VertexData, uint32_t, VertexHash, VertexEqual> uniqueVertices;

        // faceを解析する
        for(uint32_t faceIdx = 0; faceIdx < mesh->mNumFaces; ++faceIdx) {
            aiFace& face = mesh->mFaces[faceIdx];
            assert(face.mNumIndices == 3); // 三角形以外は無視

            // インデックスデータを構築
            for(uint32_t element = 0; element < face.mNumIndices; ++element) {
                uint32_t vertexIdx = face.mIndices[element];

                // 頂点の情報を取得
                aiVector3D position = mesh->mVertices[vertexIdx];
                aiVector3D normal = mesh->mNormals[vertexIdx];
                aiVector3D texcoord = mesh->mTextureCoords[0][vertexIdx];

                // 頂点を作成
                VertexData vertex;
                vertex.position_ = { position.x, position.y, position.z, 1.0f };
                vertex.normal_ = { normal.x, normal.y, normal.z };
                vertex.texcoord_ = { texcoord.x, texcoord.y };

                // 左手座標系に変換
                vertex.position_.x *= -1.0f;
                vertex.normal_.x *= -1.0f;

                // 頂点を一意に管理
                if(uniqueVertices.count(vertex) == 0) {
                    uniqueVertices[vertex] = static_cast<uint32_t>(meshData.vertices.size());
                    meshData.vertices.push_back(vertex);
                }

                // インデックスを追加
                meshData.indices.push_back(uniqueVertices[vertex]);
            }
        }

        // マテリアルインデックスを設定
        meshData.materialIndex = mesh->mMaterialIndex;

        // メッシュを追加
        meshes.push_back(meshData);
    }

    return meshes;
}

//std::vector<MeshData> ModelManager::ParseMeshes(const aiScene* scene){
//    std::vector<MeshData> meshes;
//
//    for(uint32_t meshIdx = 0; meshIdx < scene->mNumMeshes; ++meshIdx) {
//        aiMesh* mesh = scene->mMeshes[meshIdx];
//        MeshData meshData;
//
//        assert(mesh->HasTextureCoords(0));   // UV座標必須
//
//        // faceを解析する
//        for(uint32_t faceIdx = 0; faceIdx < mesh->mNumFaces; ++faceIdx){
//
//            aiFace& face = mesh->mFaces[faceIdx];
//            assert(face.mNumIndices == 3);// 三角形以外は
//
//            // vertexを解析する
//            for(uint32_t element = 0; element < face.mNumIndices; ++element){
//
//                // 頂点の情報を取得
//                uint32_t vertexIdx = face.mIndices[element];
//                aiVector3D position = mesh->mVertices[vertexIdx];
//                aiVector3D normal = mesh->mNormals[vertexIdx];
//                aiVector3D texcoord = mesh->mTextureCoords[0][vertexIdx];
//
//                // 頂点を格納
//                VertexData vertex;
//                vertex.position_ = { position.x,position.y,position.z,1.0f };
//                vertex.normal_ = { normal.x,normal.y,normal.z };
//                vertex.texcoord_ = { texcoord.x,texcoord.y };
//
//                // 左手座標系に変換
//                vertex.position_.x *= -1.0f;
//                vertex.normal_.x *= -1.0f;
//
//                // 頂点を格納
//                meshData.vertices.push_back(vertex);
//            }
//
//
//            //// インデックスデータを構築
//            for(uint32_t i = 0; i < 3; ++i) {
//                meshData.indices.push_back(face.mIndices[i]);
//            }
//
//            // マテリアルインデックスを設定
//            meshData.materialIndex = mesh->mMaterialIndex;
//        }
//
//        // メッシュを追加
//        meshes.push_back(meshData);
//    }
//
//    return meshes;
//}

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


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// アニメーションを解析
///////////////////////////////////////////////////////////////////////////////////////////////////////////

Animation ModelManager::LoadAnimation(const std::string& directoryPath, const std::string& filename){

    // アニメーションデータの格納用
    Animation animation;

    // assinmpのインポート設定
    Assimp::Importer importer;
    std::string filePath = directoryPath + "/" + filename.substr(0, filename.find_last_of('.')) + "/" + filename;
    const aiScene* scene = importer.ReadFile(filePath.c_str(),0);
    assert(scene->HasAnimations());// animationがない場合はアサート

    // アニメーションの解析を行っていく
    aiAnimation* aiAnimation = scene->mAnimations[0];
    animation.name = aiAnimation->mName.C_Str();// アニメーション名を設定
    animation.duration = float(aiAnimation->mDuration / aiAnimation->mTicksPerSecond);// アニメーションの長さを秒単位に設定


    for(uint32_t channelIdx = 0; channelIdx < aiAnimation->mNumChannels; ++channelIdx) {
        aiNodeAnim* aiNodeAnim = aiAnimation->mChannels[channelIdx];
        NodeAnimation nodeAnimation;

        // 位置情報の解析
        for(uint32_t keiIdx = 0; keiIdx < aiNodeAnim->mNumPositionKeys; ++keiIdx) {
            aiVectorKey key = aiNodeAnim->mPositionKeys[keiIdx];
            Keyframe<Vector3> frame;
            frame.time = float(key.mTime / aiAnimation->mTicksPerSecond);
            frame.value = Vector3(key.mValue.x, key.mValue.y, key.mValue.z);
            nodeAnimation.translate.keyframes.push_back(frame);
        }

        // 回転情報の解析
        for(uint32_t keyIdx = 0; keyIdx < aiNodeAnim->mNumRotationKeys; ++keyIdx) {
            aiQuatKey key = aiNodeAnim->mRotationKeys[keyIdx];
            Keyframe<Quaternion> frame;
            frame.time = float(key.mTime / aiAnimation->mTicksPerSecond);
            frame.value = Quaternion(key.mValue.w, key.mValue.x, key.mValue.y, key.mValue.z);
            nodeAnimation.rotate.keyframes.push_back(frame);
        }

        // スケール情報の解析
        for(uint32_t keyIdx = 0; keyIdx < aiNodeAnim->mNumScalingKeys; ++keyIdx) {
            aiVectorKey key = aiNodeAnim->mScalingKeys[keyIdx];
            Keyframe<Vector3> frame;
            frame.time = float(key.mTime / aiAnimation->mTicksPerSecond);
            frame.value = Vector3(key.mValue.x, key.mValue.y, key.mValue.z);
            nodeAnimation.scale.keyframes.push_back(frame);
        }

        animation.nodeAnimations.push_back(nodeAnimation);
    }

    return animation;
}
