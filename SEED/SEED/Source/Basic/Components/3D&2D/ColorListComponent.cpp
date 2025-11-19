#include "ColorListComponent.h"
#include <SEED/Source/Basic/Object/GameObject.h>
#include <SEED/Source/Basic/Object/GameObject2D.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////
// コンストラクタ
/////////////////////////////////////////////////////////////////////////////////////////////////////
ColorListComponent::ColorListComponent(std::variant<GameObject*, GameObject2D*> pOwner, const std::string& tagName)
    : IComponent(pOwner, tagName){
    if(tagName == ""){
        componentTag_ = "ColorList_ID:" + std::to_string(componentID_);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// 初期化
/////////////////////////////////////////////////////////////////////////////////////////////////////
void ColorListComponent::Initialize(){
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// フレーム開始時処理
/////////////////////////////////////////////////////////////////////////////////////////////////////
void ColorListComponent::BeginFrame(){
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
// 更新処理
/////////////////////////////////////////////////////////////////////////////////////////////////////
void ColorListComponent::Update(){
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// 描画処理
/////////////////////////////////////////////////////////////////////////////////////////////////////
void ColorListComponent::Draw(){
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// フレーム終了時処理
/////////////////////////////////////////////////////////////////////////////////////////////////////
void ColorListComponent::EndFrame(){
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// 終了処理
/////////////////////////////////////////////////////////////////////////////////////////////////////
void ColorListComponent::Finalize(){
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// アクセッサ
/////////////////////////////////////////////////////////////////////////////////////////////////////

// 名前から色を取得
const Color& ColorListComponent::GetColor(const std::string& name) const{

    for(int32_t i = 0; i < colorNameList_.size(); i++){
        if(colorNameList_[i] == name){
            return colorList_[i];
        }
    }

    // 見つからなかった場合は無効な色を返す
    static Color nullColor = Color(0.0f);
    return nullColor;
}

// 要素数から色を取得
const Color& ColorListComponent::GetColor(uint32_t index) const{
    if(index < colorList_.size()){
        return colorList_[index];
    }

    // 範囲外の場合は無効な色を返す
    static Color nullColor = Color(0.0f);
    return nullColor;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
// 編集処理
/////////////////////////////////////////////////////////////////////////////////////////////////////
void ColorListComponent::EditGUI(){
#ifdef _DEBUG
    ImGui::Indent();
    static std::string hash = "";

    // 色リストの表示・編集
    for(int32_t i = 0; i < colorList_.size(); i++){

        // 削除ボタン
        hash = MyFunc::PtrToStr(&colorNameList_[i]);
        if(ImGui::Button("削除##" + hash)){
            colorList_.erase(colorList_.begin() + i);
            colorNameList_.erase(colorNameList_.begin() + i);
            break;
        }

        // 名前と色の編集
        ImGui::SameLine();
        ImFunc::InputText("名前##" + hash, colorNameList_[i]);
        ImGui::SameLine();
        ImGui::ColorPicker4("色##" + hash, &colorList_[i].value.x);
    }

    // 色の追加
    hash = MyFunc::PtrToStr(this);
    ImGui::SeparatorText("新規色の追加");
    if(ImGui::Button("追加##" + hash)){
        colorList_.emplace_back(Color(1.0f));
        colorNameList_.emplace_back("Color" + std::to_string(colorList_.size() - 1));
    }

    ImGui::Unindent();
#endif // _DEBUG
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// json関連
/////////////////////////////////////////////////////////////////////////////////////////////////////
nlohmann::json ColorListComponent::GetJsonData() const{
    nlohmann::json jsonData;

    // 基本情報
    jsonData["componentType"] = "ColorList";
    jsonData.update(IComponent::GetJsonData());

    // 色情報の書き出し
    for(int32_t i = 0; i < colorList_.size(); i++){
        nlohmann::json colorJson;
        colorJson["name"] = colorNameList_[i];
        colorJson["color"] = colorList_[i];
        jsonData["colors"].push_back(colorJson);
    }

    return jsonData;
}

void ColorListComponent::LoadFromJson(const nlohmann::json& jsonData){
    IComponent::LoadFromJson(jsonData);

    // 色情報の読み込み
    colorList_.clear();
    if(jsonData.contains("colors")){
        for(const auto& colorJson : jsonData["colors"]){
            std::string name = colorJson["name"];
            Color color = colorJson["color"].get<Color>();
            colorList_.emplace_back(color);
            colorNameList_.emplace_back(name);
        }
    }
}
