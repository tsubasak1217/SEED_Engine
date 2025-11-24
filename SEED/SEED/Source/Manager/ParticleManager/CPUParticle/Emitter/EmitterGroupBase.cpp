#include "EmitterGroupBase.h"
#include <SEED/Source/Manager/ParticleManager/CPUParticle/Emitter/EmitterBase.h>
#include <SEED/Source/Manager/ClockManager/ClockManager.h>
#include <SEED/Source/Manager/TextureManager/TextureManager.h>
#include <SEED/Source/Manager/ParticleManager/CPUParticle/Emitter/EmitterGroup3D.h>
#include <SEED/Source/Manager/ParticleManager/CPUParticle/Emitter/EmitterGroup2D.h>

// Emitter
#include "Emitter3D.h"
#include "Emitter2D.h"

///////////////////////////////////////////////////////////////////////////////////////
// EmitterGroupのコンストラクタ
///////////////////////////////////////////////////////////////////////////////////////
EmitterGroupBase::EmitterGroupBase(){
    idTag_ = "##" + std::to_string(nextGroupID_); // ユニークなIDタグを生成
    nextGroupID_++;
}

///////////////////////////////////////////////////////////////////////////////////////
// アクセッサ
///////////////////////////////////////////////////////////////////////////////////////
bool EmitterGroupBase::GetIsAlive() const{
    // エミッターが1つもない場合はtrueを返す
    if(emitters.size() == 0){
        return true;
    }

    // 1つでも生存しているエミッターがあればtrueを返す
    for(const auto& emitter : emitters){
        if(emitter->isAlive_){
            return true;
        }
    }

    return false;
}


///////////////////////////////////////////////////////////////////////////////////////
// 子に自分を親として設定してあげる
///////////////////////////////////////////////////////////////////////////////////////
void EmitterGroupBase::TeachParent(){
    for(auto& emitter : emitters){
        emitter->parentGroup_ = this;
    }
}

///////////////////////////////////////////////////////////////////////////////////////
// 編集関数
///////////////////////////////////////////////////////////////////////////////////////
void EmitterGroupBase::Edit(){
#ifdef _DEBUG

    // 初期化変数
    static bool isInitialized = false;
    if(!isInitialized){
        icons_["visible"] = TextureManager::GetImGuiTexture("[Engine]visible.png");
        icons_["invisible"] = TextureManager::GetImGuiTexture("[Engine]invisible.png");
    }

    // グループのパラメーター編集
    ImGui::Text("--初期化時にあらかじめ経過させておく時間--");
    ImGui::DragFloat("秒" + idTag_, &initUpdateTime_, 0.05f, 0.0f, 100.0f);

    // エミッターごとに編集
    ImGui::Text("エミッター一覧(要素数:%d)", (int)emitters.size());
    std::string tag = "";
    for(int32_t i = 0; i < emitters.size(); i++){

        // 一意なタグの生成
        tag = "##" + MyFunc::PtrToStr(emitters[i].get());

        // アクティブかどうかのチェックボックス
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImFunc::ActivateImageButton("visible" + tag, emitters[i]->isActive_, icons_["visible"], icons_["invisible"], ImVec2(30, 20));
        ImGui::PopStyleColor();

        // エミッターのヘッダー
        ImGui::SameLine();
        bool isOpen = ImFunc::CollapsingHeader("Emitter_" + std::to_string(i), EditorColor::emitterHeader);
        bool isRightClicked = ImGui::IsItemClicked(ImGuiMouseButton_Right);
        if(isOpen){
            emitters[i]->Edit();
        }

        // 右クリックメニューを開く
        if(isRightClicked){
            ImGui::OpenPopup("エミッターメニュー" + idTag_);
            selectEmitterIdx_ = i;
        }
    }

    ImGui::Separator();
    ImGui::Text("要素の追加");

    // エミッターの追加ボタン
    if(ImGui::Button("エミッターの追加" + tag)){
        if(EmitterGroup3D* group3D = dynamic_cast<EmitterGroup3D*>(this)){
            emitters.emplace_back(new Emitter3D());
        } else{
            emitters.emplace_back(new Emitter2D());
        }
        emitters.back()->parentGroup_ = this;
    }

    // エミッターグループをjsonに保存
    ImGui::SeparatorText("出力");
    ImGui::Spacing();
    if(ImGui::Button("エフェクトをファイルに出力" + tag)){
        OutputGUI();
    }

    // コンテキストメニューの表示
    if(ImGui::BeginPopup("エミッターメニュー" + idTag_)){
        ContextMenu();
        ImGui::EndPopup();
    }

    // 再活性化処理(編集中のみ)
    Reactivation();

#endif // _DEBUG
}


