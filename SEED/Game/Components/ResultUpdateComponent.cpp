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
    timer_ = TimerArray({ 1.0f,1.0f,1.0f,1.0f });

    // playResult初期化
    playResult_ = Scene_Clear::GetResult();
    displayResult_.score = 0.0f;

    // displayResult初期化
    rankToString_[ScoreRank::ULT] = "ULT.prefab";
    rankToString_[ScoreRank::SSS] = "SSS.prefab";
    rankToString_[ScoreRank::SS] = "SS.prefab";
    rankToString_[ScoreRank::S] = "S.prefab";
    rankToString_[ScoreRank::A] = "A.prefab";
    rankToString_[ScoreRank::B] = "B.prefab";
    rankToString_[ScoreRank::C] = "C.prefab";
    rankToString_[ScoreRank::D] = "D.prefab";


    // リザルトシーンの読み込み
    auto* scene = GameSystem::GetScene();
    auto* hierarchy = scene->GetHierarchy();
    hierarchy->LoadScene("ResultScene.scene", false);
    rankObj_ = hierarchy->LoadObject("Result/Ranks/" + rankToString_[playResult_.rank]);
    cameraPointObj_ = hierarchy->LoadObject("Result/ResultCameraPoints.prefab");
    // 背景エフェクトの読み込み
    hierarchy->LoadObject("PlayScene/Effects/StageBack.prefab");

    // カメラ位置の設定
    if(cameraPointObj_){
        Transform cameraPoint = cameraPointObj_->GetComponent<Routine3DComponent>()->GetControlPoint(0);
       SEED::Instance::GetMainCamera()->SetTransform(cameraPoint);
    }

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
    if(auto* obj = GameSystem::GetScene()->GetHierarchy()->GetGameObject2D("AP")){
        uiObjects_["AP"] = obj;
    }
    if(auto* obj = GameSystem::GetScene()->GetHierarchy()->GetGameObject2D("FC")){
        uiObjects_["FC"] = obj;
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
        uiObjects_["Title"]->GetComponent<Routine2DComponent>()->Play();
        uiObjects_["Title"]->GetComponent<UIComponent>()->GetText(0).text = playResult_.title;
        // 上から出てくるやつ
        uiObjects_["Top"]->GetComponent<Routine2DComponent>()->Play();
        // 下から出てくるやつ
        uiObjects_["Bottom"]->GetComponent<Routine2DComponent>()->Play();
        // jacket
        uiObjects_["Jacket"]->GetComponent<Routine2DComponent>()->Play();
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
            uiObjects_["Difficulty"]->GetComponent<Routine2DComponent>()->Play();
            int difficulty = playResult_.notesJson.value("difficulty",0);
            uiObjects_["Difficulty"]->GetComponent<UIComponent>()->GetText(0).text = std::to_string(difficulty);
            // 各判定
            uiObjects_["Perfect"]->GetComponent<Routine2DComponent>()->Play();
            uiObjects_["Perfect"]->GetComponent<UIComponent>()->GetText(1).text = std::to_string(playResult_.evaluationCount[0]);
            uiObjects_["Great"]->GetComponent<Routine2DComponent>()->Play();
            uiObjects_["Great"]->GetComponent<UIComponent>()->GetText(1).text = std::to_string(playResult_.evaluationCount[1]);
            uiObjects_["Good"]->GetComponent<Routine2DComponent>()->Play();
            uiObjects_["Good"]->GetComponent<UIComponent>()->GetText(1).text = std::to_string(playResult_.evaluationCount[2]);
            uiObjects_["Miss"]->GetComponent<Routine2DComponent>()->Play();
            uiObjects_["Miss"]->GetComponent<UIComponent>()->GetText(1).text = std::to_string(playResult_.evaluationCount[3]);
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

        case 2:
            // APアイコン
            if(playResult_.isAllPerfect){
                uiObjects_["AP"]->GetComponent<Routine2DComponent>()->Play();

            } else{
                // FCアイコン
                if(playResult_.isFullCombo){
                    uiObjects_["FC"]->GetComponent<Routine2DComponent>()->Play();
                }
            }

            break;

        default:
            break;
        }
    }

    // タイマーの段階に応じた処理
    int32_t currentPhase = timer_.GetCurrentIndex();
    if(currentPhase >= 1){
        // コンボ, Fast, Lateを徐々に表示
        uiObjects_["Combo"]->GetComponent<UIComponent>()->SetMasterColor({ 1.0f,1.0f,1.0f,timer_.GetProgress(1) });
        uiObjects_["Fast"]->GetComponent<UIComponent>()->SetMasterColor({ 1.0f,1.0f,1.0f,timer_.GetProgress(1) });
        uiObjects_["Late"]->GetComponent<UIComponent>()->SetMasterColor({ 1.0f,1.0f,1.0f,timer_.GetProgress(1) });

    }

    if(currentPhase >= 2){
        // 表示用のスコアをカウントアップ
        displayResult_.score = MyMath::Lerp(displayResult_.score, static_cast<float>(playResult_.score), timer_.GetProgress(2));
    }

    // rankObjを回転させる
    static float rotateSpeed = 3.14f * 0.5f;
    if(rankObj_){
        rankObj_->AddWorldRotate(Vector3(0.0f, 1.0f, 0.0f) * rotateSpeed * ClockManager::DeltaTime());
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