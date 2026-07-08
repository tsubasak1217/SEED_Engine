#include "GameSystem.h"
#include <SEED/Source/Manager/SceneManager/SceneManager.h>
#include <SEED/Source/Basic/Scene/SceneRegister.h>
#include <SEED/Source/Manager/ImGuiManager/ImGuiManager.h>
#include <SEED/Source/Manager/VideoManager/VideoManager.h>
#include <SEED/Source/Manager/SceneTransitionDrawer/SceneTransitionDrawer.h>
#include <SEED/Source/Manager/HttpManager/HttpManager.h>
#include <SEED/Source/Manager/InputManager/InputManager.h>
#include <Game/Manager/UserManager.h>

namespace SEED{
    std::unique_ptr<GameSystem> GameSystem::instance_ = nullptr;

    GameSystem* GameSystem::GetInstance(){
        if(instance_ == nullptr){
            instance_.reset(new GameSystem());
        }
        return instance_.get();
    }

    void GameSystem::Initialize(){
        GetInstance();
        HttpManager::Initialize();
        UserManager::Initialize();
        SceneRegister::RegisterScenes();
        ChangeScene("Title");
    }

    void GameSystem::Finalize(){
        instance_->pScene_->Finalize();
        HttpManager::Release();
    }

    void GameSystem::Run(){
        instance_->BeginFrame();
        instance_->Update();
        instance_->Draw();
        instance_->EndFrame();
    }

    void GameSystem::Update(){
        HttpManager::Update();
        UserManager::GetInstance()->Update();
        if (SEED::Input::IsTriggerKey(DIK_ESCAPE)) { UserManager::GetInstance()->OpenLoginWindow(); }

        if (!UserManager::GetInstance()->IsLoggedIn()) {
            return;
        }

        instance_->pScene_->Update();
        VideoManager::GetInstance()->Update();
        ParticleManager::Update();
        CameraManager::Update();
        SceneTransitionDrawer::Update();
    }

    void GameSystem::Draw(){
        if (!UserManager::GetInstance()->IsLoggedIn()) {
            return;
        }

        if(instance_->pScene_){
            instance_->pScene_->Draw();
        }
        ParticleManager::Draw();
        SceneTransitionDrawer::Draw();
        DrawGUI();
    }

    void GameSystem::BeginFrame(){
        CollisionManager::ResetColliderList();
        if (instance_->pScene_) {
            instance_->pScene_->BeginFrame();
            instance_->pScene_->HandOverColliders();
        }
        CollisionManager::CheckCollision();
        CollisionManager::Draw();
    }

    void GameSystem::EndFrame(){
        if(instance_->pScene_){
            instance_->pScene_->EndFrame();
        }
    }

    void GameSystem::DrawGUI(){
    #ifdef _DEBUG
        CollisionManager::GUI();
    #endif // _DEBUG
    }

    void GameSystem::ChangeScene(const std::string& sceneName){
        instance_->pScene_.reset();
        auto* nextScene = SceneManager::CreateScene(sceneName);
        instance_->pScene_ = std::unique_ptr<Scene_Base>(nextScene);
        instance_->pScene_->Initialize();
        Scene_Base::isChangeScene_ = true;
    }
}
