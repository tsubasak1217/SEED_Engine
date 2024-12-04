#include "Model.h"
#include "MatrixFunc.h"
#include "SEED.h"
#include "ModelManager.h"

Model::Model(const std::string& filename){
    Initialize(filename);
}

void Model::Initialize(const std::string& filename){
    modelName_ = filename;
    ModelManager::LoadModel(modelName_);
    auto modelData = ModelManager::GetModelData(modelName_);

    scale_ = { 1.0f,1.0f,1.0f };
    rotate_ = { 0.0f,0.0f,0.0f };
    translate_ = { 0.0f,0.0f,0.0f };
    worldMat_ = IdentityMat4();

    // マテリアルの数だけ初期化
    for(int i = 0; i < modelData->materials.size(); i++){

        uv_scale_.push_back(modelData->materials[i].UV_scale_);
        uv_rotate_.push_back({ 0.0f,0.0f,0.0f });
        uv_translate_.push_back(
            modelData->materials[i].UV_offset_ +
            modelData->materials[i].UV_translate_
        );

        uvTransform_.push_back(AffineMatrix(uv_scale_.back(), uv_rotate_.back(), uv_translate_.back()));
        textureGH_.push_back(
            TextureManager::LoadTexture(modelData->materials[i].textureFilePath_)
        );
    }

    color_ = { 1.0f,1.0f,1.0f,1.0f };
    lightingType_ = LIGHTINGTYPE_HALF_LAMBERT;
}

void Model::Draw(){
    SEED::DrawModel(this);
}


void Model::UpdateMatrix(){

    // ワールド変換行列の更新
    if(isRotateWithQuaternion_){
        worldMat_ = AffineMatrix(scale_, rotateQuat_, translate_);
        rotate_ = Quaternion::ToEuler(rotateQuat_);// 切り替えても大丈夫なように同期させておく
    } else{
        worldMat_ = AffineMatrix(scale_, rotate_, translate_);
        rotateQuat_ = Quaternion::EulerToQuaternion(rotate_);// 切り替えても大丈夫なように同期させておく
    }

    // UV変換行列の更新
    for(int i = 0; i < uvTransform_.size(); i++){
        uvTransform_[i] = AffineMatrix(uv_scale_[i], uv_rotate_[i], uv_translate_[i]);
    }

    // 親のワールド変換行列を掛ける
    if(parent_){
        worldMat_ = Multiply(worldMat_, parent_->worldMat_);
    }
}

void Model::PlayAnimation(const std::string& animationName,bool loop, float speedRate){

}
