#include "Scene_Game.h"
#include <GameState_Play.h>
#include <GameState_Enter.h>
#include <SEED.h>
#include "Environment.h"
#include "ParticleManager.h"

Scene_Game::Scene_Game(SceneManager* pSceneManager){
    pSceneManager_ = pSceneManager;
    ChangeState(new GameState_Play(this));
    Initialize();
};

Scene_Game::~Scene_Game(){}

void Scene_Game::Initialize(){

    SEED::SetCamera("debug");

    ////////////////////////////////////////////////////
    //  モデル生成
    ////////////////////////////////////////////////////

    for(int i = 0; i < 128; i++){

        if(i % 4 == 0){
            models_.emplace_back(std::make_unique<Model>("Player_result.gltf"));
        } else if(i % 4 == 1){
            models_.emplace_back(std::make_unique<Model>("bunny.obj"));
        } else if(i % 4 == 2){
            models_.emplace_back(std::make_unique<Model>("sphere.obj"));
        } else if(i % 4 == 3){
            models_.emplace_back(std::make_unique<Model>("teapot.obj"));
        }

        auto& model = models_.back();
        model->translate_ = { (i % 4)* 10.0f,0.0f,(i / 4 )* 10.0f };
    
        if(i % 2 == 0){
            model->blendMode_ = BlendMode::ADD;
        }

        model->UpdateMatrix();
    }

    ////////////////////////////////////////////////////
    //  ライトの方向初期化
    ////////////////////////////////////////////////////

    SEED::GetDirectionalLight()->direction_ = { 0.0f,-1.0f,0.0f };


    ////////////////////////////////////////////////////
    //  カメラ初期化
    ////////////////////////////////////////////////////

    SEED::GetCamera()->transform_.translate_ = { 0.0f,3.0f,-50.0f };
    SEED::GetCamera()->Update();

    ////////////////////////////////////////////////////
    //  いろんなものの作成
    ////////////////////////////////////////////////////

    ParticleManager::GetInstance();

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

    static float totalFrame = -1.0f;
    static float totalDeltaTime = 0.0f;
    if(totalFrame > 0.0f){
        totalDeltaTime += 1.0f / ClockManager::DeltaTime();
    }
    totalFrame++;

    ImGui::Begin("environment");
    /*===== FPS表示 =====*/
    ImGui::Text("FPS: %f", totalDeltaTime/totalFrame);
    /*===== 解像度の更新 ====*/
    ImGui::SliderFloat("resolutionRate", &resolutionRate_, 0.0f, 1.0f);
    ImGui::End();

#endif


    // 前フレームと値が違う場合のみ更新

    if(resolutionRate_ != preRate_){
        SEED::ChangeResolutionRate(resolutionRate_);
    }

    /*========================= 各状態の更新 ==========================*/

    ParticleManager::Emit(
        ParticleType::kRadial,
        Range3D({ -10.0f,-10.0f,-10.0f }, { 10.0f,10.0f,10.0f }),
        Range1D(1.0f, 3.5f),
        Range1D(0.1f, 0.5f),
        1.0f,
        {
            Vector4(1.0f,0.0f,0.0f,1.0f),
            Vector4(0.0f,1.0f,0.0f,1.0f),
            Vector4(0.0f,0.0f,1.0f,1.0f)
        },
        5,
        BlendMode::ADD
    );


    ParticleManager::Update();
    currentState_->Update();
}

void Scene_Game::Draw(){


    // モデル描画テスト
    for(int i = 0; i < models_.size(); i++){
        models_[i]->Draw();
    }
    ParticleManager::Draw();


    // スプライト描画テスト
    Sprite sprite = Sprite("uvChecker.png");
    sprite.blendMode = BlendMode::ADD;
    SEED::DrawSprite(sprite);


    // 三角形描画テスト
    Triangle triangle = MakeEqualTriangle(5.0f, { 1.0f,0.0f,1.0f,1.0f });
    triangle.translate = {0.0f,0.0f,10.0f};
    Triangle2D triangle2D = MakeEqualTriangle2D(60.0f, { 1.0f,0.0f,0.0f,1.0f });
    triangle2D.translate = { 640.0f,360.0f };
    triangle2D.blendMode = BlendMode::SUBTRACT;
    SEED::DrawTriangle(triangle);
    SEED::DrawTriangle2D(triangle2D);

    // ライン描画テスト
    SEED::DrawLine({ 0.0f,0.0f,0.0f }, { 10.0f,10.0f,10.0f }, { 1.0f,0.0f,0.0f,1.0f });
    SEED::DrawLine2D({ 0.0f,0.0f }, { 1280.0f,720.0f }, { 0.0f,0.0f,1.0f,1.0f });
    SEED::DrawGrid();


    // 四角形描画テスト
    Quad quad = MakeEqualQuad(5.0f, { 1.0f,0.0f,1.0f,1.0f });
    quad.blendMode = BlendMode::MULTIPLY;
    Quad2D quad2D = MakeEqualQuad2D(30.0f, { 1.0f,0.0f,1.0f,1.0f });
    quad2D.translate = { 1200.0f,360.0f };
    SEED::DrawQuad(quad);
    SEED::DrawQuad2D(quad2D);

}