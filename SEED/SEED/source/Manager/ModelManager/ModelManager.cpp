#include "ModelManager.h"
#include "MatrixFunc.h"

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
    // アニメーションデータの読み込み
    modelData->animations = LoadAnimation(directoryPath, filename);

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

    // ローカル行列を分解
    aiVector3D aiScale, aiTranslate;
    aiQuaternion aiRotate;
    node->mTransformation.Decompose(aiScale, aiRotate, aiTranslate);

    // 情報を格納(左手座標に変換して)
    result.transform.scale_ = Vector3(aiScale.x, aiScale.y, aiScale.z);
    result.transform.rotate_ = Quaternion(aiRotate.x, -aiRotate.y, -aiRotate.z, aiRotate.w);
    result.transform.translate_ = Vector3(-aiTranslate.x, aiTranslate.y, aiTranslate.z);
    result.localMatrix = AffineMatrix(result.transform.scale_, result.transform.rotate_, result.transform.translate_);

    // ノード名を取得
    result.name = node->mName.C_Str();

    // 子ノードを再帰的に処理
    for(uint32_t i = 0; i < node->mNumChildren; ++i) {
        result.children.push_back(ReadModelNode(node->mChildren[i]));
    }

    return result;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// ジョイントを作成
///////////////////////////////////////////////////////////////////////////////////////////////////////////

