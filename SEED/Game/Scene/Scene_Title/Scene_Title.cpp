#include <Game/Scene/Scene_Title/Scene_Title.h>
#include <Game/Scene/Scene_Game/Scene_Game.h>
#include <SEED/Source/SEED.h>
#include <Environment/Environment.h>
#include <SEED/Source/Manager/ParticleManager/CPUParticle/ParticleManager.h>
#include <SEED/Source/Manager/CameraManager/CameraManager.h>
#include <SEED/Source/Manager/AudioManager/AudioManager.h>
#include <SEED/Source/Basic/SceneTransition/HexagonTransition.h>

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
    sceneOutTimer_.Initialize(1.0f,0.0f,true);

    // videoPlayerの初期化
    videoPlayer_ = std::make_unique<VideoPlayer>("demoMovie.mp4", false);
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
    pressSpace_->SetIsActive(false);
    pressSpace_->aditionalTransform_.scale = Vector2(0.0f);
    pressSpace_->Update();

    // 開始時フェード
    hierarchy_->LoadObject2D("Title/titleStart.prefab");

    // BGM読み込み
    AudioManager::LoadAudio("BGM/NETHMi_title.wav");

    // ポストプロセスの初期化
    PostEffectSystem::DeleteAll();
    PostEffectSystem::Load("TitleScene.json");

    // 入力の初期化
    decideButtonInput_.Trigger = []{
        return Input::IsTriggerKey(DIK_SPACE);
    };

    tutorialSelectInput_.Trigger = []{
        return Input::IsTriggerKey({ DIK_A,DIK_D,DIK_LEFT,DIK_RIGHT });
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
            pressSpace_->SetIsActive(true);
            bgmHandle_ = AudioManager::PlayAudio(AudioDictionary::Get("TitleBGM"), true, 0.5f);
            cameraParents_->GetChild(0)->GetComponent<Routine3DComponent>()->Reset();
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
            if(decideButtonInput_.Trigger()){
                isStartButtonPressed_ = true;
                if(toSelectCamerawork_){
                    // カメラワークルーチンを初期位置に
                    toSelectCamerawork_->GetComponent<Routine3DComponent>()->Play();
                    // フェードオブジェクトを読み込む
                    hierarchy_->LoadObject2D("Title/fade.prefab")->masterColor_ = Color(0.0f);
                }

                // 音声再生
                AudioManager::PlayAudio(AudioDictionary::Get("DecideTitle"), false, 0.5f);
            }

        } else{
            // シーン遷移用のカメラワークを行う
            if(toSelectCamerawork_){
                SEED::GetMainCamera()->SetTransform(toSelectCamerawork_->GetWorldTransform());

                // カメラワークが終了したらチュートリアル選択UIを表示開始
                auto* routine = toSelectCamerawork_->GetComponent<Routine3DComponent>();
                if(routine->IsEndRoutine()){
                    if(!tutorialSelectItems_){
                        // チュートリアル選択UIを読み込む
                        tutorialSelectItems_ = hierarchy_->LoadObject2D("Title/tutorialSelectItems.prefab");
                        // "はい"項目をアクティブ化
                        auto* item_yes = tutorialSelectItems_->GetChild(1);
                        item_yes->GetComponent<Routine2DComponent>()->Play();// アニメーションを再生
                        // "いいえ"項目を非アクティブ化
                        auto* item_no = tutorialSelectItems_->GetChild(2);
                        item_no->masterColor_ = Color(0.5f, 0.5f, 0.5f, 1.0f);// "いいえ"を灰色に
                        item_no->GetComponent<Routine2DComponent>()->Play();// アニメーションを再生
                        item_no->GetComponent<Routine2DComponent>()->RevercePlay();// アニメーションを逆再生に
                        item_no->GetComponent<ColorControlComponent>()->SetIsActive(false);// 色変化を無効化
                        // 最初はscale0にしておく
                        item_yes->aditionalTransform_.scale = Vector2(0.0f);
                        item_no->aditionalTransform_.scale = Vector2(0.0f);
                        item_yes->UpdateMatrix();
                        item_no->UpdateMatrix();
                    } else{
                        if(sceneOutTimer_.isStop){
                            // チュートリアル選択処理
                            SelectTutorial();
                        } else{
                            // シーン遷移処理
                            sceneOutTimer_.Update();
                            if(sceneOutTimer_.IsFinishedNow()){
                                sceneChangeOrder_ = true;
                            }
                        }
                    }
                }

                // 音量を設定
                if(routine->GetTimer().GetProgress() > 0.7f){
                    float volumeRate = 1.0f - (routine->GetTimer().GetProgress() - 0.7f) / 0.3f;
                    AudioManager::SetAudioVolume(bgmHandle_, 0.5f * volumeRate);
                }
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
        if(sceneChangeOrder_){
            Scene_Game::SetIsPlayTutorial(isPlayTutorial_);
            AudioManager::EndAudio(bgmHandle_);
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


/////////////////////////////////////////////////////////////////////////////////////////
//
//  チュートリアル選択処理
//
/////////////////////////////////////////////////////////////////////////////////////////
void Scene_Title::SelectTutorial(){

    // オブジェクトの取得
    auto* item_yes = tutorialSelectItems_->GetChild(1);
    auto* item_no = tutorialSelectItems_->GetChild(2);

    // チュートリアル選択UIの拡大処理
    float startTime = 0.3f; // 選択開始時の時間
    float waitTime = 0.5f; // 選択開始までの待機時間
    float t = std::clamp((tutorialSelectItems_->GetAliveTime() - waitTime) / startTime, 0.0f, 1.0f);
    float ease = EaseOutExpo(t);
    item_yes->aditionalTransform_.scale = Vector2(ease);
    item_no->aditionalTransform_.scale = Vector2(ease);

    // waitTime中は入力受付しない
    if(t == 0.0f){
        return;
    }

    // 選択項目の切り替え処理
    if(tutorialSelectInput_.Trigger()){

        // チュートリアル選択状態を反転
        isPlayTutorial_ = !isPlayTutorial_;

        // "はい"項目の処理
        if(isPlayTutorial_){
            item_yes->GetComponent<ColorControlComponent>()->SetIsActive(true);// 色変化を有効化
            item_no->GetComponent<ColorControlComponent>()->SetIsActive(false);// 色変化を無効化
            item_no->masterColor_ = Color(0.5f, 0.5f, 0.5f, 1.0f);// "いいえ"を灰色に

        } else{// "いいえ"項目の処理
            item_yes->GetComponent<ColorControlComponent>()->SetIsActive(false);// 色変化を無効化
            item_no->GetComponent<ColorControlComponent>()->SetIsActive(true);// 色変化を有効化
            item_yes->masterColor_ = Color(0.5f, 0.5f, 0.5f, 1.0f);// "はい"を灰色に
        }

        // アニメーションを逆再生
        item_yes->GetComponent<Routine2DComponent>()->RevercePlay();
        item_no->GetComponent<Routine2DComponent>()->RevercePlay();

        // 音声再生
        AudioManager::PlayAudio(AudioDictionary::Get("TutorialSelect"), false, 0.5f);
    }

    // 決定処理
    if(decideButtonInput_.Trigger()){
        // シーン遷移フラグを立てる
        sceneOutTimer_.Restart();
        auto* transition = SceneTransitionDrawer::AddTransition<HexagonTransition>();
        transition->SetHexagonInfo(32.0f);
        transition->StartTransition(sceneOutTimer_.duration - ClockManager::DeltaTime() , 1.0f);

        // 音声再生
        AudioManager::PlayAudio(AudioDictionary::Get("DecideTutorial"), false, 0.5f);
    }
}
