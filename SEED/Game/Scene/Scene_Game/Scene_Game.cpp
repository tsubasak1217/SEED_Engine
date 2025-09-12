#include <Game/Scene/Scene_Game/Scene_Game.h>
#include <SEED/Source/SEED.h>
#include <Environment/Environment.h>
#include <SEED/Source/Manager/EffectSystem/EffectSystem.h>
#include <SEED/Source/Manager/CameraManager/CameraManager.h>
#include <SEED/Source/Manager/AudioManager/AudioManager.h>

// state
#include <Game/Scene/Scene_Game/State/GameState_Play.h>
#include <Game/Scene/Scene_Game/State/GameState_Pause.h>
#include <Game/Scene/Scene_Game/State/GameState_Clear.h>

/////////////////////////////////////////////////////////////////////////////////////////
//
//  コンストラクタ・デストラクタ
//
/////////////////////////////////////////////////////////////////////////////////////////

Scene_Game::Scene_Game() : Scene_Base(){
    Input::SetIsActive(false);
};

Scene_Game::~Scene_Game(){

    Finalize();
    SEED::RemoveCamera("gameCamera");
    SEED::SetMainCamera("default");
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//  初期化
//
/////////////////////////////////////////////////////////////////////////////////////////

void Scene_Game::Initialize(){

    // パーティクルの初期化
    EffectSystem::DeleteAll();

    ////////////////////////////////////////////////////
    // State初期化
    ////////////////////////////////////////////////////

    SEED::SetSkyBox("DefaultAssets/CubeMaps/rostock_laage_airport_4k.dds");

    // Playステートに初期化
    ChangeState(new GameState_Play(this));

    if(currentState_){
        currentState_->Initialize();
    }

    ////////////////////////////////////////////////////
    //  カメラ初期化
    ////////////////////////////////////////////////////

    auto* scene = this;
    scene;

    ////////////////////////////////////////////////////
    //  ライトの初期化
    ////////////////////////////////////////////////////

    directionalLight_ = std::make_unique<DirectionalLight>();
    directionalLight_->direction_ = { -0.5f,-1.0f,0.0f };

    ////////////////////////////////////////////////////
    //  オブジェクトの初期化
    ////////////////////////////////////////////////////


    ////////////////////////////////////////////////////
    // スプライトの初期化
    ////////////////////////////////////////////////////

    // UI
    UIFromJson();

    //========================================================================
    //	ステージ
    //========================================================================

    stage_ = std::make_unique<GameStage>();
    stage_->Initialize(currentStageIndex_);
    // 画面が切り替わったらポーズ解除
    stage_->SetIsPaused(false);
    maxStageCount_ = stage_->GetMaxStageCount();

    ////////////////////////////////////////////////////
    // Audio の 初期化
    ////////////////////////////////////////////////////


    ////////////////////////////////////////////////////
    //  他クラスの情報を必要とするクラスの初期化
    ////////////////////////////////////////////////////



    /////////////////////////////////////////////////
    //  関連付けや初期値の設定
    /////////////////////////////////////////////////

}

void Scene_Game::Finalize(){
    Scene_Base::Finalize();
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//  更新処理
//
/////////////////////////////////////////////////////////////////////////////////////////

void Scene_Game::Update(){

    /*======================= 各状態固有の更新 ========================*/

    // ヒエラルキー内のオブジェクトの更新
    hierarchy_->Update();

    if(currentState_){
        currentState_->Update();
    }

    if(currentEventState_){
        currentEventState_->Update();
    }

    /*==================== 各オブジェクトの基本更新 =====================*/

    //========================================================================
    //	ステージ
    //========================================================================
    if(dynamic_cast<GameState_Play*>(currentState_.get())){

        stage_->Update();
    } else{

        stage_->Update();
    }

    stage_->Edit();
    SceneEdit();

    //ステートクラス内の遷移処理を実行
    ManageState();
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//  描画処理
//
/////////////////////////////////////////////////////////////////////////////////////////

void Scene_Game::Draw(){

    /*======================= 各状態固有の描画 ========================*/

    if(currentEventState_){
        currentEventState_->Draw();
    }

    if(currentState_){
        currentState_->Draw();
    }

    /*==================== 各オブジェクトの基本描画 =====================*/

    // ヒエラルキー内のオブジェクトの描画
    hierarchy_->Draw();

    // ライトをセット
    directionalLight_->SendData();

    // ステージ
    stage_->Draw();

    // UI描画
    DrawUI();
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//  フレーム開始時の処理
//
/////////////////////////////////////////////////////////////////////////////////////////
void Scene_Game::BeginFrame(){

    // ヒエラルキー内のオブジェクトの描画
    hierarchy_->BeginFrame();

    // 現在のステートがあればフレーム開始処理を行う
    if(currentState_){
        currentState_->BeginFrame();
    }

    // 入力デバイスが変更されたらUIを再読み込み
    if(Input::IsChangedInputDevice()){
        UIFromJson();
    }
}


/////////////////////////////////////////////////////////////////////////////////////////
//
//  フレーム終了時の処理
//
/////////////////////////////////////////////////////////////////////////////////////////
void Scene_Game::EndFrame(){

    // シーン開始時にホログラムオブジェクトの削除を行う
    stage_->RemoveBorderLine();
    stage_->Reset();

    // ヒエラルキー内のオブジェクトのフレーム終了処理
    hierarchy_->EndFrame();

    // 現在のステートがあればフレーム終了処理を行う
    if(currentState_){
        currentState_->EndFrame();
    }
}


/////////////////////////////////////////////////////////////////////////////////////////
//
//  すべてのコライダーをコリジョンマネージャに渡す
//
/////////////////////////////////////////////////////////////////////////////////////////
void Scene_Game::HandOverColliders(){

    if(currentState_){
        currentState_->HandOverColliders();
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
// 
//  編集関数
// 
/////////////////////////////////////////////////////////////////////////////////////////
void Scene_Game::SceneEdit(){
#ifdef _DEBUG
    ImFunc::CustomBegin("Scene_Game", MoveOnly_TitleBar);
    {

        if(ImGui::Button("Add UI")){
            uiSprites_.push_back({ Sprite("UI/buttons.png"), false });
        }

        if(ImGui::Button("Add UIText")){
            uiTexts_.push_back({ TextBox2D("Text"), false });
        }

        ImGui::Separator();

        ImGui::Text("UI一覧");
        for(int i = 0; i < uiSprites_.size(); i++){
            if(ImGui::CollapsingHeader(("UI" + std::to_string(i)).c_str())){
                ImGui::Indent();
                uiSprites_[i].first.Edit();

                ImGui::Text("削除");
                if(ImGui::Button("Delete")){
                    uiSprites_[i].second = true;
                    break;
                }
                ImGui::Unindent();
            }
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Text("UIText一覧");
        for(int i = 0; i < uiTexts_.size(); i++){
            if(ImGui::CollapsingHeader(("UIText" + std::to_string(i)).c_str())){
                ImGui::Indent();
                uiTexts_[i].first.Edit();
                ImGui::Text("削除");
                if(ImGui::Button("Delete")){
                    uiTexts_[i].second = true;
                    break;
                }
                ImGui::Unindent();
            }
        }

        // 削除フラグが立っているものを削除
        uiSprites_.erase(std::remove_if(uiSprites_.begin(), uiSprites_.end(),
            [](const std::pair<Sprite, bool>& item){ return item.second; }),
            uiSprites_.end());
        uiTexts_.erase(std::remove_if(uiTexts_.begin(), uiTexts_.end(),
            [](const std::pair<TextBox2D, bool>& item){ return item.second; }),
            uiTexts_.end());

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // json保存
        if(ImGui::Button("UI to json")){
            UIToJson();
        }

        ImGui::End();
    }

#endif // _DEBUG
}

/////////////////////////////////////////////////////////////////////////////////////////
//  UI描画
/////////////////////////////////////////////////////////////////////////////////////////
void Scene_Game::DrawUI(){

    // UIタイマー更新
    if(dynamic_cast<GameState_Play*>(currentState_.get())){
        uiTimer_.Update();
    } else{
        uiTimer_.Update(-2.0f);
    }


    for(auto& uiSprite : uiSprites_){
        uiSprite.first.color.w = uiTimer_.GetProgress();
        uiSprite.first.Draw();
    }

    // UIテキスト描画
    for(auto& uiText : uiTexts_){
        uiText.first.color.w = uiTimer_.GetProgress();
        uiText.first.Draw();
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
// json
/////////////////////////////////////////////////////////////////////////////////////////
void Scene_Game::UIToJson(){
    nlohmann::json data;

    for(auto& uiSprite : uiSprites_){
        data["Sprites"].push_back(uiSprite.first.ToJson());
    }

    for(auto& uiText : uiTexts_){
        data["Texts"].push_back(uiText.first.GetJsonData());
    }

    bool isPad = Input::IsPressKey(DIK_P);
    if(isPad){
        MyFunc::CreateJsonFile("Resources/Jsons/Scene_Game/GameUI_Pad.json", data);
    } else{
        MyFunc::CreateJsonFile("Resources/Jsons/Scene_Game/GameUI.json", data);
    }
}

void Scene_Game::UIFromJson(){
    bool isPad = Input::GetRecentInputDevice() == InputDevice::GAMEPAD;
    nlohmann::json data;
    if(isPad){
        data = MyFunc::GetJson("Resources/Jsons/Scene_Game/GameUI_Pad.json");
    } else{
        data = MyFunc::GetJson("Resources/Jsons/Scene_Game/GameUI.json");
    }

    uiSprites_.clear();
    uiTexts_.clear();

    if(data.contains("Sprites")){
        for(auto& spriteData : data["Sprites"]){
            Sprite sprite;
            sprite.FromJson(spriteData);
            uiSprites_.push_back({ sprite,false });
        }
    }

    if(data.contains("Texts")){
        for(auto& textData : data["Texts"]){
            TextBox2D text;
            text.LoadFromJson(textData);
            uiTexts_.push_back({ text,false });
        }
    }
}