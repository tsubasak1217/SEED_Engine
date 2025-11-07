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
    titleEndTimer_.Initialize(0.5f);

    // videoPlayerの初期化
    videoPlayer_ = std::make_unique<VideoPlayer>("Tutorials/tmp.mp4", false);
    videoPlayer_->Play(0.0f, 1.0f, true);

    //logoの読み込み
    titleLogo_ = hierarchy_->LoadObject2D("Title/logo.prefab");

    // カメラ関連
    cameraParents_ = hierarchy_->LoadObject("Title/cameraParents.prefab");
    toSelectCamerawork_ = hierarchy_->LoadObject("Title/toSelectCamerawork.prefab");
    toSelectCamerawork_->GetComponent<Routine3DComponent>()->Pause();

    // 何も見えない場所にカメラを移動しておく
    Transform firstCameraTransform;
    firstCameraTransform.translate = Vector3(0.0f, -10000.0f, 0.0f);
    SEED::GetMainCamera()->SetTransform(firstCameraTransform);

    // モデルオブジェクトの読み込み
    models_ = hierarchy_->LoadObject("Title/models.prefab");
    display_ = models_->GetChild("display");


    // displayに動画テクスチャを設定
    display_->GetComponent<ModelRenderComponent>()->GetModel()->materials_[0].GH = videoPlayer_->GetVideoGH();

    // 背景オブジェクトの読み込み
    rotateRects_ = hierarchy_->LoadObject2D("Title/rotateRects.prefab");

    // "spaceキーで進む"旨のオブジェクトを読み込む
    pressSpace_ = hierarchy_->LoadObject2D("Title/pressSpace.prefab");
    pressSpace_->isActive_ = false;
    pressSpace_->aditionalTransform_.scale = Vector2(0.0f);
    pressSpace_->Update();

    // 開始時フェード
    hierarchy_->LoadObject2D("Title/titleStart.prefab");

    // ポストプロセスの初期化
    PostEffectSystem::DeleteAll();
    PostEffectSystem::Load("TitleScene.json");

    // 入力の初期化
    startButtonInput_.Trigger = []{
        return Input::IsTriggerKey(DIK_SPACE);
    };
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

    // 背景の矩形の回転処理
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

    // title開始後の処理
    if(titleStartTimer_.IsFinished()){

        // 開始タイマーが終了した直後の処理
        if(titleStartTimer_.IsFinishedNow()){
            pressSpace_->isActive_ = true;
        }

        // スタートボタンが押されていない場合の処理
        if(!isStartButtonPressed_){

            // 様々な位置でカメラの位置を追従させる
            if(cameraParents_){

                auto children = cameraParents_->GetChildren();

                // カメラの位置を切り替えて追従させる
                if(children.size() > 0){
                    int32_t cameraIdx = int32_t(totalTime_ / cameraParentTime_) % int32_t(children.size());
                    auto it = children.begin();
                    std::advance(it, cameraIdx);
                    SEED::GetMainCamera()->SetTransform((*it)->GetWorldTransform());

                    // ルーチンを最初から再生し直す(カメラが切り替わった瞬間)
                    if(prevCameraParentIdx_ != cameraIdx){
                        (*it)->GetComponent<Routine3DComponent>()->Reset();
                    }

                    // 前のカメラ番号を保存
                    prevCameraParentIdx_ = cameraIdx;
                }
            }

            // スタートボタンが押されたら移行フラグを立てる
            if(startButtonInput_.Trigger()){
                isStartButtonPressed_ = true;
                if(toSelectCamerawork_){
                    // カメラワークルーチンを初期位置に
                    toSelectCamerawork_->GetComponent<Routine3DComponent>()->Play();
                    // フェードオブジェクトを読み込む
                    hierarchy_->LoadObject2D("Title/fade.prefab")->masterColor_ = Color(0.0f);
                }
            }

        } else{
            // シーン遷移用のカメラワークを行う
            if(toSelectCamerawork_){
                SEED::GetMainCamera()->SetTransform(toSelectCamerawork_->GetWorldTransform());
            }

            // ロゴやUIのフェードアウト処理
            titleLogo_->masterColor_.value.w = 1.0f - titleEndTimer_.GetProgress();
            pressSpace_->masterColor_.value.w = 1.0f - titleEndTimer_.GetProgress();

            // 終了タイマーの更新
            titleEndTimer_.Update();
        }

        if(pressSpace_){
            float waitTime = 1.0f;
            float appearTime = 1.0f;
            float t = std::clamp((totalTime_ - waitTime) / appearTime, 0.0f, 1.0f);
            float ease = EaseInOutExpo(t);
            pressSpace_->aditionalTransform_.scale = Vector2(ease);
        }

        // 総経過時間の更新
        totalTime_ += ClockManager::DeltaTime();
    }
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

    // カメラワークルーチンが終了したらシーン遷移
    if(toSelectCamerawork_){
        if(toSelectCamerawork_->GetComponent<Routine3DComponent>()->IsEndRoutine()){
            ChangeScene("Game");
            return;
        }
    }
}


/////////////////////////////////////////////////////////////////////////////////////////
//
//  すべてのコライダーをコリジョンマネージャに渡す
//
/////////////////////////////////////////////////////////////////////////////////////////
void Scene_Title::HandOverColliders(){
    Scene_Base::HandOverColliders();
}