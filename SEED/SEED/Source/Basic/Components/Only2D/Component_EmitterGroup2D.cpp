#include "Component_EmitterGroup2D.h"
#include <SEED/Source/Manager/ParticleManager/CPUParticle/Emitter/EmitterBase.h>

//////////////////////////////////////////////////////////////////////////
// コンストラクタ
//////////////////////////////////////////////////////////////////////////
Component_EmitterGroup2D::Component_EmitterGroup2D(GameObject* pOwner, const std::string& tagName)
    : IComponent(pOwner, tagName){

    if(tagName == ""){
        componentTag_ = "EmitterGroup2D_ID:" + std::to_string(componentID_);
    }

    // EmitterGroupの生成
    emitterGroup_ = std::make_unique<EmitterGroup2D>(owner_.owner3D->GetWorldMatPtr());
}


//////////////////////////////////////////////////////////////////////////
// フレーム開始時の処理
//////////////////////////////////////////////////////////////////////////
void Component_EmitterGroup2D::BeginFrame(){

}

//////////////////////////////////////////////////////////////////////////
// 更新処理
//////////////////////////////////////////////////////////////////////////
void Component_EmitterGroup2D::Update(){

    if(isActive_){
        for(auto& emitter : emitterGroup_->emitters){
            emitter->Update();
            if(emitter->emitOrder == true){
                // ここでEffectSystemにパーティクルを発生させる
                ParticleManager::Emit(emitter.get());
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// 描画処理
//////////////////////////////////////////////////////////////////////////
void Component_EmitterGroup2D::Draw(){
#ifdef _DEBUG



#endif // _DEBUG
}

//////////////////////////////////////////////////////////////////////////
// フレーム終了時の処理
//////////////////////////////////////////////////////////////////////////
void Component_EmitterGroup2D::EndFrame(){
}


//////////////////////////////////////////////////////////////////////////
// 終了処理
//////////////////////////////////////////////////////////////////////////
void Component_EmitterGroup2D::Finalize(){
}

//////////////////////////////////////////////////////////////////////////
// GUI編集
//////////////////////////////////////////////////////////////////////////
void Component_EmitterGroup2D::EditGUI(){
#ifdef _DEBUG
    ImGui::Indent();

    // エミッターグループの編集
    emitterGroup_->Edit();

    // ownerに親子付けするかどうか
    if(ImGui::Checkbox("オーナーに親子付けする", &isParentToOwner_)){
        if(isParentToOwner_){
            emitterGroup_->parentMat = owner_.owner2D->GetWorldMatPtr();
        } else{
            emitterGroup_->parentMat = nullptr;
        }
    }

    // ファイル一覧を取得
    static const std::filesystem::path rootPath = "Resources/Jsons/Particle/3D";
    static std::filesystem::path curPath = rootPath;
    std::string selected = ImFunc::FolderView("ファイルを選択", curPath, false, { ".emtg3" }, rootPath);

    // 選択されたら読み込み
    if(!selected.empty()){
        nlohmann::json j = MyFunc::GetJson(selected);
        emitterGroup_->emitters.clear(); // 既存のエミッターをクリア
        emitterGroup_->LoadFromJson(j);
        emitterGroup_->TeachParent();
    }

    ImGui::Unindent();
#endif
}

//////////////////////////////////////////////////////////////////////////
// エミッターを初期化する
//////////////////////////////////////////////////////////////////////////
void Component_EmitterGroup2D::InitEmitters(){
    emitterGroup_->InitEmitters();
}


//////////////////////////////////////////////////////////////////////////
// Jsonデータの読み込み
//////////////////////////////////////////////////////////////////////////
void Component_EmitterGroup2D::LoadFromJson(const nlohmann::json& jsonData){
    IComponent::LoadFromJson(jsonData);

    // コンポーネントの設定を読み込み
    isParentToOwner_ = jsonData.value("isParentToOwner", true);
    isActive_ = jsonData.value("isActive", true);

    // エミッターグループの情報を読み込み
    if(jsonData.contains("emitterGroup")){
        emitterGroup_ = std::make_unique<EmitterGroup2D>();
        emitterGroup_->LoadFromJson(jsonData["emitterGroup"]);
        emitterGroup_->parentMat = isParentToOwner_ ? owner_.owner2D->GetWorldMatPtr() : nullptr;
    }
}


//////////////////////////////////////////////////////////////////////////
// Jsonデータの出力
//////////////////////////////////////////////////////////////////////////
nlohmann::json Component_EmitterGroup2D::GetJsonData() const{
    nlohmann::ordered_json jsonData;
    jsonData["componentType"] = "EmitterGroup2D";
    jsonData.update(IComponent::GetJsonData());

    // コンポーネントの設定を保存
    jsonData["isParentToOwner"] = isParentToOwner_;
    jsonData["isActive"] = isActive_;

    // エミッターグループの情報を保存
    jsonData["emitterGroup"] = emitterGroup_->GetJson();

    return jsonData;
}
