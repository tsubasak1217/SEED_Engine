#include <cassert>
#include "Model.h"
#include "MatrixFunc.h"
#include "SEED.h"
#include "ModelManager.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                          //
//                                          初期化・終了処理                                                   //
//                                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

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


    // アニメーションの情報取得
    hasAnimation_ = modelData->animations.size() > 0;// アニメーションが存在するか

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                          //
//                                               更新処理                                                    //
//                                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Model::Update(){

    // アニメーションの更新
    if(isAnimation_){
        totalAnimationTime_ += ClockManager::DeltaTime() * animationSpeedRate_;

        // ループするかどうかで処理を変える
        if(isAnimationLoop_){
            animationTime_ = std::fmod(totalAnimationTime_, animationDuration_);
        } else{
            animationTime_ = std::clamp(totalAnimationTime_, 0.0f,animationDuration_);
        }

        animationLoopCount_ = int32_t(totalAnimationTime_ / animationDuration_);

        // スキニング用のパレットの更新
        UpdatePalette();
    }

    // マトリックスの更新
    UpdateMatrix();
}


// マトリックスの更新
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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                          //
//                                               描画処理                                                    //
//                                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Model::Draw(){
    SEED::DrawModel(this);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                          //
//                                           アニメーション関連                                                //
//                                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

// アニメーション開始
void Model::StartAnimation(int32_t animationIndex, bool loop, float speedRate){

    // アニメーションがない場合は処理しない
    if(!hasAnimation_){ return; }
    auto& animations = ModelManager::GetModelData(modelName_)->animations;
    // 範囲外例外処理
    if(animations.size() - 1 < animationIndex) { assert(false); }

    // アニメーション名の取得
    int index = 0;
    for(auto animetion : animations){
        if(index == animationIndex){
            animationName_ = animetion.first;
            break;
        }
        index++;
    }

    // 情報の設定
    isAnimation_ = true;
    isAnimationLoop_ = loop;
    animationTime_ = 0.0f;
    totalAnimationTime_ = 0.0f;
    animationSpeedRate_ = speedRate;
    animationDuration_ = ModelManager::GetModelData(modelName_)->animations[animationName_].duration;
}

// アニメーション一時停止
void Model::PauseAnimation(){
    isAnimation_ = false;
}

// アニメーション再開
void Model::RestartAnimation(){
    isAnimation_ = true;
}

// アニメーション終了
void Model::EndAnimation(){
    isAnimation_ = false;
    animationName_ = "";
    animationTime_ = 0.0f;
    animationLoopCount_ = 0;
    totalAnimationTime_ = 0.0f;
}

// スキニング用のパレットの更新
void Model::UpdatePalette(){

    // アニメーションしない場合は更新しない
    if(!isAnimation_ or !hasAnimation_) { return; }

    // アニメーション適用後のスケルトンを取得
    const ModelSkeleton& skeleton = ModelManager::AnimatedSkeleton(
        ModelManager::GetModelData(modelName_)->animations[animationName_],
        ModelManager::GetModelData(modelName_)->defaultSkeleton,
        animationTime_
    );

    // スキニング用のパレットの更新
    const auto& inverseBindPoseMatrices = ModelManager::GetModelData(modelName_)->defaultSkinClusterData.inverseBindPoseMatrices;
    palette_.resize(skeleton.joints.size());
    for(size_t jointIndex = 0; jointIndex < skeleton.joints.size(); ++jointIndex){

        assert(jointIndex < inverseBindPoseMatrices.size());

        // スケルトン空間行列の更新
        palette_[jointIndex].skeletonSpaceMatrix =
            inverseBindPoseMatrices[jointIndex] * skeleton.joints[jointIndex].skeletonSpaceMatrix;

        // スケルトン空間行列の逆行列転置行列の更新
        palette_[jointIndex].skeletonSpaceInverceTransposeMatrix =
            Transpose(InverseMatrix(palette_[jointIndex].skeletonSpaceMatrix));
    }
}
