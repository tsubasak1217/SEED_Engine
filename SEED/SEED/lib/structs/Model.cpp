#include "Model.h"
#include "MatrixFunc.h"
#include "SEED.h"
#include "ModelManager.h"

Model::Model(const std::string& filename){
    Initialize(filename);
}

void Model::Initialize(const std::string& filename)
{
    modelName_ = filename;
    ModelManager::LoadModel(modelName_);

    scale_ = { 1.0f,1.0f,1.0f };
    rotation_ = { 0.0f,0.0f,0.0f };
    translation_ = { 0.0f,0.0f,0.0f };
    worldMat_ = IdentityMat4();

    uv_scale_ = ModelManager::GetModelData(modelName_)->materialData.UV_scale_;
    uv_rotation_ = { 0.0f,0.0f,0.0f };
    uv_translation_ = 
        ModelManager::GetModelData(modelName_)->materialData.UV_offset_ +
        ModelManager::GetModelData(modelName_)->materialData.UV_translate_;
    uvTransform_ = AffineMatrix(uv_scale_,uv_rotation_,uv_translation_);

    textureGH_ = SEED::LoadTexture(ModelManager::GetModelData(modelName_)->materialData.textureFilePath_);
    colorf_ = { 1.0f,1.0f,1.0f,1.0f };
    lightingType_ = LIGHTINGTYPE_HALF_LAMBERT;
}

void Model::Draw(){
    SEED::DrawModel(this);
}

void Model::UpdateMatrix(){
    worldMat_ = AffineMatrix(scale_, rotation_, translation_);
    uvTransform_ = AffineMatrix(uv_scale_, uv_rotation_, uv_translation_);

    if (parent_){
        worldMat_ = Multiply(worldMat_, parent_->worldMat_);
    }
}
