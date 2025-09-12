#include <Game/Scene/Scene_Title/Scene_Title.h>

//lib
#include <SEED/Source/Manager/InputManager/InputManager.h>

////////////////////////////////////////////////////////////////////////////////////////////////
//
// コンストラクタ・デストラクタ
//
//////////////////////////////////////////////////////////////////////////////////////////////
Scene_Title::Scene_Title(){
    SEED::SetMainCamera("default");
    PostEffectSystem::DeleteAll();
    PostEffectSystem::Load("gj3_defaultPostEffect.json");

    // 再生
    bgmHandle_ = AudioManager::PlayAudio(AudioDictionary::Get("タイトルシーン_BGM"), true, kBgmVolume_);

}

Scene_Title::~Scene_Title(){}

////////////////////////////////////////////////////////////////////////////////////////////////
//
// 初期化
//
//////////////////////////////////////////////////////////////////////////////////////////////
void Scene_Title::Initialize(){

    // spriteの初期化
    sprites_["body"] = Sprite("Scene_Title/titleLogo_Body.png");
    sprites_["rightLeg"] = Sprite("Scene_Title/titleLogo_Leg.png");
    sprites_["leftLeg"] = Sprite("Scene_Title/titleLogo_Leg.png");
    sprites_["title"] = Sprite("Scene_Title/titleLogo_Title.png");
    sprites_["bg"] = Sprite("Scene_Title/tempBG.png");
    sprites_["A"] = Sprite("UI/Buttons.png");
    sprites_["Space"] = Sprite("UI/Buttons.png");

    // spriteの設定
    sprites_["bg"].size = { 1280.0f,720.0f };
    sprites_["bg"].color = { 1.0f,1.0f,1.0f,1.0f };
    sprites_["bg"].drawLocation = DrawLocation::Back;

    {
        Vector2 anchor = { 0.5f,1.0f };
        float scale = 0.5f;
        titleOffset_ = { 0.0f,-15.0f };

        sprites_["body"].anchorPoint = anchor;
        sprites_["body"].transform.scale = { scale,scale };
        basePos_ = { 640.0f,360.0f + sprites_["body"].size.y * 0.5f * sprites_["body"].transform.scale.y };
        sprites_["body"].transform.translate = basePos_;

        sprites_["rightLeg"].anchorPoint = anchor;
        sprites_["rightLeg"].transform.scale = { scale,scale };
        sprites_["rightLeg"].transform.translate = basePos_;
        sprites_["rightLeg"].uvTransform.scale = { -1.0f,1.0f };

        sprites_["leftLeg"].anchorPoint = anchor;
        sprites_["leftLeg"].transform.scale = { scale,scale };
        sprites_["leftLeg"].transform.translate = basePos_;

        sprites_["title"].anchorPoint = { 0.5f,0.5f };
        sprites_["title"].transform.scale = { 2.75f,2.1f };
        sprites_["title"].transform.translate = basePos_;

        basePos_ = { 640.0f,493.0f };
        titleOffset_ = { 0.0f,-261.0f };

    }

    for(auto& [name, sprite] : sprites_){
        sprite.isApplyViewMat = false;
    }

    // textの初期化
    uiText_ = TextBox2D("[A] or [Space]");
    nlohmann::json uijson = MyFunc::GetJson("Resources/Jsons/Scene_Title/UIText.json");
    if(not uijson.is_null()){
        uiText_.LoadFromJson(uijson);
    }

    // ボタンの初期化
    nlohmann::json buttonJson = MyFunc::GetJson("Resources/Jsons/Scene_Title/TitleButtons.json");
    if(buttonJson.is_array() and buttonJson.size() >= 2){
        sprites_["A"].FromJson(buttonJson[0]);
        sprites_["Space"].FromJson(buttonJson[1]);
    }

    // 色の初期化
    hexagonColors_.push_back(MyMath::FloatColor(255,118,11,255,false));
    hexagonColors_.push_back(MyMath::FloatColor(10,10,10,255, false));
    hexagonColors_.push_back(MyMath::FloatColor(255,13,86,255, false));
    hexagonSize_ = 46.0f;

    // spriteの更新
    UpdateTitleLogo();
    UpdateButtonSprites();
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
// 終了処理
//
//////////////////////////////////////////////////////////////////////////////////////////////
void Scene_Title::Finalize(){
    if(currentState_){
        currentState_->Finalize();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
// 更新
//
//////////////////////////////////////////////////////////////////////////////////////////////
void Scene_Title::Update(){
    if(currentState_){
        currentState_->Update();
    }

    if(currentEventState_){
        currentEventState_->Update();
    }

    // シーン開始してからのタイマー
    sceneStartTimer_.Update();

    // シーン遷移タイマー
    if(isExitScene_){
        transisitionTimer_.Update();
        UpdateAudioVolume();
    }

    // spriteの更新
    UpdateTitleLogo();
    UpdateButtonSprites();

    if(Input::IsTriggerPadButton(PAD_BUTTON::A) or Input::IsTriggerKey(DIK_SPACE)){

        // 遷移処理開始
        static bool isFirstTransition = true;
        float nextSceneEnterTime = 0;

        // 最初の遷移時は次のシーン開始時間を長く取る(読み込み対策)
        if(isFirstTransition){
            nextSceneEnterTime = nextSceneStartTime_ + 0.5f;
            isFirstTransition = false;
        } else{
            nextSceneEnterTime = nextSceneStartTime_;
        }

        if(!isExitScene_){
            isExitScene_ = true;
            HexagonTransition* transition = SceneTransitionDrawer::AddTransition<HexagonTransition>();
            transition->SetHexagonInfo(hexagonSize_, hexagonColors_);
            transition->StartTransition(transisitionTimer_.GetDuration(), nextSceneEnterTime);

            // SE
            const float kSEVolume = 0.24f;
            AudioManager::PlayAudio(AudioDictionary::Get("タイトル_決定"), false, kSEVolume);
        }
    }


    // タイマーが終わったらシーン遷移
    if(transisitionTimer_.IsFinished()){
        AudioManager::EndAudio(bgmHandle_);
        ChangeScene("Select");
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
// 描画
//
//////////////////////////////////////////////////////////////////////////////////////////////
void Scene_Title::Draw(){
    //===================== state =====================//
    if(currentState_){
        currentState_->Draw();
    }

    if(currentEventState_){
        currentEventState_->Draw();
    }

    //===================== sprite =====================//
    for(auto& [name, sprite] : sprites_){
        sprite.Draw();
    }

    //===================== text =====================//
    uiText_.Draw();
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
// フレーム開始処理
//
//////////////////////////////////////////////////////////////////////////////////////////////
void Scene_Title::BeginFrame(){
    Scene_Base::BeginFrame();

    if(currentState_){
        currentState_->BeginFrame();
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
// フレーム終了処理
//
//////////////////////////////////////////////////////////////////////////////////////////////
void Scene_Title::EndFrame(){
    //===================== state =====================//
    if(currentState_){
        currentState_->EndFrame();
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//  すべてのコライダーをコリジョンマネージャに渡す
//
/////////////////////////////////////////////////////////////////////////////////////////
void Scene_Title::HandOverColliders(){
    if(currentState_){
        currentState_->HandOverColliders();
    }
}


/////////////////////////////////////////////////////////////////////////////////////////
//
// タイトルロゴの更新
//
/////////////////////////////////////////////////////////////////////////////////////////
void Scene_Title::UpdateTitleLogo(){

    static float raiseRadius = 32.0f;
    static float idleRadius = 8.0f;
    static float timeSpeed = 1.6f;
    float sin = std::sin(logoTimer_ * 3.14f * timeSpeed);

    sprites_["rightLeg"].transform.translate.x = basePos_.x;
    sprites_["rightLeg"].transform.translate.y = basePos_.y - std::clamp(sin, 0.0f, 1.0f) * raiseRadius;
    sprites_["leftLeg"].transform.translate.x = basePos_.x;
    sprites_["leftLeg"].transform.translate.y = basePos_.y - std::clamp(-sin, 0.0f, 1.0f) * raiseRadius;
    sprites_["body"].transform.translate.x = basePos_.x;
    sprites_["body"].transform.translate.y = basePos_.y - sin * (sin > 0.0f ? idleRadius : -idleRadius * 0.5f);
    sprites_["title"].transform.translate = basePos_ + titleOffset_;

    logoTimer_ += ClockManager::DeltaTime();
}


void Scene_Title::UpdateButtonSprites(){

    static float timer = 0.0f;
    float sin = std::sinf(timer * 3.14f * 0.5f);

    // スケールを変える
    float scale = 1.0f + sin * (sin > 0.0f ? 0.25f : -0.25f);
    sprites_["A"].transform.scale = { scale,scale };
    sprites_["Space"].transform.scale = { scale,scale };

    // 時間の更新
    timer += ClockManager::DeltaTime();
}

void Scene_Title::UpdateAudioVolume(){
    if(bgmHandle_){
        AudioManager::SetAudioVolume(bgmHandle_, kBgmVolume_ * (1.0f - transisitionTimer_.GetProgress()));
    }
}
