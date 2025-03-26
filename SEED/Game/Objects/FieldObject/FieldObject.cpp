#include "FieldObject.h"

#include "LocalFunc.h"

///////////////////////////////////////////////////////////////////
// コンストラクタ・デストラクタ
///////////////////////////////////////////////////////////////////
FieldObject::FieldObject(){ guid_ = GenerateGUID(); }

FieldObject::FieldObject(const std::string& modelName){
    name_ = modelName;
    std::string path = "FieldObject/" + modelName;
    model_ = std::make_unique<Model>(path);
    model_->isRotateWithQuaternion_ = false;
    Initialize();
    BaseObject::Update();

    // ユニークなIDを生成
    guid_ = GenerateGUID();
}

///////////////////////////////////////////////////////////////////
// 初期化関数
///////////////////////////////////////////////////////////////////
void FieldObject::Initialize(){
    isApplyGravity_ = false;
    UpdateMatrix();
}


///////////////////////////////////////////////////////////////////
// 更新関数
///////////////////////////////////////////////////////////////////
void FieldObject::Update(){
    BaseObject::Update();
}

///////////////////////////////////////////////////////////////////
// 描画関数
///////////////////////////////////////////////////////////////////
void FieldObject::Draw(){
    BaseObject::Draw();
}

///////////////////////////////////////////////////////////////////
// Json出力関数
///////////////////////////////////////////////////////////////////
nlohmann::json FieldObject::OutputJson(){
    nlohmann::json json = {
        { "name", name_ },
        { "type", fieldObjectType_ },
        { "position", {
            GetLocalTranslate().x,
            GetLocalTranslate().y,
            GetLocalTranslate().z
        } },
        { "scale", {
            GetLocalScale().x,
            GetLocalScale().y,
            GetLocalScale().z
        } },
        { "rotation", {
            GetLocalRotate().x,
            GetLocalRotate().y,
            GetLocalRotate().z
        } },
        { "fieldObjectID", guid_ }
    };
    
    return json;
}

///////////////////////////////////////////////////////////////////
// Json読み込み関数
///////////////////////////////////////////////////////////////////
void FieldObject::LoadFromJson(const nlohmann::json& json){
    if(json.contains("name")){
        name_ = json["name"];
    }

    if(json.contains("type")){
        fieldObjectType_ = json["type"];
    }

    if(json.contains("position")){
        SetTranslate(json["position"]);
    }

    if(json.contains("scale")){
        SetScale(json["scale"]);
    }

    if(json.contains("rotation")){
        SetRotate(Vector3(json["rotation"]));
    }

    if(json.contains("fieldObjectID")){
        guid_ = json["fieldObjectID"];
    }
}