///////////////////////////////////////////////////////////////////////////////////////
// 出力GUIの表示関数
///////////////////////////////////////////////////////////////////////////////////////
void EmitterGroupBase::OutputGUI(){
#ifdef _DEBUG

    std::filesystem::path outputDir = "";
    
    // 型に応じて保存先を変更
    if(EmitterGroup3D* group3D = dynamic_cast<EmitterGroup3D*>(this)){
        outputDir = MyFunc::OpenSaveFileDialog("Resources/Jsons/Particle/3D", ".emtg3", outputFileName_);
    } else{
        outputDir = MyFunc::OpenSaveFileDialog("Resources/Jsons/Particle/2D", ".emtg2", outputFileName_);
    }

    if(!outputDir.empty()){
        MyFunc::CreateJsonFile(outputDir, GetJson());
    }
#endif // _DEBUG
}


///////////////////////////////////////////////////////////////////////////////////////
// コンテキストメニュー
///////////////////////////////////////////////////////////////////////////////////////
void EmitterGroupBase::ContextMenu(){
#ifdef _DEBUG

    // エミッターの削除
    if(ImGui::MenuItem("削除")){
        if(selectEmitterIdx_ >= 0 && selectEmitterIdx_ < emitters.size()){
            emitters.erase(emitters.begin() + selectEmitterIdx_);
            selectEmitterIdx_ = -1;
        }
        ImGui::CloseCurrentPopup();
    }

    // 複製
    if(ImGui::MenuItem("複製")){
        if(selectEmitterIdx_ >= 0 && selectEmitterIdx_ < emitters.size()){
            // コピーコンストラクタで複製
            EmitterBase* selectedEmitter = emitters[selectEmitterIdx_].get();
            EmitterBase* newEmitter = nullptr;

            // 型を確認してインスタンス作成
            if(Emitter3D* asEmitter3D = dynamic_cast<Emitter3D*>(selectedEmitter)){
                newEmitter = new Emitter3D();
                // 型を変換してからコピー
                *dynamic_cast<Emitter3D*>(newEmitter) = *asEmitter3D;

            } else if(Emitter2D* asEmitter2D = dynamic_cast<Emitter2D*>(selectedEmitter)){
                newEmitter = new Emitter2D();
                // 型を変換してからコピー
                *dynamic_cast<Emitter2D*>(newEmitter) = *asEmitter2D;
            }

            // 追加
            newEmitter->parentGroup_ = this;
            newEmitter->CreateTag();
            emitters.emplace_back(std::unique_ptr<EmitterBase>(newEmitter));
        }
        ImGui::CloseCurrentPopup();
    }
#endif // _DEBUG
}


///////////////////////////////////////////////////////////////////////////////////////
// エミッターの再活性化処理
///////////////////////////////////////////////////////////////////////////////////////
void EmitterGroupBase::Reactivation(){
    // 復活処理など
    bool isAllStopped = true;

    // エミッターが1つもない場合はfalse
    if(emitters.size() == 0){
        isAllStopped = false;
    }

    // 1つでも生存しているエミッターがあればfalse
    for(const auto& emitter : emitters){
        if(emitter->isAlive_){
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
            emitter->isAlive_ = true; // 生存状態にする
            emitter->emitCount_ = 0; // 発生回数をリセット
            emitter->totalTime_ = 0.0f;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////
// エミッターを初期状態に戻す
///////////////////////////////////////////////////////////////////////////////////////
void EmitterGroupBase::InitEmitters(){
    for(auto& emitter : emitters){
        emitter->isAlive_ = true; // 生存状態にする
        emitter->emitCount_ = 0; // 発生回数をリセット
        emitter->totalTime_ = 0.0f;
        emitter->emitOrder_ = false;
    }
}


///////////////////////////////////////////////////////////////////////////////////////
// Json入出力
///////////////////////////////////////////////////////////////////////////////////////
void EmitterGroupBase::LoadFromJson(const nlohmann::json& j){

    // グループのパラメーターを読み込み
    initUpdateTime_ = j.value("initUpdateTime", 0.0f);

    // エミッターの情報を読み込み
    for(auto& emitterJson : j["emitters"]){
        std::unique_ptr<EmitterBase> emitter;

        // エミッターの種類によってインスタンスを生成
        if(emitterJson["emitterType"] == "Emitter_Model3D" or emitterJson["emitterType"] == "Emitter3D"){
            emitter = std::make_unique<Emitter3D>();

        } else if(emitterJson["emitterType"] == "Emitter2D"){
            emitter = std::make_unique<Emitter2D>();

        }

        emitter->LoadFromJson(emitterJson);
        emitter->parentGroup_ = this;
        emitter->initUpdateTime_ = initUpdateTime_;
        emitters.push_back(std::move(emitter));
    }
}


nlohmann::json EmitterGroupBase::GetJson(){

    // エミッターの情報をjsonに保存
    nlohmann::json j;
    for(auto& emitter : emitters){
        j["emitters"].push_back(emitter->ExportToJson());
    }

    // グループのパラメーターを保存
    j["initUpdateTime"] = initUpdateTime_;

    return j;
}