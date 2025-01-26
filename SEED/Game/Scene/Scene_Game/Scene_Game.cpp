#include "Scene_Game.h"
#include <SEED.h>
#include "Environment.h"
#include "ParticleManager.h"
#include "Scene_Title.h"
#include "CameraManager/CameraManager.h"
#include "../adapter/json/JsonCoordinator.h"

// 各ステートのインクルード
#include <GameState_Play.h>
#include <GameState_Select.h>
#include <GameState_Pause.h>
#include <GameState_Exit.h>

/////////////////////////////////////////////////////////////////////////////////////////
//
//  コンストラクタ・デストラクタ
//
/////////////////////////////////////////////////////////////////////////////////////////

Scene_Game::Scene_Game(SceneManager* pSceneManager) {
    pSceneManager_ = pSceneManager;
    Initialize();
};

Scene_Game::~Scene_Game() {
    CameraManager::DeleteCamera("follow");
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//  初期化
//
/////////////////////////////////////////////////////////////////////////////////////////

void Scene_Game::Initialize() {

    ////////////////////////////////////////////////////
    // マネージャー初期化
    ////////////////////////////////////////////////////

    //// EventManager
    //stageManager_ = std::make_unique<StageManager>(eventManager_);

    //// PlayerCorpseManager
    //playerCorpseManager_ = std::make_unique<PlayerCorpseManager>();
    //playerCorpseManager_->Initialize();

    //// EggManager
    //eggManager_ = std::make_unique<EggManager>();

    ////////////////////////////////////////////////////
    //  カメラ初期化
    ////////////////////////////////////////////////////

    SEED::GetCamera()->SetTranslation({ -191.6f,46.8f,-185.8f });
    SEED::GetCamera()->SetRotation({ 0.15173f,0.7807f,0.0f });
    SEED::GetCamera()->Update();

    followCamera_ = std::make_unique<FollowCamera>();
    CameraManager::AddCamera("follow", followCamera_.get());
    SEED::SetCamera("debug");

    ////////////////////////////////////////////////////
    //  ライトの初期化
    ////////////////////////////////////////////////////

    directionalLight_ = std::make_unique<DirectionalLight>();
    directionalLight_->color_ = MyMath::FloatColor(0xffffffff);
    directionalLight_->direction_ = MyMath::Normalize({ 2.0f,1.0f,0.5f });
    directionalLight_->intensity = 1.0f;

    pointLights_.clear();
    for (int i = 0; i < 32; i++) {
        pointLights_.push_back(std::make_unique<PointLight>());
        pointLightColor_h_.push_back(MyFunc::Random(0.0f, 1.0f));
        pointLights_[i]->color_ = MyMath::HSV_to_RGB(pointLightColor_h_[i], 1.0f, 1.0f, 1.0f);
        pointLights_[i]->position = { MyFunc::Random(-100.0f,100.0f),MyFunc::Random(2.0f,15.0f),MyFunc::Random(-100.0f,100.0f) };
        pointLights_[i]->intensity = 1.0f;
    }

    spotLights_.clear();
    for (int i = 0; i < 32; i++) {
        spotLights_.push_back(std::make_unique<SpotLight>());
        spotLightColor_h_.push_back(MyFunc::Random(0.0f, 1.0f));
        spotLights_[i]->color_ = MyMath::HSV_to_RGB(spotLightColor_h_[i], 1.0f, 1.0f, 1.0f);
        spotLights_[i]->position = { MyFunc::Random(-100.0f,100.0f),15.0f,MyFunc::Random(-100.0f,100.0f) };
        spotLights_[i]->intensity = 1.0f;
        spotLights_[i]->direction = MyFunc::RandomDirection({ 0.0f,-1.0f,0.0f }, 3.14f * 0.5f);
    }

    ////////////////////////////////////////////////////
    //  オブジェクトの初期化
    ////////////////////////////////////////////////////

    // Player
   //player_ = std::make_unique<Player>();
   //player_->Initialize();
   //stageManager_->SetPlayer(player_.get());

    ground_ = std::make_unique<Model>("Assets/ground.obj");

    sphere_ = std::make_unique<Model>("Assets/sphere.obj");


    for (int i = 0; i < 32; i++) {
        if (i % 2 == 0) {
            man_.emplace_back(std::make_unique<Model>("Assets/man.gltf"));
        } else {
            man_.emplace_back(std::make_unique<Model>("Assets/zombie.gltf"));
        }
        man_[i]->translate_ = { MyFunc::Random(-100.0f,0.0f),0.0f,MyFunc::Random(0.0f,100.0f) };
        man_[i]->StartAnimation(MyFunc::Random(0, 5), false);
    }

    //////////////////////////////////////////////////////
    //// スプライトの初期化
    //////////////////////////////////////////////////////

    //backSprite_ = std::make_unique<Sprite>("Assets/white1x1.png");
    //backSprite_->size = kWindowSize;
    //backSprite_->color = MyMath::FloatColor(0, 229, 229, 255);
    //backSprite_->drawLocation = DrawLocation::Back;
    //backSprite_->isStaticDraw = false;

    //////////////////////////////////////////////////////
    ////  他クラスの情報を必要とするクラスの初期化
    //////////////////////////////////////////////////////

    //// DoorProximityChecker の 初期化
    //doorProximityChecker_ =
    //    std::make_unique<DoorProximityChecker>(
    //        eventManager_,
    //        *stageManager_.get(),
    //        *player_.get()
    //    );

    //// EnemyManager の 初期化
    //enemyEditor_ = std::make_unique<EnemyEditor>();

    ///////////////////////////////////////////////////
    ////  関連付けや初期値の設定
    ///////////////////////////////////////////////////

    //// followCameraにplayerをセット
    //followCamera_->SetTarget(player_.get());

    //// playerに必要な情報をセット
    //player_->SetCorpseManager(playerCorpseManager_.get());
    //player_->SetFollowCameraPtr(followCamera_.get());
    //player_->SetEggManager(eggManager_.get());
    //player_->SetPosition(StageManager::GetStartPos());

    //// eggManagerにplayerをセット
    //eggManager_->SetPlayer(player_.get());
    //eggManager_->Initialize();


    /////////////////////////////////////////////////
    // テスト用のモデルの初期化
    /////////////////////////////////////////////////

    float interval = 10.0f;

    // Animation_Node
    std::string baseTestModelPath = "TestModels/Animation_Node/Animation_Node_";
    for (int i = 0; i < 6; i++) {
        std::string modelPath = baseTestModelPath + "0" + std::to_string(i) + ".gltf";
        test_AnimationNode_.push_back(std::make_unique<Model>(modelPath));
        test_AnimationNode_[i]->translate_ = { i * interval,5.0f,0.0f };
        test_AnimationNode_[i]->scale_ = { 3.0f,3.0f,3.0f };
        test_AnimationNode_[i]->cullMode = D3D12_CULL_MODE::D3D12_CULL_MODE_NONE;
        test_AnimationNode_[i]->StartAnimation(0, true);
    }

    // Animation_NodeMisc
    baseTestModelPath = "TestModels/Animation_NodeMisc/Animation_NodeMisc_";
    for (int i = 0; i < 9; i++) {
        std::string modelPath = baseTestModelPath + "0" + std::to_string(i) + ".gltf";
        test_AnimationNodeMisc_.push_back(std::make_unique<Model>(modelPath));
        test_AnimationNodeMisc_[i]->translate_ = { i * interval,5.0f,interval };
        test_AnimationNodeMisc_[i]->scale_ = { 3.0f,3.0f,3.0f };
        test_AnimationNodeMisc_[i]->cullMode = D3D12_CULL_MODE::D3D12_CULL_MODE_NONE;
        test_AnimationNodeMisc_[i]->StartAnimation(0, true);
    }

    // Animation_Skin
    baseTestModelPath = "TestModels/Animation_Skin/Animation_Skin_";
    for (int i = 0; i < 12; i++) {
        if (i == 3) {
            test_AnimationSkin_.push_back(std::make_unique<Model>());
            continue;
        }

        std::string modelPath;
        if (i < 10) {
            modelPath = baseTestModelPath + "0" + std::to_string(i) + ".gltf";
        } else {
            modelPath = baseTestModelPath + std::to_string(i) + ".gltf";
        }

        test_AnimationSkin_.push_back(std::make_unique<Model>(modelPath));
        test_AnimationSkin_[i]->translate_ = { i * interval,5.0f,interval * 2 };
        test_AnimationSkin_[i]->scale_ = { 3.0f,3.0f,3.0f };
        test_AnimationSkin_[i]->cullMode = D3D12_CULL_MODE::D3D12_CULL_MODE_NONE;
        test_AnimationSkin_[i]->StartAnimation(0, true);
    }

    // Mesh_Primitives
    baseTestModelPath = "TestModels/Mesh_Primitives/Mesh_Primitives_";
    for (int i = 0; i < 1; i++) {
        std::string modelPath = baseTestModelPath + "0" + std::to_string(i) + ".gltf";
        test_MeshPrimitives_.push_back(std::make_unique<Model>(modelPath));
        test_MeshPrimitives_[i]->translate_ = { i * interval,5.0f,interval * 3 };
        test_MeshPrimitives_[i]->scale_ = { 3.0f,3.0f,3.0f };
        test_MeshPrimitives_[i]->cullMode = D3D12_CULL_MODE::D3D12_CULL_MODE_NONE;
    }

    // Material_AlphaBlend
    baseTestModelPath = "TestModels/Material_AlphaBlend/Material_AlphaBlend_";
    for (int i = 0; i < 7; i++) {
        std::string modelPath = baseTestModelPath + "0" + std::to_string(i) + ".gltf";
        test_MaterialAlphaBlend_.push_back(std::make_unique<Model>(modelPath));
        test_MaterialAlphaBlend_[i]->translate_ = { i * interval,5.0f,interval * 4 };
        test_MaterialAlphaBlend_[i]->scale_ = { 3.0f,3.0f,3.0f };
        test_MaterialAlphaBlend_[i]->cullMode = D3D12_CULL_MODE::D3D12_CULL_MODE_NONE;
    }

    // Texture_Sampler
    baseTestModelPath = "TestModels/Texture_Sampler/Texture_Sampler_";
    for (int i = 0; i < 14; i++) {
        std::string modelPath;
        if (i < 10) {
            modelPath = baseTestModelPath + "0" + std::to_string(i) + ".gltf";
        } else {
            modelPath = baseTestModelPath + std::to_string(i) + ".gltf";
        }
        test_TextureSampler_.push_back(std::make_unique<Model>(modelPath));
        test_TextureSampler_[i]->translate_ = { i * interval,5.0f,interval * 5 };
        test_TextureSampler_[i]->scale_ = { 3.0f,3.0f,3.0f };
        test_TextureSampler_[i]->cullMode = D3D12_CULL_MODE::D3D12_CULL_MODE_NONE;
    }
}

void Scene_Game::Finalize() {}

/////////////////////////////////////////////////////////////////////////////////////////
//
//  更新処理
//
/////////////////////////////////////////////////////////////////////////////////////////

void Scene_Game::Update() {

    /*========================== ImGui =============================*/

#ifdef _DEBUG
    ImGui::Begin("environment");
    /*===== FPS表示 =====*/
    ImGui::Text("FPS: %f", ImGui::GetIO().Framerate);
    ImGui::End();

    //enemyEditor_->ShowImGui();
#endif

    /*======================= 各状態固有の更新 ========================*/

    if (currentState_) {
        currentState_->Update();
    }

    /*==================== 各オブジェクトの基本更新 =====================*/

    for (int i = 0; i < pointLights_.size(); i++) {
        pointLightColor_h_[i] += ClockManager::DeltaTime();
        pointLights_[i]->color_ = MyMath::HSV_to_RGB(pointLightColor_h_[i], 1.0f, 1.0f, 1.0f);
        pointLights_[i]->intensity = 2.0f + std::sin(3.14f * ClockManager::DeltaTime()) * 0.5f;
    }

    for (int i = 0; i < spotLights_.size(); i++) {
        spotLightColor_h_[i] += ClockManager::DeltaTime();
        spotLights_[i]->color_ = MyMath::HSV_to_RGB(spotLightColor_h_[i], 1.0f, 1.0f, 1.0f);
        spotLights_[i]->intensity = 2.0f + std::sin(3.14f * ClockManager::DeltaTime()) * 2.0f;

        Vector2 preDirectionXZ = { spotLights_[i]->direction.x,spotLights_[i]->direction.z };
        // 回転する
        preDirectionXZ *= RotateMatrix(ClockManager::DeltaTime() * 0.33f);
        spotLights_[i]->direction = { preDirectionXZ.x,spotLights_[i]->direction.y,preDirectionXZ.y };
    }

    for (auto& man : man_) {
        if (man->GetIsEndAnimation()) {
            man->StartAnimation(MyFunc::Random(0, 5), false);
        }
        man->Update();
    }

    sphere_->Update();

    // ポーズ中は以下を更新しない
    //if (isPaused_) { return; }

    //ParticleManager::Update();

    //player_->Update();

    //eggManager_->Update();
    //playerCorpseManager_->Update();

    //// フィールドの更新
    //stageManager_->Update();

    //// ドアとの距離をチェックし、近ければイベント発行
    //doorProximityChecker_->Update();

    // テストモデルの更新
    for (int i = 0; i < test_AnimationNode_.size(); i++) {
        test_AnimationNode_[i]->Update();
    }

    for (int i = 0; i < test_AnimationNodeMisc_.size(); i++) {
        test_AnimationNodeMisc_[i]->Update();
    }

    for (int i = 0; i < test_AnimationSkin_.size(); i++) {
        test_AnimationSkin_[i]->Update();
    }

    for (int i = 0; i < test_MeshPrimitives_.size(); i++) {
        test_MeshPrimitives_[i]->Update();
    }

    for (int i = 0; i < test_MaterialAlphaBlend_.size(); i++) {
        test_MaterialAlphaBlend_[i]->Update();
    }

    for (int i = 0; i < test_TextureSampler_.size(); i++) {
        test_TextureSampler_[i]->Update();
    }


#ifdef _DEBUG

    ImGui::Begin("CG3_Test");

    ImGui::Separator();
    ImGui::Text("----------How to play----------");
    ImGui::Text("LStick / WASD : move_XZ");
    ImGui::Text("LRTrigger / QE : move_Y");
    ImGui::Text("RStick / Mouse : rotation");
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0.0f, 5.0f));

    ImGui::Checkbox("isPointLightActive", &isPointLightActive_);
    ImGui::Checkbox("isSpotLightActive", &isSpotLightActive_);
    if (ImGui::Checkbox("isUseDebugCamera", &isUseDebugCamera_)) {
        if (isUseDebugCamera_) {
            SEED::SetCamera("debug");
        } else {
            SEED::SetCamera("main");
        }
    }
    ImGui::Separator();
    ImGui::Text("Sphere");
    ImGui::DragFloat3("translate", &sphere_->translate_.x, 0.1f);
    ImGui::DragFloat3("scale", &sphere_->scale_.x, 0.1f);
    ImGui::ColorEdit4("color", &sphere_->color_.x);
    ImGui::DragFloat("shininess", &sphere_->shininess_, 0.1f, 0.0f, 100.0f);

    ImGui::End();

