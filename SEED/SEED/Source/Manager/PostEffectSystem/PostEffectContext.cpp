#include "PostEffectContext.h"
#include <SEED/Source/Manager/ImGuiManager/ImGuiManager.h>

// postProcess
#include <SEED/Source/Basic/PostProcess/GaussianFilter/GaussianFilter.h>
#include <SEED/Source/Basic/PostProcess/DoF/DoF.h>
#include <SEED/Source/Basic/PostProcess/ScanLine/ScanLine.h>
#include <SEED/Source/Basic/PostProcess/RGBShift/RGBShift.h>
#include <SEED/Source/Basic/PostProcess/GrayScale/GrayScale.h>
#include <SEED/Source/Basic/PostProcess/Vignette/Vignette.h>
#include <SEED/Source/Basic/PostProcess/Fog/Fog.h>
#include <SEED/Source/Basic/PostProcess/Bloom/Bloom.h>
#include <SEED/Source/Basic/PostProcess/Glitch/Glitch.h>


void PostProcessGroup::Edit(){
#ifdef _DEBUG
    static std::string saveFileName = "postProcess";
    static std::string label;

    // ポストプロセスグループのアクティブ切り替え =======================================
    label = "アクティブ##" + std::to_string(handle_);
    ImGui::Checkbox(label.c_str(), &isActive);

    // 新しいポストエフェクトの追加 ===================================================
    label = "新しいポストエフェクトの追加##" + std::to_string(handle_);
    if(ImGui::CollapsingHeader(label.c_str())){
        ImGui::Indent();
        {
            if(ImGui::Button("グレースケール")){
                postProcesses.push_back({ std::make_unique<GrayScale>(),false });
                postProcesses.back().first->Initialize();
            }
            if(ImGui::Button("ガウスぼかし")){
                postProcesses.push_back({ std::make_unique<GaussianFilter>(),false });
                postProcesses.back().first->Initialize();
            }
            if(ImGui::Button("被写界深度")){
                postProcesses.push_back({ std::make_unique<DoF>() ,false });
                postProcesses.back().first->Initialize();
            }
            if(ImGui::Button("ビネット")){
                postProcesses.push_back({ std::make_unique<Vignette>(),false });
                postProcesses.back().first->Initialize();
            }
            if(ImGui::Button("RGBシフト")){
                postProcesses.push_back({ std::make_unique<RGBShift>(),false });
                postProcesses.back().first->Initialize();
            }
            if(ImGui::Button("スキャンライン")){
                postProcesses.push_back({ std::make_unique<ScanLine>(),false });
                postProcesses.back().first->Initialize();
            }
            if(ImGui::Button("フォグ")){
                postProcesses.push_back({ std::make_unique<Fog>(),false });
                postProcesses.back().first->Initialize();
            }
            if(ImGui::Button("ブルーム")){
                postProcesses.push_back({ std::make_unique<Bloom>(),false });
                postProcesses.back().first->Initialize();
            }
            if(ImGui::Button("グリッチ")){
                postProcesses.push_back({ std::make_unique<Glitch>(),false });
                postProcesses.back().first->Initialize();
            }
        }ImGui::Unindent();
    }


    // パラメーターの編集 ===================================================
    ImGui::Separator();
    label = "ポストプロセスの編集##" + std::to_string(handle_);
    if(ImGui::CollapsingHeader(label.c_str())){
        ImGui::Indent();

        for(auto& postProcess : postProcesses){

            // ポストプロセスの有効/無効を切り替えるチェックボックス
            label = "##" + std::to_string(postProcess.first->GetId());
            ImGui::Checkbox(label.c_str(), &postProcess.first->isActive_);

            // ↑,↓ ボタンを表示して順番を変更できるようにする
            ImGui::SameLine();
            label = "↑##" + std::to_string(postProcess.first->GetId());
            if(ImGui::Button(label.c_str())){
                // 前の要素と入れ替える
                auto it = std::find(postProcesses.begin(), postProcesses.end(), postProcess);
                if(it != postProcesses.begin()){
                    std::iter_swap(it, std::prev(it));
                }
            }
            ImGui::SameLine();
            label = "↓##" + std::to_string(postProcess.first->GetId());
            if(ImGui::Button(label.c_str())){
                // 次の要素と入れ替える
                auto it = std::find(postProcesses.begin(), postProcesses.end(), postProcess);
                if(it != postProcesses.end() && std::next(it) != postProcesses.end()){
                    std::iter_swap(it, std::next(it));
                }
            }

            // 削除ボタン
            ImGui::SameLine();
            label = "削除##" + std::to_string(postProcess.first->GetId());
            if(ImGui::Button(label.c_str())){
                postProcess.second = true; // 削除フラグを立てる
            }

            // パラメータの編集
            ImGui::SameLine();
            postProcess.first->Edit();

        }
        ImGui::Unindent();
    }

    // ポストプロセスの保存 ===================================================
    {
        static bool wantsToOverwrite = false;
        static std::string pendingPath = "";

        label = "ポストプロセスグループの保存##" + std::to_string(handle_);
        if(ImGui::Button(label.c_str())){
            ImGui::OpenPopup("ポストプロセスの保存");
        }

        if(ImGui::BeginPopupModal("ポストプロセスの保存", nullptr, ImGuiWindowFlags_AlwaysAutoResize)){

            if(!wantsToOverwrite){
                ImGui::Text("ファイル名を入力");
                ImFunc::InputText(".json", saveFileName);

                if(ImGui::Button("保存")){
                    // フルパスを構築
                    pendingPath = "Resources/Jsons/PostProcess/" + saveFileName + ".json";

                    if(std::filesystem::exists(pendingPath)){
                        // 上書き確認が必要
                        wantsToOverwrite = true;

                    } else{
                        std::ofstream file(pendingPath);
                        if(file.is_open()){
                            nlohmann::json json = ToJson();
                            file << json.dump(4);
                            file.close();
                            ImGui::CloseCurrentPopup();
                        } else{
                            assert(false); // 書き込み失敗
                        }
                    }
                }

                ImGui::SameLine();
                if(ImGui::Button("キャンセル")){
                    ImGui::CloseCurrentPopup();
                }


            } else{
                ImGui::Text("既に存在します。上書きしますか？");

                if(ImGui::Button("はい")){
                    std::ofstream file(pendingPath);
                    if(file.is_open()){
                        nlohmann::json json = ToJson();
                        file << json.dump(4);
                        file.close();
                    } else{
                        assert(false); // 書き込み失敗
                    }

                    wantsToOverwrite = false;
                    pendingPath.clear();
                    ImGui::CloseCurrentPopup();
                }

                ImGui::SameLine();
                if(ImGui::Button("いいえ")){
                    wantsToOverwrite = false;
                }
            }

            ImGui::EndPopup();
        }
    }
#endif // _DEBUG
}

