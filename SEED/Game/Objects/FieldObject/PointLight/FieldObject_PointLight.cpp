#include "FieldObject_PointLight.h"

/////////////////////////////////////////////////////////////
// コンストラクタ
/////////////////////////////////////////////////////////////
FieldObject_PointLight::FieldObject_PointLight(){
    className_ = "FieldObject_PointLight";
    name_ = "PointLight";
    // モデルの初期化
    model_ = std::make_unique<Model>("Assets/Sphere.obj");
    model_->isRotateWithQuaternion_ = false;
    // コライダー関連の初期化
    colliderEditor_ = std::make_unique<ColliderEditor>(className_, this);
    InitColliders(ObjectType::Field);
    // ポイントライトの初期化
    pointLight_ = std::make_unique<PointLight>();
    // 全般の初期化
    FieldObject::Initialize();
}

void FieldObject_PointLight::Initialize(){
}

/////////////////////////////////////////////////////////////
// 更新関数
/////////////////////////////////////////////////////////////
void FieldObject_PointLight::Update(){
    FieldObject::Update();
    pointLight_->position = GetWorldTranslate();
}


/////////////////////////////////////////////////////////////
// 描画関数
/////////////////////////////////////////////////////////////
void FieldObject_PointLight::Draw(){

    // ライトの情報を送る
    pointLight_->SendData();

#ifdef _DEBUG
    FieldObject::Draw();
#endif // _DEBUG
}

/////////////////////////////////////////////////////////////
// 編集関数
/////////////////////////////////////////////////////////////
void FieldObject_PointLight::Edit(){
#ifdef _DEBUG
    // 基本の編集
    FieldObject::Edit();

    // 固有の編集
    ImGui::Text("PointLight Settings");
    ImGui::Separator();
    ImGui::ColorEdit3("color", &pointLight_->color_.x);
    ImGui::DragFloat("intensity", &pointLight_->intensity, 0.2f, 0.0f, 100.0f);
    ImGui::DragFloat("radius", &pointLight_->radius, 0.05f, 0.0f, 100.0f);
    ImGui::DragFloat("decay", &pointLight_->decay, 0.05f, 0.0f, 100.0f);
    ImGui::Separator();
#endif // _DEBUG

}

/////////////////////////////////////////////////////////////
// Json出力
/////////////////////////////////////////////////////////////
nlohmann::json FieldObject_PointLight::OutputJson(){
    nlohmann::json json = FieldObject::OutputJson();
    json["intensity"] = pointLight_->intensity;
    json["radius"] = pointLight_->radius;
    json["color"] = pointLight_->color_;
    json["decay"] = pointLight_->decay;
    return json;
}

/////////////////////////////////////////////////////////////
// Jsonからの読み込み
/////////////////////////////////////////////////////////////
void FieldObject_PointLight::LoadFromJson(const nlohmann::json& json){
    FieldObject::LoadFromJson(json);
    pointLight_->intensity = json.value("intensity", pointLight_->intensity);
    pointLight_->radius = json.value("radius", pointLight_->radius);
    pointLight_->color_ = json.value("color", pointLight_->color_);
    pointLight_->decay = json.value("decay", pointLight_->decay);
}
