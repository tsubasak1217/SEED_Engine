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

    // spriteの設定
    sprites_["bg"].size = { 1280.0f,720.0f };
    sprites_["bg"].color = { 1.0f,1.0f,1.0f,1.0f };
    sprites_["bg"].drawLocation = DrawLocation::Back;

    {
        Vector2 anchor = { 0.5f,1.0f };
        float scale = 0.5f;
        titleOffset_ = { 0.0f,-15.0f };

        sprites_["body"].anchorPoint = anchor;
        sprites_["body"].scale = { scale,scale };
        basePos_ = { 640.0f,360.0f + sprites_["body"].size.y * 0.5f * sprites_["body"].scale.y };
        sprites_["body"].translate = basePos_;

        sprites_["rightLeg"].anchorPoint = anchor;
        sprites_["rightLeg"].scale = { scale,scale };
        sprites_["rightLeg"].translate = basePos_;
        sprites_["rightLeg"].uvTransform = ScaleMatrix({ -1.0f,1.0f,1.0f });

        sprites_["leftLeg"].anchorPoint = anchor;
        sprites_["leftLeg"].scale = { scale,scale };
        sprites_["leftLeg"].translate = basePos_;

        sprites_["title"].anchorPoint = {0.5f,0.5f};
        sprites_["title"].scale = { 1.65f,1.3f };
        sprites_["title"].translate = basePos_;

        basePos_ = { 640.0f,421.0f };
        titleOffset_ = { 0.0f,43.0f };

    }
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

    if(Input::IsTriggerPadButton(PAD_BUTTON::A) or Input::IsTriggerKey(DIK_SPACE)){
        ChangeScene("Select");
    }

    // spriteの更新
    UpdateTitleLogo();
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

    sprites_["rightLeg"].translate.x = basePos_.x;
    sprites_["rightLeg"].translate.y = basePos_.y - std::clamp(sin,0.0f,1.0f) * raiseRadius;
    sprites_["leftLeg"].translate.x = basePos_.x;
    sprites_["leftLeg"].translate.y = basePos_.y - std::clamp(-sin, 0.0f, 1.0f) * raiseRadius;
    sprites_["body"].translate.x = basePos_.x;
    sprites_["body"].translate.y = basePos_.y - sin * (sin > 0.0f ? idleRadius : -idleRadius * 0.5f);
    sprites_["title"].translate = basePos_ + titleOffset_;

    logoTimer_ += ClockManager::DeltaTime();


    ImFunc::CustomBegin("Title", MoveOnly_TitleBar);
    {
        ImGui::DragFloat2("titlePos", &sprites_["title"].translate.x);
        ImGui::DragFloat2("titleScale", &sprites_["title"].scale.x,0.05f);
        ImGui::DragFloat2("titleOffset", &titleOffset_.x);
        ImGui::DragFloat2("basePos", &basePos_.x);

        ImGui::End();
    }
}