#endif // _DEBUG

}

/////////////////////////////////////////////////////////////////////////////////////////
//
//  描画処理
//
/////////////////////////////////////////////////////////////////////////////////////////

void Scene_Game::Draw() {

    /*======================= 各状態固有の描画 ========================*/

    if (currentState_) {
        currentState_->Draw();
    }

    /*==================== 各オブジェクトの基本描画 =====================*/

    // ライトの情報を送る
    directionalLight_->SendData();

    if (isPointLightActive_) {
        for (int i = 0; i < pointLights_.size(); i++) {
            pointLights_[i]->SendData();
            SEED::DrawLight(pointLights_[i].get());
        }
    }

    if (isSpotLightActive_) {
        for (int i = 0; i < spotLights_.size(); i++) {
            spotLights_[i]->SendData();
            SEED::DrawLight(spotLights_[i].get());
        }
    }

    // 地面
    ground_->Draw();

    // 球
    sphere_->Draw();

    for (auto& man : man_) {
        man->Draw();
    }

    // テストモデルの更新
    for (int i = 0; i < test_AnimationNode_.size(); i++) {
        test_AnimationNode_[i]->Draw();
    }

    for (int i = 0; i < test_AnimationNodeMisc_.size(); i++) {
        test_AnimationNodeMisc_[i]->Draw();
    }

    for (int i = 0; i < test_AnimationSkin_.size(); i++) {
        test_AnimationSkin_[i]->Draw();
    }

    for (int i = 0; i < test_MeshPrimitives_.size(); i++) {
        test_MeshPrimitives_[i]->Draw();
    }

    for (int i = 0; i < test_MaterialAlphaBlend_.size(); i++) {
        test_MaterialAlphaBlend_[i]->Draw();
    }

    for (int i = 0; i < test_TextureSampler_.size(); i++) {
        test_TextureSampler_[i]->Draw();
    }

}

