// local
#include <SEED/Source/Manager/DxManager/DxManager.h>
#include <SEED/Source/Manager/PostEffectSystem/PostEffectSystem.h>
#include <SEED/Source/SEED.h>
#include <SEED/Lib/Functions/MyMath.h>
#include <SEED/Lib/Structs/CS_Buffers.h>
#include <SEED/Source/Manager/DxManager/PSO/PSOManager.h>
#include <SEED/Source/Basic/PostProcess/IPostProcess.h>

// external
#include <cmath>


////////////////////////////////////////////////////////////////////////////////////////
//                          static変数初期化
////////////////////////////////////////////////////////////////////////////////////////

PostEffectSystem* PostEffectSystem::instance_ = nullptr;


//////////////////////////////////////////////////////////////////////////////////////////
//                          コンストラクタ・デストラクタ
//////////////////////////////////////////////////////////////////////////////////////////

PostEffectSystem::~PostEffectSystem(){}

////////////////////////////////////////////////////////////////////////////////////////
//                          初期化・終了関数
////////////////////////////////////////////////////////////////////////////////////////

void PostEffectSystem::Initialize(){
    CreateResources();
}


//////////////////////////////////////////////////////////////////////////////////////////
// 更新処理
//////////////////////////////////////////////////////////////////////////////////////////
void PostEffectSystem::Update(){

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

void PostEffectSystem::InitPSO(){
    PSOManager::CreatePipelines("DoFPipeline.pip");// 被写界深度パイプライン
    PSOManager::CreatePipelines("GrayScale.pip");// グレースケールパイプライン
    PSOManager::CreatePipelines("Vignette.pip");// ビネットパイプライン
    PSOManager::CreatePipelines("RGBShift.pip");// RGBシフトパイプライン
    PSOManager::CreatePipelines("ScanLine.pip");// スキャンラインパイプライン
}


/////////////////////////////////////////////////////////////////////////////////////////////
// リソースを作成する
/////////////////////////////////////////////////////////////////////////////////////////////
void PostEffectSystem::CreateResources(){

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

void PostEffectSystem::Release(){

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
            postEffectTextureResource[i].resource.Reset();
        }
    }

    if(postEffectResultResource.resource){
        postEffectResultResource.resource.Reset();
    }
}


//////////////////////////////////////////////////////////////////////////////////////
// オフスクリーンの内容をポストエフェクト用のテクスチャにコピー
//////////////////////////////////////////////////////////////////////////////////////
void PostEffectSystem::CopyOffScreen(){

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

PostEffectSystem* PostEffectSystem::GetInstance(){
    if(instance_ == nullptr){
        instance_ = new PostEffectSystem();
    }
    return instance_;
}


////////////////////////////////////////////////////////////////////////////////////////
// 有効なポストエフェクトを適用する処理
////////////////////////////////////////////////////////////////////////////////////////
void PostEffectSystem::PostProcess(){

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

void PostEffectSystem::StartTransition(){

    // ポストエフェクトの結果格納用Transition
    postEffectTextureResource[currentBufferIndex_].TransitionState(D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
    postEffectTextureResource[(currentBufferIndex_ + 1) % 2].TransitionState(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
}

////////////////////////////////////////////////////////////////////////////////////////
// ポストエフェクト終了時の遷移
////////////////////////////////////////////////////////////////////////////////////////

void PostEffectSystem::EndTransition(){
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

void PostEffectSystem::Edit(){
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
            std::string selectedFile = ImFunc::FolderView("ポストプロセスの読み込み", currentPath, { ".json" }, rootPath);

            // ファイルが選択された場合、読み込む
            if(!selectedFile.empty()){
                Load(selectedFile);
            }
        }

    }ImGui::End();
#endif // _DEBUG
}



//////////////////////////////////////////////////////////////////////////////////////////
// ファイルからポストエフェクトを読み込む関数
//////////////////////////////////////////////////////////////////////////////////////////
PostEffectHandle PostEffectSystem::Load(const std::string& fileName){
    static std::filesystem::path rootPath = "Resources/Jsons/PostProcess/";
    std::filesystem::path filePath;

    // フルパスかどうかで場合分け
    if(fileName.starts_with("Resources")){
        filePath = fileName;
    } else{
        filePath = MyFunc::ToFullPath(rootPath / fileName);
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
void PostEffectSystem::DeleteAll(){
    instance_->postProcessGroups_.clear();
}

void PostEffectSystem::Delete(PostEffectHandle handle){
    auto it = instance_->postProcessGroups_.find(handle);
    if(it != instance_->postProcessGroups_.end()){
        instance_->postProcessGroups_.erase(it);
    }
}

void PostEffectSystem::SetActive(PostEffectHandle handle, bool isActive){
    auto it = instance_->postProcessGroups_.find(handle);
    if(it != instance_->postProcessGroups_.end()){
        it->second.isActive = isActive;
    }
}
