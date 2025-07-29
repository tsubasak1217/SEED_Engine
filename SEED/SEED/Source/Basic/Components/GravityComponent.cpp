#include "GravityComponent.h"
#include <SEED/Source/Basic/Object/GameObject.h>

/////////////////////////////////////////////////////////////////////////
// コンストラクタ
/////////////////////////////////////////////////////////////////////////
GravityComponent::GravityComponent(GameObject* pOwner, const std::string& tagName)
    : IComponent(pOwner, tagName){
    if(tagName == ""){
        componentTag_ = "Physics_ID:" + std::to_string(componentID_);
    }
}

/////////////////////////////////////////////////////////////////////////
// 初期化処理
/////////////////////////////////////////////////////////////////////////
void GravityComponent::Initialize(){
    // 重力方向の初期化(下方向)
    gravityDirection_ = Vector3(0.0f, -1.0f, 0.0f);
    // ベロシティの初期化
    velocity_ = Vector3();
    // 落下フラグの初期化
    isDrop_ = false;
    // 落下速度の初期化
    dropSpeed_ = 0.0f;
}

/////////////////////////////////////////////////////////////////////////
// フレーム開始時の処理
/////////////////////////////////////////////////////////////////////////
void GravityComponent::BeginFrame(){
    // 落下フラグの初期化
    isDrop_ = true;
}

/////////////////////////////////////////////////////////////////////////
// 更新処理
/////////////////////////////////////////////////////////////////////////
void GravityComponent::Update(){
    // ワールド基準でベロシティを加算する
    owner_->AddWorldTranslate(velocity_ * ClockManager::DeltaTime());
}


/////////////////////////////////////////////////////////////////////////
// 描画処理
/////////////////////////////////////////////////////////////////////////
void GravityComponent::Draw(){
}


/////////////////////////////////////////////////////////////////////////
// コンストラクタ
/////////////////////////////////////////////////////////////////////////
void GravityComponent::EndFrame(){

    // 落下フラグの更新(一時的にy0で止めるようにしておく)
    if(owner_->GetWorldTranslate().y <= 0.0f){
        isDrop_ = false;
    }

    // 終了時の落下フラグに応じた処理
    if(isDrop_){
        float downAccel = isUseCostomGravity_? customGravity_ : Physics::kGravity * ClockManager::DeltaTime();
        dropSpeed_ += downAccel;
        velocity_ = gravityDirection_ * dropSpeed_;
    } else{
        dropSpeed_ = 0.0f;
        velocity_ = Vector3(0.0f);
    }
}


/////////////////////////////////////////////////////////////////////////
// フレーム終了時の処理
/////////////////////////////////////////////////////////////////////////
void GravityComponent::Finalize(){
}


/////////////////////////////////////////////////////////////////////////
// GUI上での編集処理
/////////////////////////////////////////////////////////////////////////
void GravityComponent::EditGUI(){
#ifdef _DEBUG

    ImGui::Indent();

    // 編集
    ImGui::DragFloat3("重力方向", &gravityDirection_.x, 0.01f, -1.0f, 1.0f, "%.2f");
    ImGui::Checkbox("カスタム重力を使用", &isUseCostomGravity_);
    if(isUseCostomGravity_){
        ImGui::DragFloat("カスタム重力", &customGravity_, 0.01f);
    }

    ImGui::Unindent();

#endif // _DEBUG
}


/////////////////////////////////////////////////////////////////////////
// jsonへの書き出し
/////////////////////////////////////////////////////////////////////////
nlohmann::json GravityComponent::GetJsonData() const{
    
    nlohmann::ordered_json jsonData;
    jsonData["componentType"] = "Gravity";
    jsonData.update(IComponent::GetJsonData());

    // 固有な情報を追加
    jsonData["isUseCustomGravity"] = isUseCostomGravity_;
    jsonData["customGravity"] = customGravity_;
    jsonData["gravityDirection"] = gravityDirection_;

    return jsonData;
}


/////////////////////////////////////////////////////////////////////////
// jsonからの読み込み処理
/////////////////////////////////////////////////////////////////////////
void GravityComponent::LoadFromJson(const nlohmann::json& jsonData){
    IComponent::LoadFromJson(jsonData);
    // 固有な情報を読み込み
    isUseCostomGravity_ = jsonData.value("isUseCustomGravity", false);
    customGravity_ = jsonData.value("customGravity", Physics::kGravity);
    gravityDirection_ = jsonData.value("gravityDirection", Vector3(0.0f, -1.0f, 0.0f));
}