int32_t ModelManager::CreateJoint(
    const ModelNode& node,
    const std::optional<int32_t>& parent,
    std::vector<ModelJoint>&joints
){
    ModelJoint joint;
    joint.name = node.name;
    joint.localMatrix = node.localMatrix;
    joint.skeletonMatrix = node.localMatrix;
    joint.skeletonMatrix = IdentityMat4();
    joint.transform = node.transform;
    joint.index = static_cast<int32_t>(joints.size());
    joint.parent = parent;
    joints.push_back(joint);

    for(const ModelNode& child : node.children ){
        // 子ジョイントを作成しindexを登録
        int32_t childIndex = CreateJoint(child, joint.index, joints);
        joints[joint.index].children.push_back(childIndex);
    }

    return joint.index;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// スケルトン関連
///////////////////////////////////////////////////////////////////////////////////////////////////////////

// スケルトンの作成
ModelSkeleton ModelManager::CreateSkeleton(const ModelNode& rootNode){
    ModelSkeleton skeleton;
    skeleton.rootIndex = CreateJoint(rootNode, std::nullopt, skeleton.joints);

    for(const ModelJoint& joint : skeleton.joints){
        skeleton.jointMap.emplace(joint.name, joint.index);
    }

    return skeleton;
}

// スケルトン行列の更新
void ModelManager::UpdateSkeleton(ModelSkeleton& skeleton){
    for(ModelJoint& joint : skeleton.joints){
        joint.localMatrix = AffineMatrix(joint.transform.scale_, joint.transform.rotate_, joint.transform.translate_);
        if(joint.parent){
            joint.skeletonMatrix = joint.localMatrix * skeleton.joints[*joint.parent].skeletonMatrix;
        } else{
            joint.skeletonMatrix = joint.localMatrix;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// スキンクラスタを解析
///////////////////////////////////////////////////////////////////////////////////////////////////////////

std::unordered_map<std::string, JointWeightData> ModelManager::ParseSkinCluster(const aiMesh* mesh){
    std::unordered_map<std::string, JointWeightData> result;

    for(uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {

        //------------- jointごとの格納領域を作る -------------//

        aiBone* bone = mesh->mBones[boneIndex];
        std::string jointName = bone->mName.C_Str();
        JointWeightData& jointWeightData = result[jointName];

        //----------- InverseBindPoseMatrixの抽出 -----------//

        aiMatrix4x4 bindPoseMatrixAssimp = bone->mOffsetMatrix.Inverse();
        aiVector3D scale, translate;
        aiQuaternion rotate;
        bindPoseMatrixAssimp.Decompose(scale, rotate, translate);
        // バインドポーズ行列を左手座標系に変換し求める
        Matrix4x4 bindPoseMatrix = AffineMatrix(
            { scale.x, scale.y, scale.z },
            { rotate.x, -rotate.y, -rotate.z, rotate.w },
            { -translate.x, translate.y, translate.z }
        );
        // 逆バインドポーズ行列に変換し格納
        jointWeightData.inverseBindPoseMatrix = InverseMatrix(bindPoseMatrix);

        //--------------- Weight情報を取り出す ---------------//

        for(uint32_t weightIndex = 0; weightIndex < bone->mNumWeights; ++weightIndex) {
            jointWeightData.vertexWeights.push_back(
                VertexWeightData(bone->mWeights[weightIndex].mWeight, bone->mWeights[weightIndex].mVertexId)
            );
        }
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// アニメーションを解析
///////////////////////////////////////////////////////////////////////////////////////////////////////////

std::unordered_map<std::string, ModelAnimation> ModelManager::LoadAnimation(const std::string& directoryPath, const std::string& filename){

    // アニメーションデータの格納用
    std::unordered_map<std::string, ModelAnimation> result;

    // assinmpのインポート設定
    Assimp::Importer importer;
    std::string filePath = directoryPath + "/" + filename.substr(0, filename.find_last_of('.')) + "/" + filename;
    const aiScene* scene = importer.ReadFile(filePath.c_str(),0);
    if(!scene->HasAnimations()){ return result; }// animationがない場合は終了

    // アニメーションの解析を行っていく
    for(uint32_t animIdx = 0; animIdx < scene->mNumAnimations; ++animIdx) {

        aiAnimation* anim = scene->mAnimations[animIdx];
        ModelAnimation animation;

        // アニメーションの長さを取得
        assert(anim->mTicksPerSecond != 0);// 0であればアサート
        animation.duration = static_cast<float>(anim->mDuration / anim->mTicksPerSecond);

        // ノードごとのアニメーションを解析
        for(uint32_t channelIdx = 0; channelIdx < anim->mNumChannels; ++channelIdx) {
            aiNodeAnim* channel = anim->mChannels[channelIdx];
            NodeAnimation nodeAnim;

            // 位置アニメーションを解析
            if(channel->mNumPositionKeys > 0) {
                for(uint32_t keyIdx = 0; keyIdx < channel->mNumPositionKeys; ++keyIdx) {
                    aiVectorKey key = channel->mPositionKeys[keyIdx];
                    Keyframe<Vector3> keyframe;
                    keyframe.time = static_cast<float>(key.mTime);
                    keyframe.value = Vector3(-key.mValue.x, key.mValue.y, key.mValue.z);
                    nodeAnim.translate.keyframes.push_back(keyframe);
                }
            }

            // 回転アニメーションを解析
            if(channel->mNumRotationKeys > 0) {
                for(uint32_t keyIdx = 0; keyIdx < channel->mNumRotationKeys; ++keyIdx) {
                    aiQuatKey key = channel->mRotationKeys[keyIdx];
                    Keyframe<Quaternion> keyframe;
                    keyframe.time = static_cast<float>(key.mTime);
                    keyframe.value = Quaternion(key.mValue.x, -key.mValue.y, -key.mValue.z, key.mValue.w);
                    nodeAnim.rotate.keyframes.push_back(keyframe);
                }
            }

            // スケールアニメーションを解析
            if(channel->mNumScalingKeys > 0) {
                for(uint32_t keyIdx = 0; keyIdx < channel->mNumScalingKeys; ++keyIdx) {
                    aiVectorKey key = channel->mScalingKeys[keyIdx];
                    Keyframe<Vector3> keyframe;
                    keyframe.time = static_cast<float>(key.mTime);
                    keyframe.value = Vector3(key.mValue.x, key.mValue.y, key.mValue.z);
                    nodeAnim.scale.keyframes.push_back(keyframe);
                }
            }

            // ノード名をキーにしてアニメーションデータに追加(重複を確認して重複している名前があれば後ろに番号を割り当てる)
            if(animation.nodeAnimations.find(channel->mNodeName.C_Str()) == animation.nodeAnimations.end()){
                animation.nodeAnimations[channel->mNodeName.C_Str()] = nodeAnim;

            } else{
                int i = 1;
                while(true){
                    std::string nodeName = channel->mNodeName.C_Str() + std::to_string(i);
                    if(animation.nodeAnimations.find(nodeName) == animation.nodeAnimations.end()){
                        animation.nodeAnimations[nodeName] = nodeAnim;
                        break;
                    }
                    i++;
                }
            }
        }

        // アニメーションデータを追加(重複を確認して重複している名前があれば後ろに番号を割り当てる)
        if(result.find(anim->mName.C_Str()) == result.end()){
            result[anim->mName.C_Str()] = animation;
        } else{
            int i = 1;
            while(true){
                std::string animName = anim->mName.C_Str() + std::to_string(i);
                if(result.find(animName) == result.end()){
                    result[animName] = animation;
                    break;
                }
                i++;
            }
        }
    }


    return result;
}

