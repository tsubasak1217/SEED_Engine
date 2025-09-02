#include "BlockComponent.h"


BlockComponent::BlockComponent(GameObject2D* pOwner, const std::string& tagName) : IComponent(pOwner, tagName){

    // タグの名前が指定されていなければIDをタグ名にする
    if(tagName == ""){
        componentTag_ = "BlockComponent_ID:" + std::to_string(componentID_);
    }
}

void BlockComponent::Initialize(){
}

void BlockComponent::BeginFrame(){
}

void BlockComponent::Update(){
    // blockの更新
    //block_->Update();
}

void BlockComponent::Draw(){
    // blockの描画
    //block_->Draw();
}

void BlockComponent::EndFrame(){
}

void BlockComponent::Finalize(){
}

//////////////////////////////////////////////////////////////////////////////
// GUI編集
//////////////////////////////////////////////////////////////////////////////
void BlockComponent::EditGUI(){
#ifdef _DEBUG

    ImGui::Indent();

    // 編集

    ImGui::Unindent();

#endif // _DEBUG

}

//////////////////////////////////////////////////////////////////////////////
// jsonデータの取得
//////////////////////////////////////////////////////////////////////////////
nlohmann::json BlockComponent::GetJsonData() const{
    nlohmann::json jsonData;
    jsonData["componentType"] = "Block";
    jsonData.update(IComponent::GetJsonData());
    
    // block情報をjsonに書き込む

    return jsonData; // JSONデータを返す
}

//////////////////////////////////////////////////////////////////////////////
// jsonデータからの読み込み
////////////////////////////////////////////////////////////////////////////////
void BlockComponent::LoadFromJson(const nlohmann::json& jsonData){
    IComponent::LoadFromJson(jsonData); // 基底クラスのjsonデータを読み込み

    // block情報をjsonから読み込む

}