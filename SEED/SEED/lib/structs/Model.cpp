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
    rotate_ = { 0.0f,0.0f,0.0f };
    translate_ = { 0.0f,0.0f,0.0f };
    worldMat_ = IdentityMat4();

    uv_scale_ = ModelManager::GetModelData(modelName_)->materialData.UV_scale_;
    uv_rotate_ = { 0.0f,0.0f,0.0f };
    uv_translate_ = 
        ModelManager::GetModelData(modelName_)->materialData.UV_offset_ +
        ModelManager::GetModelData(modelName_)->materialData.UV_translate_;
    uvTransform_ = AffineMatrix(uv_scale_,uv_rotate_,uv_translate_);

    textureGH_ = TextureManager::LoadTexture(ModelManager::GetModelData(modelName_)->materialData.textureFilePath_);
    color_ = { 1.0f,1.0f,1.0f,1.0f };
    lightingType_ = LIGHTINGTYPE_HALF_LAMBERT;
}

void Model::Draw(){
    SEED::DrawModel(this);
}

void Model::UpdateMatrix(){
    worldMat_ = AffineMatrix(scale_, rotate_, translate_);
    uvTransform_ = AffineMatrix(uv_scale_, uv_rotate_, uv_translate_);

    if (parent_){
        worldMat_ = Multiply(worldMat_, parent_->worldMat_);
    }
}
