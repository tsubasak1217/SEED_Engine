#include "Scene_Game.h"
#include <GameState_Play.h>
#include <GameState_Enter.h>
#include <SEED.h>
#include "Environment.h"
#include "ParticleManager.h"
#include "Scene_Title.h"
#include "SceneManager.h"
#include "CameraManager/CameraManager.h"

#include "../SEED/source/Manager/JsonManager/JsonCoordinator.h"

/////////////////////////////////////////////////////////////////////////////////////////
//
//  コンストラクタ・デストラクタ
//
/////////////////////////////////////////////////////////////////////////////////////////

Scene_Game::Scene_Game(SceneManager* pSceneManager){
    pSceneManager_ = pSceneManager;
    ChangeState(new GameState_Play(this));
    Initialize();
};

Scene_Game::~Scene_Game(){
    CameraManager::DeleteCamera("follow");
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//  初期化
//
/////////////////////////////////////////////////////////////////////////////////////////

void Scene_Game::Initialize(){

    ////////////////////////////////////////////////////
    //  モデル生成
    ////////////////////////////////////////////////////

    player_ = std::make_unique<Player>();
    //enemies_.push_back(std::make_unique<Enemy>(player_.get()));

    for(int i = 0; i < 12; i++) {
        auto& enemy = enemies_.emplace_back(std::make_unique<Enemy>(player_.get()));
        enemy->SetTranslate({ MyFunc::Random(-50.0f,50.0f),0.0f,MyFunc::Random(-50.0f,50.0f) });
        enemy->UpdateMatrix();
        ParticleManager::AddEffect("zombie.json", { 0.0f,0.0f,0.0f }, enemy->GetWorldMatPtr());
    }


    ////////////////////////////////////////////////////
    //  ライトの方向初期化
    ////////////////////////////////////////////////////

    SEED::GetDirectionalLight()->direction_ = { -1.0f,0.0f,0.0f };


    ////////////////////////////////////////////////////
    //  カメラ初期化
    ////////////////////////////////////////////////////

    SEED::GetCamera()->SetTranslation({ 0.0f,2.0f,-30.0f });
    SEED::GetCamera()->Update();

    followCamera_ = std::make_unique<FollowCamera>();
    CameraManager::AddCamera("follow", followCamera_.get());
    SEED::SetCamera("follow");

    ////////////////////////////////////////////////////
    //  親子付けなど
    ////////////////////////////////////////////////////

    followCamera_->SetTarget(player_.get());
    player_->SetFollowCameraPtr(followCamera_.get());

    // エフェクトの追加
    ParticleManager::AddEffect("leaves.json", { 0.0f,3.0f,0.0f }, player_->GetWorldMatPtr());
    ParticleManager::AddEffect("snow.json", { 0.0f,0.0f,0.0f });
    ParticleManager::AddEffect("darkPower.json", { 0.0f,0.0f,0.0f });


}

/////////////////////////////////////////////////////////////////////////////////////////
//
//  終了処理
//
/////////////////////////////////////////////////////////////////////////////////////////

void Scene_Game::Finalize(){}

/////////////////////////////////////////////////////////////////////////////////////////
//
//  更新処理
//
/////////////////////////////////////////////////////////////////////////////////////////

void Scene_Game::Update(){

    if(isClear_){
        pSceneManager_->ChangeScene(new Scene_Title(pSceneManager_));
        return;
    }

    /*========================== ImGui =============================*/

#ifdef _DEBUG
    ImGui::Begin("environment");
    /*===== FPS表示 =====*/
    ImGui::Text("FPS: %f", ClockManager::FPS());
    ImGui::End();

#endif

    /*========================== Manager ============================*/

    ParticleManager::Update();

    /*========================= 各状態の更新 ==========================*/
    currentState_->Update();

    player_->Update();

    for(auto& enemy : enemies_) {
        enemy->Update();
    }

    if(enemies_.size() == 0) {
        isStartFadeOut_ = true;
    }

    if(isStartFadeIn_){
        FadeIn();
    }

    if(isStartFadeOut_){
        FadeOut();
    }


}


/////////////////////////////////////////////////////////////////////////////////////////
//
//  描画処理
//
/////////////////////////////////////////////////////////////////////////////////////////

void Scene_Game::Draw(){

    // グリッドの描画
    SEED::DrawGrid();

    ParticleManager::Draw();

    player_->Draw();

    for(auto& enemy : enemies_) {
        enemy->Draw();
    }

    Sprite sprite = Sprite("howto.png");
    SEED::DrawSprite(sprite);
}


/////////////////////////////////////////////////////////////////////////////////////////
//
//  フレーム終了時の処理
//
/////////////////////////////////////////////////////////////////////////////////////////
void Scene_Game::EndFrame(){

    player_->EndFrame();

    for(auto& enemy : enemies_) {
        enemy->EndFrame();
    }

    // 死んだ敵の削除
    enemies_.remove_if([](const std::unique_ptr<Enemy>& enemy) {
        return !enemy->GetIsAlive();
    });
}


/////////////////////////////////////////////////////////////////////////////////////////
//
//  フェードイン
//
/////////////////////////////////////////////////////////////////////////////////////////
void Scene_Game::FadeIn(){
    fadeTime_ -= ClockManager::DeltaTime();

    float t = fadeTime_ / kFadeTime_;

    Sprite sprite("Assets/white1x1.png");
    sprite.size = { 1280.0f,720.0f };
    sprite.color = { 1.0f,1.0f,1.0f,t };
    SEED::DrawSprite(sprite);

    if(fadeTime_ <= 0.0f){
        isStartFadeIn_ = false;
        fadeTime_ = kFadeTime_;
    }

}

/////////////////////////////////////////////////////////////////////////////////////////
//
//  フェードアウト
//
/////////////////////////////////////////////////////////////////////////////////////////
void Scene_Game::FadeOut(){
    fadeTime_ -= ClockManager::DeltaTime();

    float t = fadeTime_ / kFadeTime_;
    fadeTime_ -= ClockManager::DeltaTime();

    Sprite sprite("Assets/white1x1.png");
    sprite.size = { 1280.0f,720.0f };
    sprite.color = { 1.0f,1.0f,1.0f,1.0f - t };
    SEED::DrawSprite(sprite);

    if(fadeTime_ <= 0.0f){
        isClear_ = true;
    }
}
