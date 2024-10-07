#include "Scene_Game.h"
#include <GameState_Play.h>
#include <GameState_Enter.h>
#include <SEED.h>

Scene_Game::Scene_Game(SceneManager* pSceneManager){
    pSceneManager_ = pSceneManager;
    ChangeState(new GameState_Play(this));
    Initialize();
};

Scene_Game::~Scene_Game()
{
}

void Scene_Game::Initialize()
{
    ////////////////////////////////////////////////////
    //  デバッグ用の地面生成
    ////////////////////////////////////////////////////
    ground_ = std::make_unique<Model>("ground");

    sprite = Sprite("uvChecker.png");
    sprite.anchorPoint = { 0.5f,0.5f };
    sprite.translate = { 640.0f,360.0f };
    sprite.clipLT = { 256.0f,256.0f };
    sprite.clipSize = { -64.0f,-64.0f };

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

    skydome_ = std::make_unique<Model>("skydome");
    skydome_->scale_ = { 150.0f,150.0f,150.0f };
    skydome_->lightingType_ = LIGHTINGTYPE_NONE;
    skydome_->UpdateMatrix();


    ////////////////////////////////////////////////////
    //  解像度の初期設定
    ////////////////////////////////////////////////////

    SEED::ChangeResolutionRate(resolutionRate_);
}

void Scene_Game::Finalize()
{
}

void Scene_Game::Update()
{
    /*======================= 前フレームの値保存 ======================*/

    preRate_ = resolutionRate_;

    /*========================== ImGui =============================*/

#ifdef _DEBUG
#endif

    /*========================= 解像度の更新 ==========================*/

    // 前フレームと値が違う場合のみ更新
    if(resolutionRate_ != preRate_){
        SEED::ChangeResolutionRate(resolutionRate_);
    }

    /*========================= 各状態のの更新 ==========================*/

    currentState_->Update();

    sprite.rotate += 3.14f * (1.0f / 60.0f);
}

void Scene_Game::Draw()
{
    skydome_->Draw();
    ground_->Draw();

    currentState_->Draw();

    SEED::DrawSprite(sprite);
}