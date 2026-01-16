#include "PostEffectContext.h"
#include <SEED/Source/Manager/ImGuiManager/ImGuiManager.h>
#include <SEED/Lib/Functions/FileFunc.h>

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


namespace SEED{
    void PostProcessGroup::Edit(){
    #ifdef _DEBUG

        std::string hash = "##" + std::to_string(handle_);

        // ポストプロセスグループのアクティブ切り替え =======================================
        ImGui::Checkbox("アクティブ" + hash, &isActive);

        // 新しいポストエフェクトの追加 ===================================================
        if(ImGui::CollapsingHeader("新しいポストエフェクトの追加" + hash)){
            ImGui::Indent();
            {
                if(ImGui::Button("グレースケール" + hash)){
                    postProcesses.push_back({ std::make_unique<GrayScale>(),false });
                    postProcesses.back().first->Initialize();
                }
                if(ImGui::Button("ガウスぼかし" + hash)){
                    postProcesses.push_back({ std::make_unique<GaussianFilter>(),false });
                    postProcesses.back().first->Initialize();
                }
                if(ImGui::Button("被写界深度" + hash)){
                    postProcesses.push_back({ std::make_unique<DoF>() ,false });
                    postProcesses.back().first->Initialize();
                }
                if(ImGui::Button("ビネット" + hash)){
                    postProcesses.push_back({ std::make_unique<Vignette>(),false });
                    postProcesses.back().first->Initialize();
                }
                if(ImGui::Button("RGBシフト" + hash)){
                    postProcesses.push_back({ std::make_unique<RGBShift>(),false });
                    postProcesses.back().first->Initialize();
                }
                if(ImGui::Button("スキャンライン" + hash)){
                    postProcesses.push_back({ std::make_unique<ScanLine>(),false });
                    postProcesses.back().first->Initialize();
                }
                if(ImGui::Button("フォグ" + hash)){
                    postProcesses.push_back({ std::make_unique<Fog>(),false });
                    postProcesses.back().first->Initialize();
                }
                if(ImGui::Button("ブルーム" + hash)){
                    postProcesses.push_back({ std::make_unique<Bloom>(),false });
                    postProcesses.back().first->Initialize();
                }
                if(ImGui::Button("グリッチ" + hash)){
                    postProcesses.push_back({ std::make_unique<Glitch>(),false });
                    postProcesses.back().first->Initialize();
                }
            }ImGui::Unindent();
        }


        // パラメーターの編集 ===================================================
        ImGui::Separator();
        if(ImGui::CollapsingHeader("ポストプロセスの編集" + hash)){
            ImGui::Indent();

            for(auto& postProcess : postProcesses){

                // ポストプロセスの有効/無効を切り替えるチェックボックス
                std::string label = "##" + std::to_string(postProcess.first->GetId());
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
                if(ImGui::Button("削除##" + std::to_string(postProcess.first->GetId()))){
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
            if(ImGui::Button("ポストプロセスグループの保存" + hash)){
                // 保存ボタンが押されたら保存ダイアログを開く
                auto fillename = Methods::File::OpenSaveFileDialog("Resources/Jsons/PostProcess/", ".json");

                // ダイアログ上で保存ボタンが押されたら保存処理を行う
                if(!fillename.empty()){
                    Methods::File::CreateJsonFile(fillename, ToJson());
                }
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
}