/////////////////////////////////////////////////////////////////////////////////////////
//
//  フレーム開始時の処理
//
/////////////////////////////////////////////////////////////////////////////////////////
void Scene_Game::BeginFrame() {
    Scene_Base::BeginFrame();
    //player_->BeginFrame();
    //playerCorpseManager_->BeginFrame();

    //eggManager_->BeginFrame();
    //stageManager_->BeginFrame();

    if (currentState_) {
        currentState_->BeginFrame();
    }
}


/////////////////////////////////////////////////////////////////////////////////////////
//
//  フレーム終了時の処理
//
/////////////////////////////////////////////////////////////////////////////////////////
void Scene_Game::EndFrame() {

    // 現在のステートがあればフレーム終了処理を行う
    if (currentState_) {
        currentState_->EndFrame();
    }

    // もしstateが変わっていたら以下は処理しない
    if (isStateChanged_) { return; }

    // 各オブジェクトのフレーム終了処理
    //player_->EndFrame();
    //playerCorpseManager_->EndFrame();
    //eggManager_->EndFrame();
    //stageManager_->EndFrame();

}


/////////////////////////////////////////////////////////////////////////////////////////
//
//  すべてのコライダーをコリジョンマネージャに渡す
//
/////////////////////////////////////////////////////////////////////////////////////////
void Scene_Game::HandOverColliders() {

    //if (currentState_) {
    //    currentState_->HandOverColliders();
    //}

    //player_->HandOverColliders();
    //eggManager_->HandOverColliders();
    //stageManager_->HandOverColliders();
}
