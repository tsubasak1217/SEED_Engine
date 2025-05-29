#include <SEED/Source/Object/Particle/Emitter/Emitter.h>
#include <SEED/Source/Manager/ClockManager/ClockManager.h>
#include <SEED/Source/Manager/TextureManager/TextureManager.h>

// Emitter
#include "Emitter_Model.h"


Emitter_Base::Emitter_Base(){
    colors.push_back(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    //texturePaths.push_back(std::string("ParticleTextures/particle.png"));
    //textureSet.insert(texturePaths.back()); // テクスチャセットに追加
    totalTime = interval;
    // タグの生成
    idTag_ = "##" + std::to_string(nextEmitterID_); // ユニークなIDタグを生成
    nextEmitterID_++;
}

void Emitter_Base::Update(){

    if(!isActive){
        return;
    }

    totalTime += ClockManager::DeltaTime();

    if(totalTime >= interval){
        totalTime = 0.0f;
        emitOrder = true;
        emitCount++;

        // 無限発生の場合はこのまま処理を続ける
        if(emitType == EmitType::kInfinite){
            return;
        }

        // カスタム発生 or 1回発生の場合は最大発生回数に達したら非アクティブにする
        if(emitCount >= kMaxEmitCount or emitType == EmitType::kOnce){
            if(!isEdittting){
                isAlive = false;
            } else{
                isActive = false;
                emitCount = 0;
            }
            return;
        }
    }
}

// 中心座標を取得
Vector3 Emitter_Base::GetCenter(){
    if(parentGroup){
        return parentGroup->GetCenter() + center;
    } else{
        return center;
    }
}

// EmitterGroupのコンストラクタ
EmitterGroup::EmitterGroup(){
    idTag_ = "##" + std::to_string(nextGroupID_); // ユニークなIDタグを生成
    nextGroupID_++;
}

// 編集関数
void EmitterGroup::Edit(){

    ImGui::Text("エミッター一覧(要素数:%d)", (int)emitters.size());

    // エミッターごとに編集
    static std::string label = "";
    int32_t emitterNo = 0;

    for(auto itEmitter = emitters.begin(); itEmitter != emitters.end();){

        label = "Emitter_" + std::to_string(emitterNo) + idTag_;
        Emitter_Base* emitter = itEmitter->get();

        // 選択を更新
        if(ImGui::Button(label.c_str())){
            selectedEmitter_ = emitter; // 選択されたエミッターを保存
            selectedItEmitter_ = itEmitter; // 選択されたエミッターのイテレータを保存
            selectedEmitterName_ = "Emitter_" + std::to_string(emitterNo); // 選択されたエミッターの名前を保存
        }

        ++itEmitter;// 次の要素に進む
        ++emitterNo;
    }

    ImGui::Separator();
    ImGui::Text("要素の追加");

    // エミッターの追加ボタン
    label = "エミッターの追加" + idTag_;
    if(ImGui::Button(label.c_str())){
        // ここでエミッターの種類によって追加するエミッターを変える (あとで)
        emitters.emplace_back(new Emitter_Model());

        emitters.back()->parentGroup = this;
        emitters.back()->isEdittting = true;
    }

    // エミッターグループをjsonに保存
    ImGui::Separator();
    ImGui::Text("出力");
    label = "jsonに保存" + idTag_;
    if(ImGui::Button(label.c_str())){
        // 入力前に文字列をクリア
        for(auto& character : outputFileName_){ character = '\0'; }
        ImGui::OpenPopup("保存");
    }

    OutputGUI();
}


void EmitterGroup::OutputGUI(){

    if(ImGui::BeginPopupModal("保存")){
        ImGui::Text("ファイル名を入力");
        ImGui::InputText(".json", outputFileName_, IM_ARRAYSIZE(outputFileName_));

        if(ImGui::Button("決定")){
            // ファイルに保存
            Output();
            // 成功した旨を表示
            MessageBoxA(nullptr, "Json is Saved", "SaveToJson", MB_OK);
            // ポップアップを閉じる
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();
        if(ImGui::Button("キャンセル")){
            // ポップアップを閉じる
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}


void EmitterGroup::Output(){

    nlohmann::json j;

    // エミッターの情報をjsonに保存
    for(auto& emitter : emitters){
        j["emitters"].push_back(emitter->ExportToJson());
    }

    // ファイルに保存
    std::ofstream ofs("resources/jsons/particle/" + std::string(outputFileName_) + ".json");
    ofs << j.dump(4);
}

// エミッターの再活性化処理
void EmitterGroup::Reactivation(){
    // 復活処理など
    bool isAllStopped = true;

    // エミッターが1つもない場合はfalse
    if(emitters.size() == 0){
        isAllStopped = false;
    }

    // 1つでも生存しているエミッターがあればfalse
    for(const auto& emitter : emitters){
        if(emitter->isActive){
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
            emitter->isEdittting = true; // 編集モードにする
            emitter->isActive = true; // アクティブにする
            emitter->isAlive = true; // 生存状態にする
            emitter->emitCount = 0; // 発生回数をリセット
            emitter->totalTime = 0.0f;
        }
    }
}
