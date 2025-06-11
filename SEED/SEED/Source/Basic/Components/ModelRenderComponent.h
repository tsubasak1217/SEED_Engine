#pragma once
#include <SEED/Source/Basic/Object/GameObject.h>
#include <SEED/Source/Basic/Components/IComponent.h>
#include <SEED/Lib/Structs/Model.h>

// stl
#include <memory>

/*----------- モデルを描画するやつ ----------*/
class ModelRenderComponent : public IComponent{
public:
    ModelRenderComponent(GameObject* pOwner, const std::string& tagName);
    ~ModelRenderComponent() = default;
    void Initialize(const std::string& modelPath);
    void BeginFrame();
    void Update();
    void Draw();
    void EndFrame();
    void Finalize();

public:
    /*------ Model -------*/
    Model* GetModel(){ return model_.get(); }
    void ChangeModel(const std::string& modelFilePath);

    /*----- アニメーション関連 -----*/
    void StartAnimation(int32_t animationIndex, bool loop, float speedRate = 1.0f){ 
        model_->StartAnimation(animationIndex, loop, speedRate);// インデックスから取得
    }
    void StartAnimation(const std::string& animationName, bool loop, float speedRate = 1.0f){
        model_->StartAnimation(animationName, loop, speedRate);// 名前から取得
    }
    void PauseAnimation(){model_->PauseAnimation();}
    void RestartAnimation(){model_->RestartAnimation();}
    void EndAnimation(){model_->EndAnimation();}
    int32_t GetAnimationLoopCount()const{ return model_->GetAnimationLoopCount(); }
    void SetIsLoopAnimation(bool isLoop){ model_->SetIsLoopAnimation(isLoop); }
    void SetAnimationSpeedRate(float speedRate){ model_->SetAnimationSpeedRate(speedRate); }
    bool GetIsAnimation()const{ return model_->GetIsAnimation(); }
    float GetAnimationDuration()const{ return model_->GetAnimationDuration(); }
    bool GetIsEndAnimation()const{ return model_->GetIsEndAnimation(); }
    void SetIsSkeletonVisible(bool isSkeletonVisible){ model_->SetIsSkeletonVisible(isSkeletonVisible); }

private:
    std::unique_ptr<Model> model_;
};