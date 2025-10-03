#include "UIComponent.h"

//////////////////////////////////////////////////////////////////////////
// コンストラクタ
//////////////////////////////////////////////////////////////////////////
UIComponent::UIComponent(GameObject2D* pOwner, const std::string& tagName)
    : IComponent(pOwner, tagName){

    if(tagName == ""){
        componentTag_ = "UI_ID:" + std::to_string(componentID_);
    }
}


//////////////////////////////////////////////////////////////////////////
// フレーム開始時の処理
//////////////////////////////////////////////////////////////////////////
void UIComponent::BeginFrame(){

}

//////////////////////////////////////////////////////////////////////////
// 更新処理
//////////////////////////////////////////////////////////////////////////
void UIComponent::Update(){

}

//////////////////////////////////////////////////////////////////////////
// 描画処理
//////////////////////////////////////////////////////////////////////////
void UIComponent::Draw(){
    // スプライトの描画
    for(auto& spritePair : sprites_){
        Sprite sprite = spritePair.second;
        sprite.color *= masterColor_;
        sprite.Draw();
    }
    // テキストの描画
    for(auto& textPair : texts_){
        TextBox2D textBox = textPair.second;
        textBox.color *= masterColor_;
        textBox.Draw();
    }
}

//////////////////////////////////////////////////////////////////////////
// フレーム終了時の処理
//////////////////////////////////////////////////////////////////////////
void UIComponent::EndFrame(){
}


//////////////////////////////////////////////////////////////////////////
// 終了処理
//////////////////////////////////////////////////////////////////////////
void UIComponent::Finalize(){
}

//////////////////////////////////////////////////////////////////////////
// GUI編集
//////////////////////////////////////////////////////////////////////////
void UIComponent::EditGUI(){
#ifdef _DEBUG
    {
        ImGui::Indent();
        static std::string label{};

        // スプライト
        {
            label = "Sprites##" + std::to_string(componentID_);
            ImGui::SeparatorText(label.c_str());

            // スプライトの追加
            label = "スプライトの追加##" + std::to_string(componentID_);
            if(ImGui::Button(label.c_str())){
                auto& sprite = sprites_.emplace_back("sprite" + std::to_string(spriteCount_++), Sprite());
                sprite.second.parentMat = owner_.owner2D->GetWorldMatPtr();
            }

            {
                ImGui::Indent();
                // スプライト一覧
                for(size_t i = 0; i < sprites_.size(); i++){


                    // スプライトの削除
                    label = "削除##" + std::to_string(i);
                    if(ImGui::Button(label.c_str())){
                        sprites_.erase(sprites_.begin() + i);
                        break;
                    }

                    // スプライトの編集
                    ImGui::SameLine();
                    auto& spritePair = sprites_[i];
                    label = spritePair.first + "##" + std::to_string(componentID_);

                    if(ImGui::CollapsingHeader(label.c_str())){
                        ImGui::Indent();

                        // 名前の編集
                        label = "スプライト名##" + std::to_string(i);
                        ImFunc::InputText(label.c_str(), spritePair.first);
                        // スプライトの編集
                        spritePair.second.Edit();

                        ImGui::Unindent();
                    }
                }
                ImGui::Unindent();
            }
        }

        // テキスト
        {
            label = "Texts##" + std::to_string(componentID_);
            ImGui::SeparatorText(label.c_str());

            // テキストの追加
            label = "テキストの追加##" + std::to_string(componentID_);
            if(ImGui::Button(label.c_str())){
                auto& text = texts_.emplace_back("text" + std::to_string(textCount_++), TextBox2D("テキスト"));
                text.second.parentMat = owner_.owner2D->GetWorldMatPtr();
            }

            {
                ImGui::Indent();
                // テキスト一覧
                for(size_t i = 0; i < texts_.size(); i++){
                    
                    // テキストの削除
                    label = "削除##" + std::to_string(i);
                    if(ImGui::Button(label.c_str())){
                        texts_.erase(texts_.begin() + i);
                        break;
                    }

                    // テキストの編集
                    auto& textPair = texts_[i];
                    label = textPair.first + "##" + std::to_string(componentID_);
                    ImGui::SameLine();

                    if(ImGui::CollapsingHeader(label.c_str())){
                        ImGui::Indent();

                        // 名前の編集
                        label = "テキスト名##" + std::to_string(i);
                        ImFunc::InputText(label.c_str(), textPair.first);

                        // テキストの編集
                        textPair.second.Edit();
                        ImGui::Unindent();
                    }
                }
                ImGui::Unindent();
            }
        }
        ImGui::Unindent();
    }
#endif
}

//////////////////////////////////////////////////////////////////////////
// スプライトの取得
//////////////////////////////////////////////////////////////////////////
Sprite& UIComponent::GetSprite(size_t index){
    if(index >= sprites_.size()){
        throw std::out_of_range("UIComponent::GetSprite(): index out of range");
    }
    return sprites_[index].second;
}

Sprite& UIComponent::GetSprite(const std::string& name){
    for(auto& spritePair : sprites_){
        if(spritePair.first == name){
            return spritePair.second;
        }
    }
    throw std::out_of_range("UIComponent::GetSprite(): name not found");
}

//////////////////////////////////////////////////////////////////////////
// テキストの取得
//////////////////////////////////////////////////////////////////////////
TextBox2D& UIComponent::GetText(size_t index){
    if(index >= texts_.size()){
        throw std::out_of_range("UIComponent::GetText(): index out of range");
    }
    return texts_[index].second;
}

TextBox2D& UIComponent::GetText(const std::string& name){
    for(auto& textPair : texts_){
        if(textPair.first == name){
            return textPair.second;
        }
    }
    throw std::out_of_range("UIComponent::GetText(): name not found");
}




//////////////////////////////////////////////////////////////////////////
// Jsonデータの読み込み
//////////////////////////////////////////////////////////////////////////
void UIComponent::LoadFromJson(const nlohmann::json& jsonData){
    IComponent::LoadFromJson(jsonData);

    // sprite情報
    sprites_.clear();
    if(jsonData.contains("sprites")){
        for(auto& spriteJson : jsonData["sprites"]){
            Sprite sprite;
            sprite.FromJson(spriteJson);
            sprite.parentMat = owner_.owner2D->GetWorldMatPtr();
            sprites_.emplace_back(spriteJson["name"], sprite);
        }
    }

    // text情報
    texts_.clear();
    if(jsonData.contains("texts")){
        for(auto& textJson : jsonData["texts"]){
            TextBox2D text;
            text.LoadFromJson(textJson);
            text.parentMat = owner_.owner2D->GetWorldMatPtr();
            texts_.emplace_back(textJson["name"], text);
        }
    }
}


//////////////////////////////////////////////////////////////////////////
// Jsonデータの出力
//////////////////////////////////////////////////////////////////////////
nlohmann::json UIComponent::GetJsonData() const{
    nlohmann::ordered_json jsonData;
    jsonData["componentType"] = "UI";
    jsonData.update(IComponent::GetJsonData());

    // sprite情報
    for(auto& spritePair : sprites_){
        jsonData["sprites"].push_back(spritePair.second.ToJson());
        jsonData["sprites"].back()["name"] = spritePair.first;
    }

    // text情報
    for(auto& textPair : texts_){
        jsonData["texts"].push_back(textPair.second.GetJsonData());
        jsonData["texts"].back()["name"] = textPair.first;
    }

    return jsonData;
}
