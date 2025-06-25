#include <SEED/Source/Basic/Particle/Emitter/Emitter.h>
#include <SEED/Source/Manager/ClockManager/ClockManager.h>
#include <SEED/Source/Manager/TextureManager/TextureManager.h>

// Emitter
#include "Emitter_Model.h"


Emitter_Base::Emitter_Base(){
    colors.push_back(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    totalTime = interval;
    // タグの生成
    idTag_ = "##" + std::to_string(nextEmitterID_); // ユニークなIDタグを生成
    nextEmitterID_++;
}

////////////////////////////////////////////////////////
// 更新関数
////////////////////////////////////////////////////////
void Emitter_Base::Update(){

    if(!isAlive){
        return;
    }

    totalTime += ClockManager::DeltaTime();

    if(totalTime >= interval){
        totalTime = 0.0f;
        emitCount++;

        // アクティブな時だけ発生依頼を出す
        if(isActive){
            emitOrder = true;
        }

        // 無限発生の場合はこのまま処理を続ける
        if(emitType == EmitType::kInfinite){
            return;
        }

        // カスタム発生 or 1回発生の場合は最大発生回数に達したら非アクティブにする
        if(emitCount >= kMaxEmitCount or emitType == EmitType::kOnce){

            isAlive = false;
            if(isEdittting){
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

    //
    ImTextureID visibleIcons[] = {
        TextureManager::GetImGuiTexture("../../SEED/EngineResources/Textures/visible.png"),
        TextureManager::GetImGuiTexture("../../SEED/EngineResources/Textures/invisible.png")
    };

    // エミッターごとに編集
    static std::string label = "";
    int32_t emitterNo = 0;

    for(auto itEmitter = emitters.begin(); itEmitter != emitters.end();){

        Emitter_Base* emitter = itEmitter->get();
        label = "visible_" + std::to_string(emitterNo) + idTag_;

        // アクティブかどうかのチェックボックス
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        if(emitter->isActive){
            if(ImGui::ImageButton(label.c_str(), visibleIcons[0], ImVec2(30, 20), ImVec2(0, 0), ImVec2(1, 1))){
                emitter->isActive = false; // 非アクティブにする
            }
        } else{
            if(ImGui::ImageButton(label.c_str(), visibleIcons[1], ImVec2(30, 20), ImVec2(0, 0), ImVec2(1, 1))){
                emitter->isActive = true; // アクティブにする
            }
        }
        ImGui::PopStyleColor();

        // エミッターボタン
        ImGui::SameLine();
        label = "Emitter_" + std::to_string(emitterNo) + idTag_;
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


    // コピー
    if(selectedEmitter_){
        // ctrl + cでコピー情報取得
        if(Input::IsPressKey(DIK_LCONTROL) && Input::IsPressKey(DIK_C)){
            copyDstEmitter_ = selectedEmitter_;
        }
    }

    // ペースト
    if(copyDstEmitter_){
        // ctrl + vのトリガー取得
        bool isTrigger = false;
        if(Input::IsPressKey(DIK_LCONTROL)){
            if(Input::IsTriggerKey(DIK_V)){
                isTrigger = true;
            }
        } else if(Input::IsPressKey(DIK_RCONTROL)){
            if(Input::IsTriggerKey(DIK_V)){
                isTrigger = true;
            }
        }

        // ペースト処理
        if(isTrigger){
            // コピー元のエミッターの情報を元に新しいエミッターを作成
            if(Emitter_Model* modelEmitter = dynamic_cast<Emitter_Model*>(copyDstEmitter_)){
                // コピー元のエミッターの情報を元に新しいエミッターを作成
                emitters.emplace_back(new Emitter_Model(*modelEmitter));
            }

            // 親の設定
            emitters.back()->parentGroup = this;
            emitters.back()->isEdittting = true; // 編集モードにする
        }
    }
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
            emitter->isEdittting = true; // 編集モードにする
            emitter->isAlive = true; // 生存状態にする
            emitter->emitCount = 0; // 発生回数をリセット
            emitter->totalTime = 0.0f;
        }
    }
}
