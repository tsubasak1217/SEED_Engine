#include <Game/Scene/Scene_Title/Scene_Title.h>
#include <SEED/Source/SEED.h>
#include <Environment/Environment.h>
#include <SEED/Source/Manager/ParticleManager/CPUParticle/ParticleManager.h>
#include <SEED/Source/Manager/CameraManager/CameraManager.h>
#include <SEED/Source/Manager/AudioManager/AudioManager.h>

/////////////////////////////////////////////////////////////////////////////////////////
//
//  コンストラクタ・デストラクタ
//
/////////////////////////////////////////////////////////////////////////////////////////

Scene_Title::Scene_Title() : Scene_Base(){
};

Scene_Title::~Scene_Title(){
    Scene_Base::Finalize();

    // カメラのリセット
    SEED::RemoveCamera("gameCamera");
    SEED::SetMainCamera("default");
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//  初期化
//
/////////////////////////////////////////////////////////////////////////////////////////

void Scene_Title::Initialize(){

    // 共通初期化
    Scene_Base::Initialize();

    // Timerの初期化
    titleStartTimer_.Initialize(2.0f);

    //logoの読み込み
    titleLogo_ = hierarchy_->LoadObject2D("Title/logo.prefab");

    // カメラの設定
    cameraParents_ = hierarchy_->LoadObject("Title/cameraParents.prefab");

    // モデルオブジェクトの読み込み
    models_ = hierarchy_->LoadObject("Title/models.prefab");
    display_ = models_->GetChild("display");

    // UIオブジェクトの読み込み
    hierarchy_->LoadObject2D("Title/pressSpace.prefab");

    // ポストプロセスの初期化
    PostEffectSystem::DeleteAll();
    PostEffectSystem::Load("TitleScene.json");

    // videoPlayerの初期化
    videoPlayer_ = std::make_unique<VideoPlayer>();
}

void Scene_Title::Finalize(){
    Scene_Base::Finalize();
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//  更新処理
//
/////////////////////////////////////////////////////////////////////////////////////////

void Scene_Title::Update(){
    Scene_Base::Update();

    // 開始Timerの更新
    titleStartTimer_.Update();

    // title開始後の処理
    if(titleStartTimer_.IsFinished()){

        // 開始タイマーが終了した直後の処理
        if(titleStartTimer_.IsFinishedNow()){
            rotateRects_ = hierarchy_->LoadObject2D("Title/rotateRects.prefab");
        }

        // 回転処理
        if(rotateRects_){
            int32_t childIndex = 0;
            auto& children = rotateRects_->GetChildren();
            float baseRotateSpeed = 3.14f * 0.125f; // 基本回転速度(秒あたり)
            float addRotateSpeed = 3.14f * 0.0625f; // 追加回転速度(秒あたり)
            // 大きいインデックスほど速く回転する
            for(auto& child : children){
                float rotateSpeed = baseRotateSpeed + (addRotateSpeed * childIndex);
                child->aditionalTransform_.rotate += rotateSpeed * ClockManager::DeltaTime();
                childIndex++;
            }
        }
    }

    // カメラの位置を追従させる
    if(cameraParents_){

        auto children = cameraParents_->GetChildren();

        // カメラの位置を切り替えて追従させる
        if(children.size() > 0){
            int32_t cameraIdx = int32_t(totalTime_ / cameraParentTime_) % int32_t(children.size());
            auto it = children.begin();
            std::advance(it, cameraIdx);
            SEED::GetMainCamera()->SetTransform((*it)->GetWorldTransform());
        }
    }

    // 総経過時間の更新
    totalTime_ += ClockManager::DeltaTime();
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//  描画処理
//
/////////////////////////////////////////////////////////////////////////////////////////

void Scene_Title::Draw(){

    // 共通描画
    Scene_Base::Draw();

}

/////////////////////////////////////////////////////////////////////////////////////////
//
//  フレーム開始時の処理
//
/////////////////////////////////////////////////////////////////////////////////////////
void Scene_Title::BeginFrame(){
    Scene_Base::BeginFrame();
}


/////////////////////////////////////////////////////////////////////////////////////////
//
//  フレーム終了時の処理
//
/////////////////////////////////////////////////////////////////////////////////////////
void Scene_Title::EndFrame(){
    Scene_Base::EndFrame();
}


/////////////////////////////////////////////////////////////////////////////////////////
//
//  すべてのコライダーをコリジョンマネージャに渡す
//
/////////////////////////////////////////////////////////////////////////////////////////
void Scene_Title::HandOverColliders(){
    Scene_Base::HandOverColliders();
}