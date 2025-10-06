#include "ResultUpdateComponent.h"
#include <Game/Scene/Scene_Clear/Scene_Clear.h>
#include <json.hpp>

ResultUpdate2DComponent::ResultUpdate2DComponent(GameObject2D* pOwner, const std::string& tagName)
    : IComponent(pOwner, tagName){
    // タグ名が空文字列ならデフォルトのタグ名を設定
    if(tagName == ""){
        componentTag_ = "ResultUpdate2D_ID:" + std::to_string(componentID_);
    }

    // timer初期化
    timer_ = TimerArray({ 1.0f,1.0f,2.0f });

    // playResult初期化
    playResult_ = Scene_Clear::GetResult();

    // リザルトシーンの読み込み
    auto* scene = GameSystem::GetScene();
    auto* hierarchy = scene->GetHierarchy();
    hierarchy->LoadFromJson("Resources/Jsons/Scenes/ResultScene.json", false);

    // オブジェクトの取得
    if(auto* obj = GameSystem::GetScene()->GetHierarchy()->GetGameObject2D("Title")){
        uiObjects_["Title"] = obj;
    }
    if(auto* obj = GameSystem::GetScene()->GetHierarchy()->GetGameObject2D("Difficulty")){
        uiObjects_["Difficulty"] = obj;
    }
    if(auto* obj = GameSystem::GetScene()->GetHierarchy()->GetGameObject2D("Score")){
        uiObjects_["Score"] = obj;
    }
    if(auto* obj = GameSystem::GetScene()->GetHierarchy()->GetGameObject2D("Combo")){
        uiObjects_["Combo"] = obj;
    }
    if(auto* obj = GameSystem::GetScene()->GetHierarchy()->GetGameObject2D("Perfect")){
        uiObjects_["Perfect"] = obj;
    }
    if(auto* obj = GameSystem::GetScene()->GetHierarchy()->GetGameObject2D("Great")){
        uiObjects_["Great"] = obj;
    }
    if(auto* obj = GameSystem::GetScene()->GetHierarchy()->GetGameObject2D("Good")){
        uiObjects_["Good"] = obj;
    }
    if(auto* obj = GameSystem::GetScene()->GetHierarchy()->GetGameObject2D("Miss")){
        uiObjects_["Miss"] = obj;
    }
    if(auto* obj = GameSystem::GetScene()->GetHierarchy()->GetGameObject2D("Fast")){
        uiObjects_["Fast"] = obj;
    }
    if(auto* obj = GameSystem::GetScene()->GetHierarchy()->GetGameObject2D("Late")){
        uiObjects_["Late"] = obj;
    }
    if(auto* obj = GameSystem::GetScene()->GetHierarchy()->GetGameObject2D("Bottom")){
        uiObjects_["Bottom"] = obj;
    }
    if(auto* obj = GameSystem::GetScene()->GetHierarchy()->GetGameObject2D("Top")){
        uiObjects_["Top"] = obj;
    }
    if(auto* obj = GameSystem::GetScene()->GetHierarchy()->GetGameObject2D("Jacket")){
        uiObjects_["Jacket"] = obj;
    }

    /*------- 特定オブジェクトを非表示に --------*/
    // スコア
    uiObjects_["Score"]->GetComponent<UIComponent>()->GetText(0).text = "";
    // late
    uiObjects_["Late"]->GetComponent<UIComponent>()->SetMasterColor({ 1.0f,1.0f,1.0f,0.0f });
    // fast
    uiObjects_["Fast"]->GetComponent<UIComponent>()->SetMasterColor({ 1.0f,1.0f,1.0f,0.0f });
    // combo
    uiObjects_["Combo"]->GetComponent<UIComponent>()->SetMasterColor({ 1.0f,1.0f,1.0f,0.0f });

}

void ResultUpdate2DComponent::BeginFrame(){
}

