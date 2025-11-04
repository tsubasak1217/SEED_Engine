#include <SEED/Source/Manager/ModelManager/ModelManager.h>
#include <SEED/Lib/Functions/MatrixFunc.h>
#include <SEED/Source/Manager/DxManager/ViewManager.h>
#include <SEED/Source/Manager/TextureManager/TextureManager.h>
#include <SEED/Lib/Functions/MyMath.h>
#include <SEED/Lib/Functions/MyFunc.h>
#include <d3dx12.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <execution>
#include <algorithm>

// static変数初期化
ModelManager* ModelManager::instance_ = nullptr;
const std::string ModelManager::directoryPath_ = "Resources/Models/";

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                        //
//                                               初期化・終了関数                                                            //
//                                                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
    //StartUpLoad();

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                        //
//                                        その他外部から呼び出せる関数                                                         //
//                                                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 起動時に読み込みたいモデルをここで読み込む
void ModelManager::StartUpLoad(){
}

void ModelManager::LoadModel(const std::string& filename){

    // すでに読み込み済みのファイルであればreturn
    if(instance_->modelData_.find(filename) != instance_->modelData_.end()){ return; }

    // "Resources/"の階層からのパスならdirを空にする
    std::string dir = instance_->directoryPath_;
    std::string path = filename;
    if(path.starts_with("Resources")){
        dir = "";
    }

    // 読み込み
    instance_->modelData_[path] = instance_->LoadModelFile(dir, path);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                        //
//                                              モデルファイルを読み込む                                                      //
//                                                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ModelData* ModelManager::LoadModelFile(const std::string& directoryPath, const std::string& filename){

    // assinmpのインポート設定
    Assimp::Importer importer;
    std::filesystem::path fullPath = MyFunc::ToFullPath(directoryPath + filename);
    const aiScene* scene = importer.ReadFile(
        fullPath.generic_string().c_str(),
        // 三角形反転・UV反転・自動三角形化
        aiProcess_FlipWindingOrder | aiProcess_FlipUVs | aiProcess_Triangulate | aiProcess_GenSmoothNormals
    );

    assert(scene->HasMeshes());// メッシュがない場合はアサート

    // 結果格納用
    ModelData* modelData = new ModelData();
    // メッシュデータの読み込み
    modelData->meshes = ParseMeshes(scene);
    // メッシュレットの作成
    CreateMeshlet(modelData);
    // マテリアルデータの読み込み
    modelData->materials = ParseMaterials(scene, filename);
    modelData->modelName = filename;
    // アニメーションデータの読み込み
    modelData->animations = LoadAnimation(directoryPath, filename);
    // ルートノードの読み込み
    modelData->rootNode = ReadModelNode(scene->mRootNode);
    // スケルトンの作成
    modelData->defaultSkeleton = CreateSkeleton(modelData->rootNode);
    // ジョイントのウェイトの作成
    modelData->jointWeightData = CreateJointWeightData(scene);
    // 頂点の影響情報の作成
    CreateVertexInfluence(modelData->defaultSkeleton, modelData);

    return modelData;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                        //
//                                                   メッシュデータ関連                                                      //
//                                                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

    for(uint32_t meshIdx = 0; meshIdx < scene->mNumMeshes; ++meshIdx){
        aiMesh* mesh = scene->mMeshes[meshIdx];
        MeshData meshData;
        bool hasTexcoords_ = mesh->HasTextureCoords(0);

        // 頂点を元ファイルの順番で格納
        for(uint32_t vertexIdx = 0; vertexIdx < mesh->mNumVertices; ++vertexIdx){
            aiVector3D position = mesh->mVertices[vertexIdx];
            aiVector3D normal = mesh->mNormals[vertexIdx];
            aiVector3D texcoord = { 0.0f, 0.0f, 0.0f };
            if(hasTexcoords_){
                texcoord = mesh->mTextureCoords[0][vertexIdx];
            }

            // 頂点を作成
            VertexData vertex;
            vertex.position_ = { position.x, position.y, position.z, 1.0f };
            vertex.normal_ = { normal.x, normal.y, normal.z };
            vertex.texcoord_ = { texcoord.x, texcoord.y };

            // 左手座標系に変換
            vertex.position_.x *= -1.0f;
            vertex.normal_.x *= -1.0f;

            // 頂点をそのまま格納
            meshData.vertices.push_back(vertex);
        }

        // インデックスを格納
        for(uint32_t faceIdx = 0; faceIdx < mesh->mNumFaces; ++faceIdx){
            aiFace& face = mesh->mFaces[faceIdx];
            assert(face.mNumIndices == 3); // 三角形以外は無視

            // インデックスデータを追加
            for(uint32_t element = 0; element < face.mNumIndices; ++element){
                meshData.indices.push_back(face.mIndices[element]);
            }
        }

        // マテリアルインデックスを設定
        meshData.materialIndex = mesh->mMaterialIndex;

        // メッシュを追加
        meshes.push_back(meshData);
    }

    return meshes;
}


