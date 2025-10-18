#include <cassert>
#include <SEED/Lib/Structs/Model.h>
#include <SEED/Lib/Functions/MatrixFunc.h>
#include <SEED/Source/SEED.h>
#include <SEED/Source/Manager/ModelManager/ModelManager.h>
#include <execution>
#include <algorithm>

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

    worldMat_ = IdentityMat4();

    // マテリアルの数だけ初期化
    materials_.clear();
    for(int i = 0; i < modelData->materials.size(); i++){

        auto material = &modelData->materials[i];

        // 新しいマテリアルを追加
        Material newMaterial;
        newMaterial.color = material->color_;
        newMaterial.GH = TextureManager::LoadTexture(material->textureFilePath_);
        newMaterial.uvTransform = AffineMatrix(
            material->UV_scale_,
            { 0.0f,0.0f,0.0f },
            material->UV_offset_ + material->UV_translate_
        );

        materials_.push_back(newMaterial);

    }

    // 全体の色
    masterColor_ = { 1.0f,1.0f,1.0f,1.0f };


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
            animationTime_ = std::clamp(totalAnimationTime_, 0.0f, animationDuration_);
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

    // ローカル変換行列の更新
    localMat_ = AffineMatrix(transform_.scale, transform_.rotate, transform_.translate);

    // ワールド行列の更新
    if(parentMat_){

        if(isParentRotate_ + isParentScale_ + isParentTranslate_ == 3){
            worldMat_ = localMat_ * (*parentMat_);
            return;
        } else{

            Matrix4x4 cancelMat = IdentityMat4();

            // 親の行列から取り出した要素を打ち消す行列を作成
            if(!isParentScale_){
                Vector3 inverseScale = Vector3(1.0f, 1.0f, 1.0f) / ExtractScale(*parentMat_);
                cancelMat = cancelMat * ScaleMatrix(inverseScale);
            }

            if(!isParentRotate_){
                Vector3 inverseRotate = ExtractRotation(*parentMat_) * -1.0f;
                cancelMat = cancelMat * RotateMatrix(inverseRotate);
            }

            if(!isParentTranslate_){
                Vector3 inverseTranslate = ExtractTranslation(*parentMat_) * -1.0f;
                cancelMat = cancelMat * TranslateMatrix(inverseTranslate);
            }

            Matrix4x4 canceledMat = cancelMat * *parentMat_;
            worldMat_ = localMat_ * canceledMat;

        }

    } else{
        worldMat_ = localMat_;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                          //
//                                               描画処理                                                    //
//                                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Model::Draw(const std::optional<Color>& masterColor){
    SEED::DrawModel(this,masterColor);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                          //
//                                           アニメーション関連                                                //
//                                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

// アニメーション開始(インデックスから)
void Model::StartAnimation(int32_t animationIndex, bool loop, float speedRate){

    // アニメーションがない場合は処理しない
    if(!hasAnimation_){ return; }
    auto& animations = ModelManager::GetModelData(modelName_)->animations;
    // 範囲外例外処理
    if(animations.size() - 1 < animationIndex){ assert(false); }

    // なければ骨を作成
    if(!animetedSkeleton_){
        animetedSkeleton_ = std::make_unique<ModelSkeleton>();

        // jointの名前、インデックスのマップをコピー
        auto& defaultSkeleton = ModelManager::GetModelData(modelName_)->defaultSkeleton;
        for(auto& [jointName, jointIndex] : defaultSkeleton.jointMap){
            animetedSkeleton_->jointMap[jointName] = jointIndex;
        }
    }

    // 切り替える前にスケルトンの状態を保持
    if(!preSkeleton_){
        preSkeleton_ = std::make_unique<ModelSkeleton>();
        ModelManager::AnimatedSkeleton(
            ModelManager::GetModelData(modelName_)->animations[animationName_],
            ModelManager::GetModelData(modelName_)->defaultSkeleton,
            preSkeleton_.get(),
            animationTime_
        );

    } else{
        // アニメーション適用後のスケルトンを取得
        ModelManager::AnimatedSkeleton(
            ModelManager::GetModelData(modelName_)->animations[animationName_],
            ModelManager::GetModelData(modelName_)->defaultSkeleton,
            animetedSkeleton_.get(),
            animationTime_
        );

        // アニメーション補間
        progressOfAnimLerp_ = animLerpTime_ / kAnimLerpTime_;
        progressOfAnimLerp_ = std::clamp(progressOfAnimLerp_, 0.0f, 1.0f);

        // 補間後のスケルトンを設定
        preSkeleton_.reset(new ModelSkeleton(ModelManager::InterpolateSkeleton(
            *preSkeleton_,
            *animetedSkeleton_,
            progressOfAnimLerp_
        )));
    }

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
    animLerpTime_ = 0.0f;
}

// アニメーション開始(アニメーション名から)
void Model::StartAnimation(const std::string& animationName, bool loop, float speedRate){

    // アニメーションがない場合は処理しない
    if(!hasAnimation_){ return; }

    // アニメーション名が存在するか確認
    auto& animations = ModelManager::GetModelData(modelName_)->animations;
    auto itr = animations.find(animationName);
    if(itr == animations.end()){
        assert(false);// 存在しないのでアサート
    }

    // なければ骨を作成
    if(!animetedSkeleton_){
        animetedSkeleton_ = std::make_unique<ModelSkeleton>();

        // jointの名前、インデックスのマップをコピー
        auto& defaultSkeleton = ModelManager::GetModelData(modelName_)->defaultSkeleton;
        for(auto& [jointName, jointIndex] : defaultSkeleton.jointMap){
            animetedSkeleton_->jointMap[jointName] = jointIndex;
        }
    }

    // 切り替える前にスケルトンの状態を保持
    if(!preSkeleton_){
        preSkeleton_ = std::make_unique<ModelSkeleton>();
        ModelManager::AnimatedSkeleton(
            ModelManager::GetModelData(modelName_)->animations[animationName_],
            ModelManager::GetModelData(modelName_)->defaultSkeleton,
            preSkeleton_.get(),
            animationTime_
        );

    } else{
        // アニメーション適用後のスケルトンを取得
        ModelManager::AnimatedSkeleton(
            ModelManager::GetModelData(modelName_)->animations[animationName_],
            ModelManager::GetModelData(modelName_)->defaultSkeleton,
            animetedSkeleton_.get(),
            animationTime_
        );

        // アニメーション補間
        progressOfAnimLerp_ = animLerpTime_ / kAnimLerpTime_;
        progressOfAnimLerp_ = std::clamp(progressOfAnimLerp_, 0.0f, 1.0f);

        // 補間後のスケルトンを設定
        preSkeleton_.reset(new ModelSkeleton(ModelManager::InterpolateSkeleton(
            *preSkeleton_,
            *animetedSkeleton_,
            progressOfAnimLerp_
        )));
    }

    // 情報の設定
    isAnimation_ = true;
    isAnimationLoop_ = loop;
    animationTime_ = 0.0f;
    totalAnimationTime_ = 0.0f;
    animationSpeedRate_ = speedRate;
    animationName_ = animationName;
    animationDuration_ = ModelManager::GetModelData(modelName_)->animations[animationName_].duration;
    animLerpTime_ = 0.0f;
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
    if(!isAnimation_ or !hasAnimation_){ return; }

    // アニメーション適用後のスケルトンを取得
    ModelManager::AnimatedSkeleton(
        ModelManager::GetModelData(modelName_)->animations[animationName_],
        ModelManager::GetModelData(modelName_)->defaultSkeleton,
        animetedSkeleton_.get(),
        animationTime_
    );

    // アニメーション補間
    if(preSkeleton_){
        if(progressOfAnimLerp_ < 1.0f){

            // 補間の進捗度(t)
            progressOfAnimLerp_ = animLerpTime_ / kAnimLerpTime_;
            progressOfAnimLerp_ = std::clamp(progressOfAnimLerp_, 0.0f, 1.0f);

            // 補間後のスケルトンを取得
            const ModelSkeleton& lerpedSkeleton = ModelManager::InterpolateSkeleton(
                *preSkeleton_,
                *animetedSkeleton_,
                progressOfAnimLerp_
            );

            // 補間時間を加算
            animLerpTime_ += ClockManager::DeltaTime();

            // スキニング用のパレットの更新
            const auto& inverseBindPoseMatrices = ModelManager::GetModelData(modelName_)->defaultSkinClusterData.inverseBindPoseMatrices;
            palette_.resize(lerpedSkeleton.joints.size());
            std::for_each(std::execution::par_unseq, animetedSkeleton_->joints.begin(), animetedSkeleton_->joints.end(), [&](const ModelJoint& joint){

                uint32_t jointIndex = joint.index;
                assert(jointIndex < inverseBindPoseMatrices.size());

                // スケルトン空間行列の更新
                palette_[jointIndex].skeletonSpaceMatrix =
                    inverseBindPoseMatrices[jointIndex] * lerpedSkeleton.joints[jointIndex].skeletonSpaceMatrix;

                // スケルトン空間行列の逆行列転置行列の更新
                palette_[jointIndex].skeletonSpaceInverceTransposeMatrix =
                    Transpose(InverseMatrix(palette_[jointIndex].skeletonSpaceMatrix));
            });

            return;

        } else{
            // 補間終了
            preSkeleton_.release();
            progressOfAnimLerp_ = 0.0f;
            animLerpTime_ = 0.0f;
        }
    }

    // スキニング用のパレットの更新
    const auto& inverseBindPoseMatrices = ModelManager::GetModelData(modelName_)->defaultSkinClusterData.inverseBindPoseMatrices;
    palette_.resize(animetedSkeleton_->joints.size());
    std::for_each(std::execution::par_unseq, animetedSkeleton_->joints.begin(), animetedSkeleton_->joints.end(), [&](const ModelJoint& joint){

        uint32_t jointIndex = joint.index;
        assert(jointIndex < inverseBindPoseMatrices.size());

        // スケルトン空間行列の更新
        palette_[jointIndex].skeletonSpaceMatrix =
            inverseBindPoseMatrices[jointIndex] * animetedSkeleton_->joints[jointIndex].skeletonSpaceMatrix;

        // スケルトン空間行列の逆行列転置行列の更新
        palette_[jointIndex].skeletonSpaceInverceTransposeMatrix =
            Transpose(InverseMatrix(palette_[jointIndex].skeletonSpaceMatrix));

    });
}

// アニメーションの名前一覧を取得
std::vector<std::string> Model::GetAnimationNames() const{
    auto modelData = ModelManager::GetModelData(modelName_);
    std::vector<std::string> animationNames;
    for(const auto& animation : modelData->animations){
        animationNames.push_back(animation.first);
    }
    return animationNames;
}


// モデルの変更
void Model::ChangeModel(const std::string& modelName){
    // モデルの読み込み
    ModelManager::LoadModel(modelName);
    auto modelData = ModelManager::GetModelData(modelName);
    // マテリアルの数だけ初期化
    materials_.clear();
    for(int i = 0; i < modelData->materials.size(); i++){
        auto material = &modelData->materials[i];
        // 新しいマテリアルを追加
        Material newMaterial;
        newMaterial.color = material->color_;
        newMaterial.GH = TextureManager::LoadTexture(material->textureFilePath_);
        newMaterial.uvTransform = AffineMatrix(
            material->UV_scale_,
            { 0.0f,0.0f,0.0f },
            material->UV_offset_ + material->UV_translate_
        );
        materials_.push_back(newMaterial);
    }
    // 全体の色
    masterColor_ = { 1.0f,1.0f,1.0f,1.0f };
    // アニメーションの情報取得
    hasAnimation_ = modelData->animations.size() > 0;
    // モデル名の更新
    modelName_ = modelName;
}
