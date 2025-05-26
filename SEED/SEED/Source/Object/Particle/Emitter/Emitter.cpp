#include <SEED/Source/Object/Particle/Emitter/Emitter.h>
#include <SEED/Source/Manager/ClockManager/ClockManager.h>
#include <SEED/Source/Manager/TextureManager/TextureManager.h>

Emitter_Base::Emitter_Base(){
    colors.push_back(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    texturePaths.push_back(std::string("ParticleTextures/particle.png"));
    totalTime = interval;
}

void Emitter_Base::Update(){

    if(!isActive){ return; }

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


// 編集関数
void EmitterGroup::Edit(){

    // エミッターごとに編集
    int32_t emitterNo = 0;
    for(auto itEmitter = emitters.begin(); itEmitter != emitters.end();){

        std::string eName = "Emitter_" + std::to_string(emitterNo);
        Emitter_Base* emitter = *itEmitter;

        if(ImGui::CollapsingHeader(eName.c_str())){
            ImGui::Indent();

            // エミッターの編集
            emitter->Edit();

            // 削除ボタン
            if(ImGui::Button("DeleteEmitter")){
                ImGui::OpenPopup("Delete?");
            }

            // 削除ボタンを押したらポップアップを出し確認をする
            if(ImGui::BeginPopupModal("Delete?")){
                ImGui::Text("Are you sure you want to delete?");

                // OKボタンを押したら削除
                if(ImGui::Button("OK", ImVec2(120, 0))){
                    itEmitter = emitters.erase(itEmitter); // 要素を削除し、次の要素を取得
                    ImGui::CloseCurrentPopup();
                    ImGui::EndPopup();
                    continue; // 削除したので次の要素に進む
                }

                // キャンセルボタンを押したらポップアップを閉じる
                if(ImGui::Button("Cancel", ImVec2(120, 0))){
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }

            ImGui::Unindent();
        }

        ++itEmitter;// 次の要素に進む
        ++emitterNo;
    }

    ImGui::Text("-------------");

    // エミッターの追加ボタン
    if(ImGui::Button("AddEmitter")){
        // ここでエミッターの種類によって追加するエミッターを変える (あとで)
        //emitterGroup->emitters.emplace_back(Emitter());

        emitters.back()->parentGroup = this;
        emitters.back()->isEdittting = true;
    }

    // エミッターグループをjsonに保存
    if(ImGui::Button("OutputToJson")){
        // 入力前に文字列をクリア
        for(auto& character : outputFileName_){ character = '\0'; }
        ImGui::OpenPopup("OutputToJson");
    }

    OutputGUI();
}


void EmitterGroup::OutputGUI(){

    if(ImGui::BeginPopupModal("OutputToJson")){
        ImGui::Text("Please fileName in below");
        ImGui::InputText(".json", outputFileName_, IM_ARRAYSIZE(outputFileName_));

        if(ImGui::Button("OK")){
            // ファイルに保存
            Output();
            // 成功した旨を表示
            MessageBoxA(nullptr, "Json is Saved", "SaveToJson", MB_OK);
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