//=============================================
// メッシュレットの作成
//=============================================
void ModelManager::CreateMeshlet(ModelData* modelData){

    uint32_t kMaxVertexCount = 64;
    uint32_t kMaxIndexCount = 128;

    for(auto& mesh : modelData->meshes){

        uint32_t curIndex = 0;

        while(true){
            // ユニークな頂点リスト
            std::unordered_set<uint32_t> vertexDict;
            // indexの合計値
            uint32_t indexCount = 0;

            // 終了チェック
            if(curIndex >= mesh.indices.size()){
                break;
            }

            // meshletが切り替わるインデックスを記録
            mesh.meshletSwitchIndices.push_back(mesh.meshletCount);

            for(uint32_t index = curIndex; index < mesh.indices.size(); index += 3){
                // インデックスバッファの中身（頂点番号）をセットに入れる
                vertexDict.insert(mesh.indices[index]);
                vertexDict.insert(mesh.indices[index + 1]);
                vertexDict.insert(mesh.indices[index + 2]);

                curIndex += 3;
                indexCount += 3;

                if(vertexDict.size() >= kMaxVertexCount || indexCount >= kMaxIndexCount){
                    break;
                }
            }

            // メッシュレットの数を増やす
            mesh.meshletCount++;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                        //
//                                                 マテリアルデータ関連                                                      //
//                                                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<ModelMaterialLoadData> ModelManager::ParseMaterials(const aiScene* scene, const std::string& modelName){
    std::vector<ModelMaterialLoadData> materials;

    // マテリアルを読み込む
    for(uint32_t materialIdx = 0; materialIdx < scene->mNumMaterials; ++materialIdx){
        aiMaterial* material = scene->mMaterials[materialIdx];
        ModelMaterialLoadData materialData;

        aiString texturePath;
        if(material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS){
            // ファイル名から種類を判別し種類に応じた処理を行う
            std::string textureFilePath = texturePath.C_Str();

            // '*'が含まれていなければ通常のテクスチャファイルとみなす
            if(textureFilePath.find("*") == std::string::npos){
                std::string findResult;
                // まずmodel階層内から探す
                findResult = MyFunc::FindFile("Resources/models/", texturePath.C_Str());
                if(!findResult.empty()){
                    materialData.textureFilePath_ = "../models/" + findResult;

                } else{// 見つからなかったらTextures階層から探す
                    findResult = MyFunc::FindFile("Resources/Textures/", texturePath.C_Str());
                    if(!findResult.empty()){
                        materialData.textureFilePath_ = findResult;
                    }
                }

            } else{// 埋め込みテクスチャ
                // "*"より後の文字列を取得
                std::string texIndexStr = textureFilePath.substr(textureFilePath.find("*") + 1);
                materialData.textureFilePath_ = modelName + texIndexStr;
                uint32_t texIndex = std::stoi(texIndexStr);

                // 埋め込みテクスチャを取得し読み込む
                const aiTexture* embeddedTexture = scene->mTextures[texIndex];
                TextureManager::LoadTexture(materialData.textureFilePath_, embeddedTexture);
            }
        }

        // テクスチャがない場合は白テクスチャを設定
        if(materialData.textureFilePath_ == ""){
            materialData.textureFilePath_ = "DefaultAssets/white1x1.png";
        } else{
            // 埋め込みテクスチャの場合(最後の'/'の次が'*'の場合)、aiTextureを設定
            if(materialData.textureFilePath_.find("/*") != std::string::npos){

            }
        }

        // 色の設定
        aiColor4D baseColor;
        if(material->Get(AI_MATKEY_BASE_COLOR, baseColor) == AI_SUCCESS){
            materialData.color_ = Vector4(baseColor.r, baseColor.g, baseColor.b, baseColor.a);

        } else{
            materialData.color_ = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
        }

        materials.push_back(materialData);
    }

    // メッシュごとのマテリアル割り当て
    std::vector<ModelMaterialLoadData> meshMaterials;
    for(uint32_t meshIdx = 0; meshIdx < scene->mNumMeshes; ++meshIdx){
        aiMesh* mesh = scene->mMeshes[meshIdx];
        uint32_t materialIndex = mesh->mMaterialIndex;

        // マテリアルが存在する場合は対応するものを割り当て
        if(materialIndex < materials.size()){
            meshMaterials.push_back(materials[materialIndex]);
        } else{
            // マテリアルが対応していない場合はデフォルトマテリアルを割り当て
            ModelMaterialLoadData defaultMaterial;
            defaultMaterial.textureFilePath_ = "DefaultAssets/white1x1.png";
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                        //
//                                                  アニメーション関連                                                       //
//                                                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*--------------------------------------*/
// アニメーションデータの解析
/*--------------------------------------*/
std::unordered_map<std::string, ModelAnimation> ModelManager::LoadAnimation(const std::string& directoryPath, const std::string& filename){

    // アニメーションデータの格納用
    std::unordered_map<std::string, ModelAnimation> result;

    // assinmpのインポート設定
    Assimp::Importer importer;
    std::filesystem::path fullPath = MyFunc::ToFullPath(directoryPath + filename);
    const aiScene* scene = importer.ReadFile(fullPath.generic_string().c_str(), 0);
    if(!scene->HasAnimations()){ return result; }// animationがない場合は終了

    // アニメーションの解析を行っていく
    for(uint32_t animIdx = 0; animIdx < scene->mNumAnimations; ++animIdx){

        aiAnimation* anim = scene->mAnimations[animIdx];
        ModelAnimation animation;

        // アニメーションの長さを取得
        assert(anim->mTicksPerSecond != 0);// 0であればアサート
        animation.duration = static_cast<float>(anim->mDuration / anim->mTicksPerSecond);

        // ノードごとのアニメーションを解析
        for(uint32_t channelIdx = 0; channelIdx < anim->mNumChannels; ++channelIdx){
            aiNodeAnim* channel = anim->mChannels[channelIdx];
            NodeAnimation nodeAnim;

            // 位置アニメーションを解析
            if(channel->mNumPositionKeys > 0){
                for(uint32_t keyIdx = 0; keyIdx < channel->mNumPositionKeys; ++keyIdx){
                    aiVectorKey key = channel->mPositionKeys[keyIdx];
                    Keyframe<Vector3> keyframe;
                    keyframe.time = static_cast<float>(key.mTime / anim->mTicksPerSecond);
                    keyframe.value = Vector3(-key.mValue.x, key.mValue.y, key.mValue.z);
                    nodeAnim.translate.keyframes.push_back(keyframe);
                }
            }

            // 回転アニメーションを解析
            if(channel->mNumRotationKeys > 0){
                for(uint32_t keyIdx = 0; keyIdx < channel->mNumRotationKeys; ++keyIdx){
                    aiQuatKey key = channel->mRotationKeys[keyIdx];
                    Keyframe<Quaternion> keyframe;
                    keyframe.time = static_cast<float>(key.mTime / anim->mTicksPerSecond);
                    keyframe.value = Quaternion(key.mValue.x, -key.mValue.y, -key.mValue.z, key.mValue.w);
                    nodeAnim.rotate.keyframes.push_back(keyframe);
                }
            }

            // スケールアニメーションを解析
            if(channel->mNumScalingKeys > 0){
                for(uint32_t keyIdx = 0; keyIdx < channel->mNumScalingKeys; ++keyIdx){
                    aiVectorKey key = channel->mScalingKeys[keyIdx];
                    Keyframe<Vector3> keyframe;
                    keyframe.time = static_cast<float>(key.mTime / anim->mTicksPerSecond);
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


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                        //
//                                                   ノードデータ関連                                                       //
//                                                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*-----------------------------------------------------------*/
// ノードデータの読み込み
/*-----------------------------------------------------------*/
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
    result.transform.scale = Vector3(aiScale.x, aiScale.y, aiScale.z);
    result.transform.rotate = Quaternion(aiRotate.x, -aiRotate.y, -aiRotate.z, aiRotate.w);
    result.transform.translate = Vector3(-aiTranslate.x, aiTranslate.y, aiTranslate.z);
    result.localMatrix = AffineMatrix(result.transform.scale, result.transform.rotate, result.transform.translate);

    // ノード名を取得
    result.name = node->mName.C_Str();

    // 子ノードを再帰的に処理
    for(uint32_t i = 0; i < node->mNumChildren; ++i){
        result.children.push_back(ReadModelNode(node->mChildren[i]));
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                        //
//                                                    スケルトン関連                                                        //
//                                                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*-----------------------------------------------------------*/
// スケルトン作成のためにノードからジョイントを作成
/*-----------------------------------------------------------*/
int32_t ModelManager::CreateJoint(
    const ModelNode& node,
    const std::optional<int32_t>& parent,
    std::vector<ModelJoint>& joints
){
    ModelJoint joint;
    joint.name = node.name;
    joint.localMatrix = node.localMatrix;
    joint.skeletonSpaceMatrix = node.localMatrix;
    joint.skeletonSpaceMatrix = IdentityMat4();
    joint.transform = node.transform;
    joint.index = static_cast<int32_t>(joints.size());
    joint.parent = parent;
    joints.push_back(joint);

    for(const ModelNode& child : node.children){
        // 子ジョイントを作成しindexを登録
        int32_t childIndex = CreateJoint(child, joint.index, joints);
        joints[joint.index].children.push_back(childIndex);
    }

    return joint.index;
}


/*-----------------------------------------------------------*/
// スケルトンの作成
/*-----------------------------------------------------------*/
ModelSkeleton ModelManager::CreateSkeleton(const ModelNode& rootNode){
    ModelSkeleton skeleton;
    skeleton.rootIndex = CreateJoint(rootNode, std::nullopt, skeleton.joints);

    for(const ModelJoint& joint : skeleton.joints){
        skeleton.jointMap.emplace(joint.name, joint.index);
    }

    UpdateSkeleton(&skeleton);

    return skeleton;
}

/*-----------------------------------------------------------*/
// スケルトンにアニメーションを適用
/*-----------------------------------------------------------*/
void ModelManager::AnimatedSkeleton(
    const ModelAnimation& modelAnimation, const ModelSkeleton& defaultSkeleton, ModelSkeleton* pSkeleton, float time
){
    pSkeleton->joints.resize(defaultSkeleton.joints.size());

    // 指定した時間の値を取得
    std::for_each(std::execution::par_unseq, defaultSkeleton.joints.begin(), defaultSkeleton.joints.end(), [&](auto& defaultJoint){

        uint32_t idx = defaultJoint.index;

        if(auto it = modelAnimation.nodeAnimations.find(defaultJoint.name); it != modelAnimation.nodeAnimations.end()){

            // ノードアニメーションを取得
            const NodeAnimation& nodeAnim = it->second;
            // 位置アニメーションを適用
            Vector3 translate = CalcMomentValue(nodeAnim.translate.keyframes, time);
            // 回転アニメーションを適用
            Quaternion rotate = CalcMomentValue(nodeAnim.rotate.keyframes, time);
            // スケールアニメーションを適用
            Vector3 scale = CalcMomentValue(nodeAnim.scale.keyframes, time);

            // トランスフォーム情報を更新
            pSkeleton->joints[idx].transform.translate = translate;
            pSkeleton->joints[idx].transform.rotate = rotate;
            pSkeleton->joints[idx].transform.scale = scale;
        } else{
            // ノードアニメーションがない場合はデフォルトの値を設定
            pSkeleton->joints[idx].transform = defaultJoint.transform;
            pSkeleton->joints[idx].skeletonSpaceMatrix = IdentityMat4();
        }

        pSkeleton->joints[idx].parent = defaultJoint.parent;
        //  indexを設定
        pSkeleton->joints[idx].index = idx;
    });

    // スケルトン行列を更新
    instance_->UpdateSkeleton(pSkeleton);
}

// スケルトンの補間
ModelSkeleton ModelManager::InterpolateSkeleton(
    const ModelSkeleton& skeletonA, const ModelSkeleton& skeletonB, float t
){
    // 違うジョイント数の場合は補間できない
    if(skeletonA.joints.size() != skeletonB.joints.size()){
        assert(false);
    }

    // ジョイントごとに補間
    ModelSkeleton skeleton;

    for(uint32_t i = 0; i < skeletonA.joints.size(); ++i){
        ModelJoint jointA = skeletonA.joints[i];
        ModelJoint jointB = skeletonB.joints[i];

        // 補間
        Quaternion rotate = Quaternion::Slerp(jointA.transform.rotate, jointB.transform.rotate, t);
        Vector3 translate = MyMath::Lerp(jointA.transform.translate, jointB.transform.translate, t);
        Vector3 scale = MyMath::Lerp(jointA.transform.scale, jointB.transform.scale, t);

        // 補間したジョイントを作成
        ModelJoint joint;
        joint.transform.rotate = rotate;
        joint.transform.translate = translate;
        joint.transform.scale = scale;
        joint.localMatrix = AffineMatrix(scale, rotate, translate);
        joint.skeletonSpaceMatrix = joint.localMatrix;
        joint.index = i;
        joint.parent = jointA.parent;
        joint.name = jointA.name;

        skeleton.joints.push_back(joint);
    }

    // スケルトン行列を更新
    instance_->UpdateSkeleton(&skeleton);

    return skeleton;
}

/*-----------------------------------------------------------*/
// スケルトン行列の更新
/*-----------------------------------------------------------*/
void ModelManager::UpdateSkeleton(ModelSkeleton* skeleton){

    std::for_each(std::execution::par_unseq, skeleton->joints.begin(), skeleton->joints.end(), [&](ModelJoint& joint){
        // ローカル行列を更新
        joint.localMatrix = AffineMatrix(joint.transform.scale, joint.transform.rotate, joint.transform.translate);
    });

    for(ModelJoint& joint : skeleton->joints){
        if(joint.parent){
            joint.skeletonSpaceMatrix = joint.localMatrix * skeleton->joints[*joint.parent].skeletonSpaceMatrix;
        } else{
            joint.skeletonSpaceMatrix = joint.localMatrix;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                        //
//                                                 スキンクラスタ関連                                                        //
//                                                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*-----------------------------------------------------------*/
// スキンクラスタのためにジョイントのウェイトを解析
/*-----------------------------------------------------------*/
std::unordered_map<std::string, JointWeightData> ModelManager::CreateJointWeightData(const aiScene* scene){

    std::unordered_map<std::string, JointWeightData> result;
    int32_t vertexCount = 0;

    for(uint32_t meshIdx = 0; meshIdx < scene->mNumMeshes; ++meshIdx){
        aiMesh* mesh = scene->mMeshes[meshIdx];

        if(mesh->mNumBones == 0){
            // メッシュに対応するノード名を取得
            aiNode* meshNode = scene->mRootNode->FindNode(mesh->mName);
            if(meshNode){
                std::string nodeName = meshNode->mName.C_Str();
                JointWeightData& jointWeightData = result[nodeName];

                // ノードの変換行列を取得し、疑似バインドポーズ行列として格納
                aiMatrix4x4 bindPoseMatrixAssimp = meshNode->mTransformation.Inverse();
                aiVector3D scale, translate;
                aiQuaternion rotate;
                bindPoseMatrixAssimp.Decompose(scale, rotate, translate);

                Matrix4x4 bindPoseMatrix = AffineMatrix(
                    { scale.x, scale.y, scale.z },
                    { rotate.x, -rotate.y, -rotate.z, rotate.w },
                    { -translate.x, translate.y, translate.z }
                );
                jointWeightData.inverseBindPoseMatrix = InverseMatrix(bindPoseMatrix);

                // 各頂点に均一なウェイトを付与
                for(uint32_t vertexIdx = 0; vertexIdx < mesh->mNumVertices; ++vertexIdx){
                    jointWeightData.vertexWeights.push_back(
                        VertexWeightData(1.0f, vertexCount + vertexIdx)
                    );
                }
            } else{

                for(uint32_t animIndex = 0; animIndex < scene->mNumAnimations; ++animIndex){
                    aiAnimation* animation = scene->mAnimations[animIndex];

                    for(uint32_t channelIndex = 0; channelIndex < animation->mNumChannels; ++channelIndex){
                        aiNodeAnim* channel = animation->mChannels[channelIndex];
                        std::string nodeName = channel->mNodeName.C_Str();
                        JointWeightData& jointWeightData = result[nodeName];

                        // デフォルトの逆バインドポーズ行列を生成
                        aiMatrix4x4 bindPoseMatrixAssimp; // 単位行列
                        aiVector3D scale(1.0f, 1.0f, 1.0f), translate(0.0f, 0.0f, 0.0f);
                        aiQuaternion rotate(1.0f, 0.0f, 0.0f, 0.0f);

                        bindPoseMatrixAssimp.Decompose(scale, rotate, translate);
                        Matrix4x4 bindPoseMatrix = AffineMatrix(
                            { scale.x, scale.y, scale.z },
                            { rotate.x, -rotate.y, -rotate.z, rotate.w },
                            { -translate.x, translate.y, translate.z }
                        );
                        jointWeightData.inverseBindPoseMatrix = InverseMatrix(bindPoseMatrix);

                        // ウェイトの生成（全頂点に均等に割り当てる）
                        for(uint32_t vertexIdx = 0; vertexIdx < mesh->mNumVertices; ++vertexIdx){
                            jointWeightData.vertexWeights.push_back(
                                VertexWeightData(1.0f, vertexCount + vertexIdx)
                            );
                        }
                    }
                }
            }

        } else{
            for(uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex){

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

                for(uint32_t weightIndex = 0; weightIndex < bone->mNumWeights; ++weightIndex){
                    jointWeightData.vertexWeights.push_back(
                        VertexWeightData(
                            bone->mWeights[weightIndex].mWeight,
                            vertexCount + bone->mWeights[weightIndex].mVertexId
                        )
                    );
                }
            }
        }

        vertexCount += mesh->mNumVertices;
    }

    return result;
}

/*-----------------------------------------------------------*/
// モデルの全頂点分のweight影響データの作成
/*-----------------------------------------------------------*/
void ModelManager::CreateVertexInfluence(const ModelSkeleton& skeleton, ModelData* modelData){

    // InverrseBindPoseMatrixを格納する場所を作成し、単位行列で初期化
    auto& palette = modelData->defaultSkinClusterData;
    palette.inverseBindPoseMatrices.resize(skeleton.joints.size(), IdentityMat4());

    // meshごとのvertexInfluence格納場所を作成
    int32_t vertexCount = 0;
    for(auto& mesh : modelData->meshes){
        mesh.vertexInfluences.resize(mesh.vertices.size());
        vertexCount += (int32_t)mesh.vertices.size();
    }

    // 全頂点分の影響情報を格納する場所を作成(あとでmeshごとに分ける)
    std::vector<VertexInfluence*> influences;
    influences.resize(vertexCount);
    for(auto& influence : influences){
        influence = new VertexInfluence();
        std::fill(std::begin(influence->weights), std::end(influence->weights), 0.0f);
        std::fill(std::begin(influence->jointIndices), std::end(influence->jointIndices), 0);
    }

    // modelDataを解析して、Influenceを作成
    for(const auto& jointWeight : modelData->jointWeightData){// ModelDataのweightDataを解析

        auto it = skeleton.jointMap.find(jointWeight.first);// 対象のjointが存在するか確認
        if(it == skeleton.jointMap.end()){
            continue;// jointが存在しない場合はスキップ
        }

        palette.inverseBindPoseMatrices[(*it).second] = jointWeight.second.inverseBindPoseMatrix;// InverseBindPoseMatrixを格納

        for(const auto& vertexWeight : jointWeight.second.vertexWeights){// 頂点ウェイトを格納

            if(vertexWeight.vertexIndex > vertexCount){
                continue;
            }

            // 格納場所を取得
            auto& currentInfluence = influences[vertexWeight.vertexIndex];

            // ウェイト、インデックス情報を格納していく
            for(uint32_t index = 0; index < kMaxInfluence; ++index){// 空いている場所に格納
                if(currentInfluence->weights[index] == 0.0f){// weight == 0 が空の状態なのでその場所にweightとindexを格納
                    currentInfluence->weights[index] = vertexWeight.weight;
                    currentInfluence->jointIndices[index] = it->second;
                    break;
                }
            }
        }
    }

    // influencesをmeshごとに分ける
    int32_t vertexIndex = 0;
    for(auto& mesh : modelData->meshes){
        for(auto& vertexInfluence : mesh.vertexInfluences){
            vertexInfluence = *influences[vertexIndex];
            vertexIndex++;
        }
    }

    // 解放
    for(auto& influence : influences){
        delete influence;
        influence = nullptr;
    }
}


