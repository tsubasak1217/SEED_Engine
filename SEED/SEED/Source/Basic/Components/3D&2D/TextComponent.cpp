#include "TextComponent.h"


TextComponent::TextComponent(std::variant<GameObject*, GameObject2D*> pOwner, const std::string& tagName) : IComponent(pOwner, tagName){
    // テキストボックスの初期化
    textBox_ = std::make_unique<TextBox2D>("テキスト");
    textBox_->SetFont("DefaultAssets/M_PLUS_Rounded_1c/MPLUSRounded1c-Regular.ttf");
    if(tagName == ""){
        componentTag_ = "TextComponent_ID:" + std::to_string(componentID_);
    }
}

void TextComponent::Initialize(const std::string& text, const Vector2& position, const Vector4& color){
    textBox_->text = text; // テキストを設定
    textBox_->transform.translate = position; // 位置を設定
    textBox_->color = color; // 色を設定
}

void TextComponent::BeginFrame(){
}

void TextComponent::Update(){
}

void TextComponent::Draw(){
    if(textBox_){
        textBox_->Draw(); // テキストボックスを描画
    }
}

void TextComponent::EndFrame(){
}

void TextComponent::Finalize(){
}

//////////////////////////////////////////////////////////////////////////////
// GUI編集
//////////////////////////////////////////////////////////////////////////////
void TextComponent::EditGUI(){
#ifdef _DEBUG

    ImGui::Indent();

    // 編集
    if(textBox_){
        textBox_->Edit();
    }

    ImGuiManager::RegisterGuizmoItem(&textBox_->transform);

    ImGui::Unindent();

#endif // _DEBUG

}

//////////////////////////////////////////////////////////////////////////////
// jsonデータの取得
//////////////////////////////////////////////////////////////////////////////
nlohmann::json TextComponent::GetJsonData() const{
    nlohmann::json jsonData;
    jsonData["componentType"] = "Text";
    jsonData.update(IComponent::GetJsonData());
    jsonData["textBox"].update(textBox_->GetJsonData()); // TextBox2Dのjsonデータを追加
    return jsonData; // JSONデータを返す
}

//////////////////////////////////////////////////////////////////////////////
// jsonデータからの読み込み
////////////////////////////////////////////////////////////////////////////////
void TextComponent::LoadFromJson(const nlohmann::json& jsonData){
    IComponent::LoadFromJson(jsonData); // 基底クラスのjsonデータを読み込み

    // TextBox2Dのjsonデータを読み込み
    if(jsonData.contains("textBox")){
        textBox_ = std::make_unique<TextBox2D>();
        textBox_->LoadFromJson(jsonData["textBox"]);
    } else{
        textBox_ = nullptr; // テキストボックスがない場合はnullptrに設定
    }
}