////////////////////////////////////////////////////////////////////////////////////////////
// 入出力
////////////////////////////////////////////////////////////////////////////////////////////
nlohmann::json PostProcessGroup::ToJson() const{
    nlohmann::ordered_json json;
    json["postProcesses"] = nlohmann::json::array();
    for(const auto& postProcess : postProcesses){
        json["postProcesses"].push_back(postProcess.first->ToJson());
    }
    return json;
}

void PostProcessGroup::FromJson(const nlohmann::json& json){
    postProcesses.clear();

    for(const auto& postProcessJson : json["postProcesses"]){
        std::string type = postProcessJson["type"];
        if(type == "GrayScale"){
            auto postProcess = std::make_unique<GrayScale>();
            postProcess->Initialize();
            postProcess->FromJson(postProcessJson);
            postProcesses.emplace_back(std::make_pair(std::move(postProcess), false));

        } else if(type == "GaussianFilter"){
            auto postProcess = std::make_unique<GaussianFilter>();
            postProcess->Initialize();
            postProcess->FromJson(postProcessJson);
            postProcesses.emplace_back(std::make_pair(std::move(postProcess), false));

        } else if(type == "DoF"){
            auto postProcess = std::make_unique<DoF>();
            postProcess->Initialize();
            postProcess->FromJson(postProcessJson);
            postProcesses.emplace_back(std::make_pair(std::move(postProcess), false));

        } else if(type == "Vignette"){
            auto postProcess = std::make_unique<Vignette>();
            postProcess->Initialize();
            postProcess->FromJson(postProcessJson);
            postProcesses.emplace_back(std::make_pair(std::move(postProcess), false));

        } else if(type == "RGBShift"){
            auto postProcess = std::make_unique<RGBShift>();
            postProcess->Initialize();
            postProcess->FromJson(postProcessJson);
            postProcesses.emplace_back(std::make_pair(std::move(postProcess), false));

        } else if(type == "ScanLine"){
            auto postProcess = std::make_unique<ScanLine>();
            postProcess->Initialize();
            postProcess->FromJson(postProcessJson);
            postProcesses.emplace_back(std::make_pair(std::move(postProcess), false));

        } else if(type == "Fog"){
            auto postProcess = std::make_unique<Fog>();
            postProcess->Initialize();
            postProcess->FromJson(postProcessJson);
            postProcesses.emplace_back(std::make_pair(std::move(postProcess), false));

        } else if(type == "Bloom"){
            auto postProcess = std::make_unique<Bloom>();
            postProcess->Initialize();
            postProcess->FromJson(postProcessJson);
            postProcesses.emplace_back(std::make_pair(std::move(postProcess), false));

        }
    }
}