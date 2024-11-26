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
    LoadModel("skydome.obj");
    LoadModel("ground.obj");
}

void ModelManager::LoadModel(const std::string& filename){
    // すでに読み込み済みのファイルであればreturn
    if(instance_->modelData_.find(filename) != instance_->modelData_.end()){ return; }

    // 読み込み
    instance_->modelData_[filename] = instance_->LoadObjFile(instance_->directoryPath_, filename);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// モデルファイルを読み込む
///////////////////////////////////////////////////////////////////////////////////////////////////////////

ModelData* ModelManager::LoadObjFile(const std::string& directoryPath, const std::string& filename){

    // assinmpのインポート設定
    Assimp::Importer importer;
    std::string filePath = directoryPath + "/" + filename.substr(0, filename.find_last_of('.')) + "/" + filename;
    const aiScene* scene = importer.ReadFile(
        filePath.c_str(),
        // 三角形反転・UV反転・自動三角形化
        aiProcess_FlipWindingOrder | aiProcess_FlipUVs | aiProcess_Triangulate
    );

    assert(scene->HasMeshes());// メッシュがない場合はアサート

    // ファイルを開く
    std::ifstream file(filePath);
    assert(file.is_open());// 失敗したらアサート

    // 結果を格納する変数
    ModelData* modelData = new ModelData();

    //================================================================//
    //                        meshを解析する
    //================================================================//
    for(uint32_t meshIdx = 0; meshIdx < scene->mNumMeshes; ++meshIdx){

        aiMesh* mesh = scene->mMeshes[meshIdx];
        MeshData meshData;
        assert(mesh->HasNormals());// 法線がない場合はアサート
        assert(mesh->HasTextureCoords(0));// テクスチャ座標がない場合はアサート

        // faceを解析する
        for(uint32_t faceIdx = 0; faceIdx < mesh->mNumFaces; ++faceIdx){

            aiFace& face = mesh->mFaces[faceIdx];
            assert(face.mNumIndices == 3);// 三角形以外はアサート

            for(uint32_t faceVertex = 0; faceVertex < 3; ++faceVertex){

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

                    // 頂点,頂点番号を格納
                    meshData.vertices.push_back(vertex);
                    meshData.indices.push_back((int32_t)meshData.indices.size());
                }
            }
        }

        // メッシュを追加
        modelData->meshes.push_back(meshData);
    }

    //================================================================//
    //                        materialを解析
    //================================================================//

    for(uint32_t materialIdx = 0; materialIdx < scene->mNumMaterials; ++materialIdx) {

        aiMaterial* material = scene->mMaterials[materialIdx];

        if(material->GetTextureCount(aiTextureType_DIFFUSE) != 0){
            aiString texturefilePath;
            material->GetTexture(aiTextureType_DIFFUSE, 0, &texturefilePath);

        }

        // テクスチャの読み込み
        modelData->materials.push_back(LoadMaterialTemplateFile(
            directoryPath + "/" + filename.substr(0, filename.find_last_of('.')),
            filename.substr(0, filename.find_last_of('.')) + ".mtl"
        ));
    }

    modelData->modelName = filename;
    return modelData;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////
// マテリアルファイルを読み込む
///////////////////////////////////////////////////////////////////////////////////////////////////////////

MaterialData ModelManager::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename){

    // ファイルを開く
    std::ifstream file(directoryPath + "/" + filename);
    assert(file.is_open());// 失敗したらアサート

    MaterialData materialData;
    std::string line;

    while(std::getline(file, line)){

        // まずobjファイルの行の先頭の識別子を読む
        std::string identifer;
        std::istringstream s(line);
        s >> identifer;

        if(identifer == "map_Kd"){// ファイル名

            std::string textureFilename;
            Vector3 scale = { 1.0f,1.0f,1.0f };
            Vector3 offset = { 0.0f,0.0f,0.0f };
            Vector3 translate = { 0.0f,0.0f,0.0f };

            // ファイル名を格納
            while(s >> textureFilename) {
                if(textureFilename[0] == '-') {
                    std::string option = textureFilename.substr(1);
                    if(option == "s") {
                        s >> scale.x >> scale.y >> scale.z;
                    } else if(option == "o") {
                        s >> offset.x >> offset.y >> offset.z;
                    } else if(option == "t") {
                        s >> translate.x >> translate.y >> translate.z;
                    }
                } else {
                    materialData.textureFilePath_ = textureFilename;
                }
            }

            materialData.UV_scale_ = scale;
            materialData.UV_offset_ = offset;
            materialData.UV_translate_ = translate;
        }
    }

    // テクスチャなしのモデルの場合
    if(materialData.textureFilePath_ == ""){
        materialData.textureFilePath_ = "white1x1.png";
    }

    return materialData;
}
