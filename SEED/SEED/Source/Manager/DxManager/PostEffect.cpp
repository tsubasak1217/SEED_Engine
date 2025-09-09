// local
#include <SEED/Source/Manager/DxManager/DxManager.h>
#include <SEED/Source/Manager/DxManager/PostEffect.h>
#include <SEED/Source/SEED.h>
#include <SEED/Lib/Functions/MyFunc/MyMath.h>
#include <SEED/Lib/Structs/CS_Buffers.h>
#include <SEED/Source/Manager/DxManager/PSO/PSOManager.h>
#include <SEED/Source/Basic/PostProcess/IPostProcess.h>

// external
#include <cmath>

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

////////////////////////////////////////////////////////////////////////////////////////
//                          static変数初期化
////////////////////////////////////////////////////////////////////////////////////////

PostEffect* PostEffect::instance_ = nullptr;


//////////////////////////////////////////////////////////////////////////////////////////
//                          コンストラクタ・デストラクタ
//////////////////////////////////////////////////////////////////////////////////////////

PostEffect::~PostEffect(){}

////////////////////////////////////////////////////////////////////////////////////////
//                          初期化・終了関数
////////////////////////////////////////////////////////////////////////////////////////

void PostEffect::Initialize(){
    CreateResources();
}


//////////////////////////////////////////////////////////////////////////////////////////
// 更新処理
//////////////////////////////////////////////////////////////////////////////////////////
void PostEffect::Update(){

    // 削除フラグのあるグループの削除を行う
    std::erase_if(instance_->postProcessGroups_,
        [](auto& kv){ return kv.second.removeOrder; }
    );

    // 削除フラグがあるメンバーの削除を行う
    for(auto& postProcessGroup : instance_->postProcessGroups_){
        auto& members = postProcessGroup.second.postProcesses;

        // 削除フラグが立っているものを削除
        auto deleteItr = std::remove_if(
            members.begin(), members.end(),
            [](const std::pair<std::unique_ptr<IPostProcess>, bool>& postProcess){return postProcess.second; }
        );

        // 実際に削除
        members.erase(deleteItr, members.end());
    }

    // ポストエフェクトの更新
    for(auto& postProcessGroup : instance_->postProcessGroups_){

        if(!postProcessGroup.second.isActive){
            continue;
        }

        // 各メンバーの更新
        auto& members = postProcessGroup.second.postProcesses;
        for(auto& member : members){
            if(member.first->GetIsActive()){
                member.first->Update();
            }
        }
    }

}

//////////////////////////////////////////////////////////////////////////////////////////////
// ファイルからポストエフェクトに必要なパイプラインを読み込み作成
//////////////////////////////////////////////////////////////////////////////////////////////

void PostEffect::InitPSO(){
    PSOManager::CreatePipelines("DoFPipeline.pip");// 被写界深度パイプライン
    PSOManager::CreatePipelines("GrayScale.pip");// グレースケールパイプライン
    PSOManager::CreatePipelines("Vignette.pip");// ビネットパイプライン
    PSOManager::CreatePipelines("RGBShift.pip");// RGBシフトパイプライン
    PSOManager::CreatePipelines("ScanLine.pip");// スキャンラインパイプライン
}


