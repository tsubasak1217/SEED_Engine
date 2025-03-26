#include "FieldObject_Wood.h"

/////////////////////////////////////////////////////////////////////
// コンストラクタ
/////////////////////////////////////////////////////////////////////
FieldObject_Wood::FieldObject_Wood(){
    className_ = "FieldObject_Wood";
    name_ = "Wood";
    // モデルの初期化
    model_ = std::make_unique<Model>("FieldObject/Wood.obj");
    model_->isRotateWithQuaternion_ = false;
    model_->meshColor_[0] = MyMath::FloatColor(0x8d6400ff);
    model_->meshColor_[1] = leafColor_;
    model_->meshColor_[2] = leafColor_;
    // コライダー関連の初期化
    colliderEditor_ = std::make_unique<ColliderEditor>(className_, this);
    InitColliders(ObjectType::Field);
    // 全般の初期化
    FieldObject::Initialize();
}

/////////////////////////////////////////////////////////////////////
// 初期化関数
/////////////////////////////////////////////////////////////////////
void FieldObject_Wood::Initialize(){
}

/////////////////////////////////////////////////////////////////////
// 更新関数
/////////////////////////////////////////////////////////////////////
void FieldObject_Wood::Update(){
    model_->meshColor_[1] = leafColor_;
    model_->meshColor_[2] = leafColor_;

    FieldObject::Update();
}

/////////////////////////////////////////////////////////////////////
// 描画関数
/////////////////////////////////////////////////////////////////////
void FieldObject_Wood::Draw(){
    FieldObject::Draw();
}

/////////////////////////////////////////////////////////////////////
// 編集関数
/////////////////////////////////////////////////////////////////////
void FieldObject_Wood::Edit(){
#ifdef _DEBUG
    // 基本の編集
    FieldObject::Edit();

    // 固有の編集
    ImGui::Text("Wood Settings");
    ImGui::Separator();
    ImGui::ColorEdit4("Leaf Color", &leafColor_.x);
    ImGui::Separator();
#endif // _DEBUG
}

///////////////////////////////////////////////////////////////////
// Json出力関数
///////////////////////////////////////////////////////////////////
nlohmann::json FieldObject_Wood::OutputJson(){
    nlohmann::json json = FieldObject::OutputJson();
    json["leafColor"] = leafColor_;
    return json;
}
