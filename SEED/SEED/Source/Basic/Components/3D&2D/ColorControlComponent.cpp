#include "ColorControlComponent.h"
#include <SEED/Source/Basic/Object/GameObject.h>
#include <SEED/Source/Basic/Object/GameObject2D.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////
// コンストラクタ
/////////////////////////////////////////////////////////////////////////////////////////////////////
ColorControlComponent::ColorControlComponent(std::variant<GameObject*, GameObject2D*> pOwner, const std::string& tagName)
    : IComponent(pOwner, tagName){
    if(tagName == ""){
        componentTag_ = "ColorControl_ID:" + std::to_string(componentID_);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// 初期化
/////////////////////////////////////////////////////////////////////////////////////////////////////
void ColorControlComponent::Initialize(){
    // タイマー初期化
    timer_ = Timer(1.0f);
    // カーブ初期化
    colorCurve_ = Curve(CurveChannel::VECTOR4);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// フレーム開始時処理
/////////////////////////////////////////////////////////////////////////////////////////////////////
void ColorControlComponent::BeginFrame(){
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
// 更新処理
/////////////////////////////////////////////////////////////////////////////////////////////////////
void ColorControlComponent::Update(){

    // 色を計算
    Vector4 rgbaColor = Vector4(1);
    if(colorControlMode_ == ColorMode::RGBA){
        rgbaColor = colorCurve_.GetValue4(timer_.GetProgress());

    } else if(colorControlMode_ == ColorMode::HSVA){
        Vector4 hsvaValue = colorCurve_.GetValue4(timer_.GetProgress());
        rgbaColor = MyMath::HSV_to_RGB(hsvaValue);
    }

    // ownerに反映
        if(owner_.is2D){
        if(!isMultiply_){
            // 直接書き換え
            owner_.owner2D->masterColor_ = rgbaColor;
        } else{
            // 乗算
            owner_.owner2D->masterColor_ *= rgbaColor;
        }
    } else{
        if(!isMultiply_){
            // 直接書き換え
            owner_.owner3D->masterColor_ = rgbaColor;
        } else{
            // 乗算
            owner_.owner3D->masterColor_ *= rgbaColor;
        }
    }

    // 時間の更新
    timer_.Update(timeScale_,isLoop_);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// 描画処理
/////////////////////////////////////////////////////////////////////////////////////////////////////
void ColorControlComponent::Draw(){
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// フレーム終了時処理
/////////////////////////////////////////////////////////////////////////////////////////////////////
void ColorControlComponent::EndFrame(){
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// 終了処理
/////////////////////////////////////////////////////////////////////////////////////////////////////
void ColorControlComponent::Finalize(){
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
// 編集処理
/////////////////////////////////////////////////////////////////////////////////////////////////////
void ColorControlComponent::EditGUI(){
#ifdef _DEBUG
    ImGui::Indent();

    // カーブの編集
    if(ImGui::CollapsingHeader("カーブの編集##" + componentTag_)){
        ImGui::Indent();
        colorCurve_.Edit();
        ImGui::Unindent();
    }

    // 設定
    ImGui::SeparatorText("詳細");
    ImGui::DragFloat("総時間##" + componentTag_, &timer_.duration, 0.1f, 0.0f, 1000.0f);
    ImFunc::PlayBar("現在の時間##" + componentTag_, timer_);
    ImFunc::Combo("色の制御方法##" + componentTag_,colorControlMode_, { "RGBA","HSVA" });
    ImGui::DragFloat("タイムスケール##" + componentTag_, &timeScale_, 0.01f);
    ImGui::Checkbox("ループするか##" + componentTag_, &isLoop_);
    ImGui::Checkbox("色を置き換えずに乗算##" + componentTag_, &isMultiply_);

    ImGui::Unindent();
#endif // _DEBUG
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// json関連
/////////////////////////////////////////////////////////////////////////////////////////////////////
nlohmann::json ColorControlComponent::GetJsonData() const{
    nlohmann::json jsonData;
    jsonData["componentType"] = "ColorControl";
    jsonData.update(IComponent::GetJsonData());
    jsonData["curve"] = colorCurve_.ToJson();
    jsonData["time"] = timer_.duration;
    jsonData["timeScale"] = timeScale_;
    jsonData["isLoop"] = isLoop_;
    jsonData["colorControlMode"] = static_cast<int32_t>(colorControlMode_);
    jsonData["isMultiply"] = isMultiply_;
    return jsonData;
}

void ColorControlComponent::LoadFromJson(const nlohmann::json& jsonData){
    IComponent::LoadFromJson(jsonData);
    if(jsonData.contains("curve")){
        colorCurve_.FromJson(jsonData["curve"]);
    }

    if(jsonData.contains("time")){
        timer_.Initialize(jsonData["time"]);
    }

    timeScale_ = jsonData.value("timeScale", 1.0f);
    isLoop_ = jsonData.value("isLoop", false);
    isMultiply_ = jsonData.value("isMultiply", false);
    colorControlMode_ = static_cast<ColorMode>(jsonData.value("colorControlMode", 0));
}