/////////////////////////////////////////////////////////////////////////////////////////////
// リソースを作成する
/////////////////////////////////////////////////////////////////////////////////////////////
void PostEffect::CreateResources(){

    //================ ポストエフェクト用のResourceの初期化 =================//
    for(int i = 0; i < 2; i++){
        // Resourceの作成
        postEffectTextureResource[i].resource = InitializeTextureResource(
            DxManager::GetInstance()->GetDevice(),
            SEED::GetInstance()->kClientWidth_, SEED::GetInstance()->kClientHeight_,
            DXGI_FORMAT_R8G8B8A8_UNORM,
            STATE_UNORDERED_ACCESS
        );

        std::wstring name = L"postEffectResource_" + std::to_wstring(i);
        postEffectTextureResource[i].resource->SetName(name.c_str());
        postEffectTextureResource[i].InitState(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

        // SRVのDesc設定
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = postEffectTextureResource[i].resource->GetDesc().Format;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // 2Dテクスチャ
        srvDesc.Texture2D.MipLevels = 1;

        // UAVのDesc設定
        D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
        uavDesc.Format = postEffectTextureResource[i].resource->GetDesc().Format;
        uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;

        // Viewの生成
        ViewManager::CreateView(VIEW_TYPE::SRV, postEffectTextureResource[i].resource.Get(), &srvDesc, "postEffect_" + std::to_string(i));
        ViewManager::CreateView(VIEW_TYPE::UAV, postEffectTextureResource[i].resource.Get(), &uavDesc, "postEffect_" + std::to_string(i) + "_UAV");
    }

    // ポストエフェクトの結果を格納するResource
    {
        // Resourceの作成
        postEffectResultResource.resource = InitializeTextureResource(
            DxManager::GetInstance()->GetDevice(),
            SEED::GetInstance()->kClientWidth_, SEED::GetInstance()->kClientHeight_,
            DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
            STATE_SHADER_RESOURCE
        );

        std::wstring name = L"postEffectResult";
        postEffectResultResource.resource->SetName(name.c_str());
        postEffectResultResource.InitState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

        // SRVのDesc設定
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = postEffectResultResource.resource->GetDesc().Format;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // 2Dテクスチャ
        srvDesc.Texture2D.MipLevels = 1;

        // UAVのDesc設定
        D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
        uavDesc.Format = postEffectResultResource.resource->GetDesc().Format;
        uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;

        // Viewの生成
        ViewManager::CreateView(VIEW_TYPE::SRV, postEffectResultResource.resource.Get(), &srvDesc, "postEffectResult");
    }

}


////////////////////////////////////////////////////////////////////////////////////////
// リソースの解放
////////////////////////////////////////////////////////////////////////////////////////

void PostEffect::Release(){

    // リソースの解放
    for(auto& postProcessGroup : instance_->postProcessGroups_){
        auto& members = postProcessGroup.second.postProcesses;
        for(auto& member : members){
            if(member.first->GetIsActive()){
                member.first->Release();
            }
        }
    }

    for(int i = 0; i < 2; i++){
        if(postEffectTextureResource[i].resource){
            postEffectTextureResource[i].resource->Release();
        }
    }

    if(postEffectResultResource.resource){
        postEffectResultResource.resource->Release();
    }
}


//////////////////////////////////////////////////////////////////////////////////////
// オフスクリーンの内容をポストエフェクト用のテクスチャにコピー
//////////////////////////////////////////////////////////////////////////////////////
void PostEffect::CopyOffScreen(){

    // コピー状態を設定
    auto& src = DxManager::instance_->offScreenResources[SEED::GetMainCameraName()];
    auto& dst = postEffectTextureResource[currentBufferIndex_];
    src.TransitionState(D3D12_RESOURCE_STATE_COPY_SOURCE);
    dst.TransitionState(D3D12_RESOURCE_STATE_COPY_DEST);

    // コピーを行う
    auto& commandList = DxManager::GetInstance()->commandList;
    commandList->CopyResource(
        postEffectTextureResource[currentBufferIndex_].resource.Get(),
        DxManager::instance_->offScreenResources[SEED::GetMainCameraName()].resource.Get()
    );

    // コピー後の状態を設定
    src.TransitionState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    dst.TransitionState(D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
}


////////////////////////////////////////////////////////////////////////////////////////
// インスタンスの取得
////////////////////////////////////////////////////////////////////////////////////////

PostEffect* PostEffect::GetInstance(){
    if(instance_ == nullptr){
        instance_ = new PostEffect();
    }
    return instance_;
}


////////////////////////////////////////////////////////////////////////////////////////
// 有効なポストエフェクトを適用する処理
////////////////////////////////////////////////////////////////////////////////////////
void PostEffect::PostProcess(){

    // ポストエフェクトの開始
    CopyOffScreen();
    StartTransition();

    // 有効なポストエフェクトを適用
    for(auto& postProcessGroup : instance_->postProcessGroups_){

        if(!postProcessGroup.second.isActive){
            continue;
        }

        // 各メンバーの更新
        auto& members = postProcessGroup.second.postProcesses;
        for(auto& member : members){
            if(member.first->GetIsActive()){
                member.first->Apply();
            }
        }
    }

    // ポストエフェクトの終了
    EndTransition();
}


////////////////////////////////////////////////////////////////////////////////////////////
// ポストエフェクト開始時の遷移
////////////////////////////////////////////////////////////////////////////////////////////

void PostEffect::StartTransition(){

    // ポストエフェクトの結果格納用Transition
    postEffectTextureResource[currentBufferIndex_].TransitionState(D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
    postEffectTextureResource[(currentBufferIndex_ + 1) % 2].TransitionState(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
}

////////////////////////////////////////////////////////////////////////////////////////
// ポストエフェクト終了時の遷移
////////////////////////////////////////////////////////////////////////////////////////

void PostEffect::EndTransition(){
    // ぼかしテクスチャのTransition
    postEffectTextureResource[currentBufferIndex_].TransitionState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    // 結果格納用のバッファに結果をコピーする(SRGBで保持用)
    {
        // コピー状態を設定
        auto& src = postEffectTextureResource[currentBufferIndex_];
        auto& dst = postEffectResultResource;
        src.TransitionState(D3D12_RESOURCE_STATE_COPY_SOURCE);
        dst.TransitionState(D3D12_RESOURCE_STATE_COPY_DEST);

        // コピーを行う
        auto& commandList = DxManager::GetInstance()->commandList;
        commandList->CopyResource(
            postEffectResultResource.resource.Get(),
            postEffectTextureResource[currentBufferIndex_].resource.Get()
        );

        // コピー後の状態を設定
        src.TransitionState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        dst.TransitionState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    }
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  編集関数
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////

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



void PostEffect::Edit(){
#ifdef _DEBUG

    ImFunc::CustomBegin("ポストエフェクト", MoveOnly_TitleBar);
    {
        // 新しいポストプロセスグループの追加
        if(ImGui::Button("新規ポストプロセスグループの追加")){
            postProcessGroups_[nextHandle_] = PostProcessGroup();
            postProcessGroups_[nextHandle_].handle_ = nextHandle_;
            nextHandle_++;
        }

        // ポストプロセスグループの編集
        for(auto& postProcessGroup : instance_->postProcessGroups_){
            std::string label = "ポストプロセスグループ " + std::to_string(postProcessGroup.first) + "##" + std::to_string(postProcessGroup.first);
            if(ImGui::CollapsingHeader(label.c_str())){
                ImGui::Indent();
                postProcessGroup.second.Edit();

                label = "削除##" + std::to_string(postProcessGroup.first);
                if(ImGui::Button(label.c_str())){
                    postProcessGroup.second.removeOrder = true;
                }

                ImGui::Unindent();
            }
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // ポストプロセスの読み込み
        {
            static std::filesystem::path currentPath = "Resources/Jsons/PostProcess/";
            static std::filesystem::path rootPath = "Resources/Jsons/PostProcess/";
            std::string selectedFile = ImFunc::FolderView("ポストプロセスの読み込み", currentPath, false, { ".json" }, rootPath);

            // ファイルが選択された場合、読み込む
            if(!selectedFile.empty()){
                Load(selectedFile);
            }
        }

    }ImGui::End();
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

//////////////////////////////////////////////////////////////////////////////////////////
// ファイルからポストエフェクトを読み込む関数
//////////////////////////////////////////////////////////////////////////////////////////
PostEffectHandle PostEffect::Load(const std::string& fileName){
    static std::filesystem::path rootPath = "Resources/Jsons/PostProcess/";
    std::filesystem::path filePath;

    // フルパスかどうかで場合分け
    if(fileName.starts_with("Resources")){
        filePath = fileName;
    } else{
        filePath = rootPath / fileName;
    }

    // 読み込む
    if(std::filesystem::exists(filePath)){
        std::ifstream file(filePath);
        if(file.is_open()){
            nlohmann::json json;
            file >> json;
            PostProcessGroup newGroup;
            newGroup.handle_ = instance_->nextHandle_;
            newGroup.FromJson(json);
            instance_->postProcessGroups_[instance_->nextHandle_] = std::move(newGroup);
            file.close();
            return instance_->nextHandle_++;
        } else{
            assert(false); // ファイル読み込み失敗
        }
    } else{
        assert(false); // ファイルが存在しない
    }

    return -1;
}


//////////////////////////////////////////////////////////////////////////////////////////
// 削除
//////////////////////////////////////////////////////////////////////////////////////////
void PostEffect::DeleteAll(){
    instance_->postProcessGroups_.clear();
}

void PostEffect::Delete(PostEffectHandle handle){
    auto it = instance_->postProcessGroups_.find(handle);
    if(it != instance_->postProcessGroups_.end()){
        instance_->postProcessGroups_.erase(it);
    }
}

void PostEffect::SetActive(PostEffectHandle handle, bool isActive){
    auto it = instance_->postProcessGroups_.find(handle);
    if(it != instance_->postProcessGroups_.end()){
        it->second.isActive = isActive;
    }
}
