#include "EmitterGroupBase.h"
#include <SEED/Source/Manager/ParticleManager/CPUParticle/Emitter/EmitterBase.h>
#include <SEED/Source/Manager/ClockManager/ClockManager.h>
#include <SEED/Source/Manager/TextureManager/TextureManager.h>
#include <SEED/Source/Manager/ParticleManager/CPUParticle/Emitter/EmitterGroup3D.h>

// Emitter
#include "Emitter3D.h"

// EmitterGroupのコンストラクタ
EmitterGroupBase::EmitterGroupBase(){
    idTag_ = "##" + std::to_string(nextGroupID_); // ユニークなIDタグを生成
    nextGroupID_++;
}

bool EmitterGroupBase::GetIsAlive() const{
    // エミッターが1つもない場合はtrueを返す
    if(emitters.size() == 0){
        return true;
    }

    if(isEditMode_){
        // 編集モードの場合は常にtrueを返す
        return true;
    }

    // 1つでも生存しているエミッターがあればtrueを返す
    for(const auto& emitter : emitters){
        if(emitter->isAlive){
            return true;
        }
    }

    return false;
}


void EmitterGroupBase::TeachParent(){
    for(auto& emitter : emitters){
        emitter->parentGroup = this;
    }
}

// 編集関数
void EmitterGroupBase::Edit(){

    // 初期化変数
    static bool isInitialized = false;
    if(!isInitialized){
        icons_["visible"] = TextureManager::GetImGuiTexture("[Engine]visible.png");
        icons_["invisible"] = TextureManager::GetImGuiTexture("[Engine]invisible.png");
    }

    // エミッターごとに編集
    ImGui::Text("エミッター一覧(要素数:%d)", (int)emitters.size());
    std::string tag = "";
    for(int32_t i = 0; i < emitters.size(); i++){

        // 一意なタグの生成
        tag = "##" + MyFunc::PtrToStr(emitters[i].get());

        // アクティブかどうかのチェックボックス
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImFunc::ActivateImageButton("visible" + tag, emitters[i]->isActive, icons_["visible"], icons_["invisible"], ImVec2(30, 20));
        ImGui::PopStyleColor();

        // エミッターのヘッダー
        ImGui::SameLine();
        if(ImGui::CollapsingHeader("Emitter_" + std::to_string(i))){
            emitters[i]->Edit();
        }
    }

    ImGui::Separator();
    ImGui::Text("要素の追加");

    // エミッターの追加ボタン
    if(ImGui::Button("エミッターの追加" + tag)){
        emitters.emplace_back(new Emitter3D());
        emitters.back()->parentGroup = this;
    }

    // エミッターグループをjsonに保存
    ImGui::Separator();
    ImGui::Text("出力");
    if(ImGui::Button("jsonに保存" + tag)){
        OutputGUI();
    }

    // 再活性化処理(編集中のみ)
    Reactivation();
}


void EmitterGroupBase::OutputGUI(){

    std::string outputDir = "";

    // 型に応じて保存先を変更
    if(EmitterGroup3D* group3D = dynamic_cast<EmitterGroup3D*>(this)){
        outputDir = MyFunc::OpenSaveFileDialog("Resources/Jsons/Particle/3D", ".emtg3", outputFileName_);
    } else{
        outputDir = MyFunc::OpenSaveFileDialog("Resources/Jsons/Particle/2D", ".emtg2", outputFileName_);
    }

    if(!outputDir.empty()){
        MyFunc::CreateJsonFile(outputDir, GetJson());
    }
}


nlohmann::json EmitterGroupBase::GetJson(){

    // エミッターの情報をjsonに保存
    nlohmann::json j;
    for(auto& emitter : emitters){
        j["emitters"].push_back(emitter->ExportToJson());
    }

    return j;
}

// エミッターの再活性化処理
void EmitterGroupBase::Reactivation(){
    // 復活処理など
    bool isAllStopped = true;

    // エミッターが1つもない場合はfalse
    if(emitters.size() == 0){
        isAllStopped = false;
    }

    // 1つでも生存しているエミッターがあればfalse
    for(const auto& emitter : emitters){
        if(emitter->isAlive){
            isAllStopped = false;
        }
    }

    // まだ誰か生きてるから処理しない
    if(!isAllStopped){ return; }

    // 全てのエミッターが停止している場合,復活処理を始める
    curReactiveTime_ += ClockManager::DeltaTime();

    // 再活性化までの時間が経過したら復活
    if(curReactiveTime_ >= kReactiveTime_){
        curReactiveTime_ = 0.0f;

        // 全てのエミッターを再活性化
        for(auto& emitter : emitters){
            emitter->isAlive = true; // 生存状態にする
            emitter->emitCount = 0; // 発生回数をリセット
            emitter->totalTime = 0.0f;
        }
    }
}


// Jsonから読み込み
void EmitterGroupBase::LoadFromJson(const nlohmann::json& j){

    // エミッターの情報を読み込み
    for(auto& emitterJson : j["emitters"]){
        std::unique_ptr<EmitterBase> emitter;

        // エミッターの種類によってインスタンスを生成
        if(emitterJson["emitterType"] == "Emitter_Model3D" or emitterJson["emitterType"] == "Emitter3D"){
            emitter = std::make_unique<Emitter3D>();

        } else if(emitterJson["emitterType"] == "Emitter2D"){
            // emitter = std::make_unique<Emitter2D>();
        } else{
            continue; // 未対応タイプはスキップ
        }

        emitter->LoadFromJson(emitterJson);
        emitters.push_back(std::move(emitter));
    }
}