void ResultUpdate2DComponent::Update(){

    // 初回更新時は処理しない
    if(isFirstUpdate_){
        isFirstUpdate_ = false;
        return;
    }

    // 一度のみの初期化処理
    if(!isInitialized_){
        // 曲名
        uiObjects_["Title"]->GetComponent<AnimCurve2DComponent>()->Play();
        uiObjects_["Title"]->GetComponent<UIComponent>()->GetText(0).text = playResult_.songData.value("songName","none");
        // 上から出てくるやつ
        uiObjects_["Top"]->GetComponent<AnimCurve2DComponent>()->Play();
        // 下から出てくるやつ
        uiObjects_["Bottom"]->GetComponent<AnimCurve2DComponent>()->Play();
        // jacket
        uiObjects_["Jacket"]->GetComponent<AnimCurve2DComponent>()->Play();
        uiObjects_["Jacket"]->GetComponent<UIComponent>()->GetSprite(0).GH = TextureManager::LoadTexture(Scene_Clear::GetJacketPath());
        isInitialized_ = true;
    }

    // タイマーが切り替わった際の処理
    if(timer_.IsFinishedNow() != -1){
        int32_t index = timer_.IsFinishedNow();
        switch(index){
        case 0:
        {
            // 難易度
            uiObjects_["Difficulty"]->GetComponent<AnimCurve2DComponent>()->Play();
            int difficulty = playResult_.songData.value("difficulty",0);
            uiObjects_["Difficulty"]->GetComponent<UIComponent>()->GetText(0).text = std::to_string(difficulty);
            // 各判定
            uiObjects_["Perfect"]->GetComponent<AnimCurve2DComponent>()->Play();
            uiObjects_["Perfect"]->GetComponent<UIComponent>()->GetText(1).text = "%d";
            uiObjects_["Perfect"]->GetComponent<UIComponent>()->GetText(1).BindDatas({ displayResult_.evalutionCount[0] });
            uiObjects_["Great"]->GetComponent<AnimCurve2DComponent>()->Play();
            uiObjects_["Great"]->GetComponent<UIComponent>()->GetText(1).text = "%d";
            uiObjects_["Great"]->GetComponent<UIComponent>()->GetText(1).BindDatas({ displayResult_.evalutionCount[1] });
            uiObjects_["Good"]->GetComponent<AnimCurve2DComponent>()->Play();
            uiObjects_["Good"]->GetComponent<UIComponent>()->GetText(1).text = "%d";
            uiObjects_["Good"]->GetComponent<UIComponent>()->GetText(1).BindDatas({ displayResult_.evalutionCount[2] });
            uiObjects_["Miss"]->GetComponent<AnimCurve2DComponent>()->Play();
            uiObjects_["Miss"]->GetComponent<UIComponent>()->GetText(1).text = "%d";
            uiObjects_["Miss"]->GetComponent<UIComponent>()->GetText(1).BindDatas({ displayResult_.evalutionCount[3] });
            // コンボ
            uiObjects_["Combo"]->GetComponent<UIComponent>()->GetText(1).text = "%d";
            uiObjects_["Combo"]->GetComponent<UIComponent>()->GetText(1).BindDatas({ playResult_.maxCombo });
            // Fast
            uiObjects_["Fast"]->GetComponent<UIComponent>()->GetText(1).text = "%d";
            uiObjects_["Fast"]->GetComponent<UIComponent>()->GetText(1).BindDatas({ playResult_.fastCount });
            // Late
            uiObjects_["Late"]->GetComponent<UIComponent>()->GetText(1).text = "%d";
            uiObjects_["Late"]->GetComponent<UIComponent>()->GetText(1).BindDatas({ playResult_.lateCount });

            break;
        }
        case 1:
            // スコア
            uiObjects_["Score"]->GetComponent<UIComponent>()->GetText(0).text = "%f%";
            uiObjects_["Score"]->GetComponent<UIComponent>()->GetText(0).BindDatas({ displayResult_.score });

            break;
        default:
            break;
        }
    }

    // タイマーの段階に応じた処理
    int32_t currentPhase = timer_.GetCurrentIndex();
    if(currentPhase >= 1){
        // 各判定数をカウントアップ
        for(int i = 0; i < 4; i++){
            displayResult_.evalutionCount[i] =
                static_cast<int32_t>(
                    MyMath::Lerp(
                        static_cast<float>(displayResult_.evalutionCount[i]),
                        static_cast<float>(playResult_.evalutionCount[i]),
                        timer_.GetProgress(1)
                    )
                    );
        }

        // コンボ, Fast, Lateを徐々に表示
        uiObjects_["Combo"]->GetComponent<UIComponent>()->SetMasterColor({ 1.0f,1.0f,1.0f,timer_.GetProgress(1) });
        uiObjects_["Fast"]->GetComponent<UIComponent>()->SetMasterColor({ 1.0f,1.0f,1.0f,timer_.GetProgress(1) });
        uiObjects_["Late"]->GetComponent<UIComponent>()->SetMasterColor({ 1.0f,1.0f,1.0f,timer_.GetProgress(1) });

    }

    if(currentPhase >= 2){
        // 表示用のスコアをカウントアップ
        displayResult_.score = MyMath::Lerp(displayResult_.score, static_cast<float>(playResult_.score), timer_.GetProgress(2));
    }

    // タイマーの更新
    timer_.Update();
}

void ResultUpdate2DComponent::Draw(){
}

void ResultUpdate2DComponent::EndFrame(){
}

void ResultUpdate2DComponent::Finalize(){
}

void ResultUpdate2DComponent::EditGUI(){
}