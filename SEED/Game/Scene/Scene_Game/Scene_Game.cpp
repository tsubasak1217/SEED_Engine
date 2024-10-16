#include "Scene_Game.h"
#include <GameState_Play.h>
#include <GameState_Enter.h>
#include <SEED.h>

Scene_Game::Scene_Game(SceneManager* pSceneManager){
    pSceneManager_ = pSceneManager;
    ChangeState(new GameState_Play(this));
    Initialize();
};

Scene_Game::~Scene_Game(){}

void Scene_Game::Initialize(){

    ////////////////////////////////////////////////////
    //  モデル生成
    ////////////////////////////////////////////////////

    model = new Model("teapot");
    model->textureGH_ = TextureManager::LoadTexture("uvChecker.png");
    model->blendMode_ = BlendMode::NORMAL;
    model->color_ = { 1.0f,1.0f,1.0f,0.1f };

    ////////////////////////////////////////////////////
    //  ライトの方向初期化
    ////////////////////////////////////////////////////

    SEED::GetDirectionalLight()->direction_ = { 0.0f,-1.0f,0.0f };


    ////////////////////////////////////////////////////
    //  カメラ初期化
    ////////////////////////////////////////////////////

    SEED::GetCamera()->transform_.translate_ = { 0.0f,3.0f,0.0f };
    SEED::GetCamera()->Update();

    ////////////////////////////////////////////////////
    //  天球の作成
    ////////////////////////////////////////////////////


    ////////////////////////////////////////////////////
    //  解像度の初期設定
    ////////////////////////////////////////////////////

    SEED::ChangeResolutionRate(resolutionRate_);
}

void Scene_Game::Finalize(){}

void Scene_Game::Update(){
    /*======================= 前フレームの値保存 ======================*/

    preRate_ = resolutionRate_;

    /*========================== ImGui =============================*/

#ifdef _DEBUG

    ImGui::Begin("Model");
    ImGui::SliderInt(
        "BlendMode",reinterpret_cast<int*>(&model->blendMode_),
        0,int(BlendMode::kBlendModeCount) - 1
    );
    ImGui::SliderFloat("Alpha", &model->color_.w, 0.0f, 1.0f);
    ImGui::End();

#endif

    /*========================= 解像度の更新 ==========================*/

    // 前フレームと値が違う場合のみ更新
    if(resolutionRate_ != preRate_){
        SEED::ChangeResolutionRate(resolutionRate_);
    }

    /*========================= 各状態のの更新 ==========================*/

    for(int i = 0; i < 2; i++){
        model->translate_ = { 0.5f * i,1.0f,0.0f };
        model->UpdateMatrix();
        model->Draw();
    }

    currentState_->Update();

}

void Scene_Game::Draw(){
    SEED::DrawGrid(1.0f, 128);
    currentState_->Draw();

}