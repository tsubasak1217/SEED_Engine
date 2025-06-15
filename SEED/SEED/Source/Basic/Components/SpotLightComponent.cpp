#include "SpotLightComponent.h"
#include <SEED/Source/SEED.h>
#include <SEED/Source/Basic/Object/GameObject.h>

//////////////////////////////////////////////////////////////////////////////
// コンストラクタ・デストラクタ
//////////////////////////////////////////////////////////////////////////////
SpotLightComponent::SpotLightComponent(GameObject* pOwner, const std::string& tagName) : IComponent(pOwner, tagName){
    // ライトの初期化
    light_ = std::make_unique<SpotLight>();
    light_->color_ = Vector4(1.0f, 1.0f, 1.0f, 1.0f); // デフォルトの色
    light_->intensity = 1.0f; // デフォルトの強さ
    light_->isLighting_ = true; // ライティングを有効にする

    // デフォルトの位置
    localTransform_.translate = Vector3(0.0f, 3.0f, 0.0f);

    // タグ名が空の場合は自動生成
    if(tagName == ""){
        componentTag_ = "SpotLight_ID:" + std::to_string(componentID_);
    }
}

//////////////////////////////////////////////////////////////////////////////
// フレーム開始時の処理
//////////////////////////////////////////////////////////////////////////////
void SpotLightComponent::BeginFrame(){
}

//////////////////////////////////////////////////////////////////////////////
// 更新処理
//////////////////////////////////////////////////////////////////////////////
void SpotLightComponent::Update(){

    // transformの更新
    Matrix4x4 localRotMat = RotateMatrix(localTransform_.rotate);
    Matrix4x4 worldRotMat = localRotMat;
    Vector3 rotated = Vector3(0.0f, -1.0f, 0.0f);
    Vector3 positionVec = localTransform_.translate;

    // 親の回転を反映する場合
    if(isParentRotate_){
        positionVec *= RotateMatrix(owner_->GetWorldRotate());
        worldRotMat = RotateMatrix(owner_->GetWorldRotate()) * localRotMat;
    }

    // 最終結果を反映
    rotated *= worldRotMat;
    light_->position = owner_->GetWorldTranslate() + positionVec;
    light_->direction = rotated;
}

//////////////////////////////////////////////////////////////////////////////
// 描画処理
//////////////////////////////////////////////////////////////////////////////
void SpotLightComponent::Draw(){

    // ライト情報を送る
    light_->SendData();

#ifdef _DEBUG
    // ライト描画
    SEED::DrawSphere(light_->position, 1.0f, 6, light_->color_);
    Vector3 lightVec = light_->direction * light_->distance;
    SEED::DrawLine(light_->position, light_->position + lightVec, light_->color_);
#endif // _DEBUG
}

//////////////////////////////////////////////////////////////////////////////
// フレーム終了時の処理
//////////////////////////////////////////////////////////////////////////////
void SpotLightComponent::EndFrame(){
}

//////////////////////////////////////////////////////////////////////////////
// 終了処理
//////////////////////////////////////////////////////////////////////////////
void SpotLightComponent::Finalize(){
}

//////////////////////////////////////////////////////////////////////////////
// GUI上での編集
//////////////////////////////////////////////////////////////////////////////

void SpotLightComponent::EditGUI(){
#ifdef _DEBUG

    // guizmoに登録
    ImGuiManager::RegisterGuizmoItem(&localTransform_, true,owner_->GetWorldMat());

    std::string label = componentTag_ + "##" + std::to_string(componentID_);
    if(ImGui::CollapsingHeader(label.c_str())){
        ImGui::Indent();

        // 編集
        ImGui::Text("------- 親子付け関連 ------");
        ImGui::Checkbox("親の回転を反映", &isParentRotate_);
        ImGui::Spacing();

        ImGui::Text("------- ライトの設定 ------");
        ImGui::ColorEdit4("色", (float*)&light_->color_);
        ImGui::DragFloat("光の強さ", &light_->intensity, 0.01f, 0.0f, 10.0f);
        ImGui::Checkbox("ライティング有効", &light_->isLighting_);
        ImGui::DragFloat3("ローカル位置", &localTransform_.translate.x, 0.01f);
        ImGui::DragFloat3("回転(方向)", &localTransform_.rotate.x, 0.01f);
        ImGui::DragFloat("距離", &light_->distance, 0.1f, 0.0f);
        ImGui::DragFloat("減衰率", &light_->decay, 0.1f, 0.0f);
        ImGui::DragFloat("角度", &light_->cosAngle, 0.01f, 0.0f, 3.14f);
        ImGui::DragFloat("フェード開始角度", &light_->cosFallofStart, 0.01f, 0.0f, 1.0f);

        Matrix4x4 rotMat = RotateMatrix(localTransform_.rotate);
        Vector3 rot1 = localTransform_.rotate;
        Vector3 rot2 = ExtractRotation(rotMat);
        Quaternion q1 = localTransform_.rotateQuat;
        Quaternion q2 = Quaternion::MatrixToQuaternion(rotMat);
        ImGui::Text("回転値1: (%.2f, %.2f, %.2f)", rot1.x, rot1.y, rot1.z);
        ImGui::Text("回転値2: (%.2f, %.2f, %.2f)", rot2.x, rot2.y, rot2.z);
        ImGui::Text("Quaternion1: (%.2f, %.2f, %.2f, %.2f)",q1.x, q1.y, q1.z, q1.w);
        ImGui::Text("Quaternion2: (%.2f, %.2f, %.2f, %.2f)", q2.x, q2.y, q2.z, q2.w);
        ImGui::Unindent();
    }

#endif // _DEBUG

}

//////////////////////////////////////////////////////////////////////////////
// jsonデータの取得
//////////////////////////////////////////////////////////////////////////////
nlohmann::json SpotLightComponent::GetJsonData() const{
    nlohmann::json jsonData;
    jsonData["componentType"] = "SpotLight";
    jsonData.update(IComponent::GetJsonData());
    jsonData["position"] = localTransform_.translate;
    jsonData["direction"] = localTransform_.rotate;
    jsonData["color"] = { light_->color_.x, light_->color_.y, light_->color_.z, light_->color_.w };
    jsonData["intensity"] = light_->intensity;
    jsonData["distance"] = light_->distance;
    jsonData["decay"] = light_->decay;
    jsonData["cosAngle"] = light_->cosAngle;
    jsonData["cosFallofStart"] = light_->cosFallofStart;
    jsonData["isParentRotate"] = isParentRotate_;
    return jsonData;
}

//////////////////////////////////////////////////////////////////////////////
// jsonデータからの読み込み
//////////////////////////////////////////////////////////////////////////////
void SpotLightComponent::LoadFromJson(const nlohmann::json& jsonData){
    IComponent::LoadFromJson(jsonData);
    localTransform_.translate = jsonData["position"];
    localTransform_.rotate = jsonData["direction"];
    light_->color_ = jsonData["color"];
    light_->intensity = jsonData["intensity"];
    light_->distance = jsonData["distance"];
    light_->decay = jsonData["decay"];
    light_->cosAngle = jsonData["cosAngle"];
    light_->cosFallofStart = jsonData["cosFallofStart"];
    isParentRotate_ = jsonData["isParentRotate"];
}
