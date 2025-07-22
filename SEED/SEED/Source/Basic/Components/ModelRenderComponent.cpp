#include "ModelRenderComponent.h"

////////////////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////////////////
ModelRenderComponent::ModelRenderComponent(GameObject* pOwner, const std::string& tagName)
    : IComponent(pOwner, tagName){
    // モデルの初期化
    model_ = std::make_unique<Model>("DefaultAssets/cube/cube.obj");
    model_->parentMat_ = owner_->GetWorldMatPtr();
    model_->UpdateMatrix();

    if(tagName == ""){
        componentTag_ = "ModelRender_ID:" + std::to_string(componentID_);
    }
}

////////////////////////////////////////////////////////////////////////////
// 初期化
////////////////////////////////////////////////////////////////////////////
void ModelRenderComponent::Initialize(const std::string& modelPath){
    // モデルの初期化
    model_ = std::make_unique<Model>(modelPath);
    model_->parentMat_ = owner_->GetWorldMatPtr();
    model_->UpdateMatrix();
}

////////////////////////////////////////////////////////////////////////////
// フレーム開始時の処理
////////////////////////////////////////////////////////////////////////////
void ModelRenderComponent::BeginFrame(){
}

////////////////////////////////////////////////////////////////////////////
// 更新処理
////////////////////////////////////////////////////////////////////////////
void ModelRenderComponent::Update(){
    model_->Update();
}

////////////////////////////////////////////////////////////////////////////
// 描画処理
////////////////////////////////////////////////////////////////////////////
void ModelRenderComponent::Draw(){
    model_->Draw();
}

////////////////////////////////////////////////////////////////////////////
// フレーム終了時の処理
////////////////////////////////////////////////////////////////////////////
void ModelRenderComponent::EndFrame(){
}


/////////////////////////////////////////////////////////////////////////////
// 終了処理
/////////////////////////////////////////////////////////////////////////////
void ModelRenderComponent::Finalize(){
}

////////////////////////////////////////////////////////////////////////////
// GUI編集
////////////////////////////////////////////////////////////////////////////
void ModelRenderComponent::EditGUI(){
#ifdef _DEBUG
    static std::vector<std::string> modelFiles;
    std::string label;
    ImGui::Indent();

    // モデルの変更
    label = "モデル変更##" + std::to_string(componentID_);
    static std::filesystem::path modelPath = "Resources/Models/";
    std::string path = ImFunc::FolderView(label.c_str(), modelPath, false, { "obj","gltf","glb" });
    if(path != ""){
        ChangeModel(path);
    }

    // localなTransformの編集
    label = "トランスフォーム##" + std::to_string(componentID_);
    if(ImGui::CollapsingHeader(label.c_str())){
        ImGui::Indent();
        ImGui::DragFloat3("Translate", &model_->transform_.translate.x, 0.1f);
        ImGui::DragFloat3("Rotate", &eulerAngles_.x, 0.05f);
        ImGui::DragFloat3("Scale", &model_->transform_.scale.x, 0.1f);
        model_->transform_.rotate = Quaternion::ToQuaternion(eulerAngles_);
        ImGui::Unindent();
    }

    // レンダリング系
    label = "レンダリング設定##" + std::to_string(componentID_);
    if(ImGui::CollapsingHeader(label.c_str())){
        ImGui::Indent();
        ImGui::ColorEdit4("色", &model_->masterColor_.x);
        ImFunc::Combo("ライティング", model_->lightingType_, { "なし","ランバート","ハーフランバート" });
        ImFunc::Combo("ブレンドモード", model_->blendMode_, { "NONE","MULTIPLY","SUBTRACT","NORMAL","ADD","SCREEN" });
        ImFunc::Combo("カリング設定", model_->cullMode_, { "なし","背面","全面" }, 1);
        ImGui::Unindent();
    }

    // アニメーションデータがあれば編集
    if(model_->hasAnimation_){
        label = "アニメーション設定##" + std::to_string(componentID_);
        if(ImGui::CollapsingHeader(label.c_str())){
            ImGui::Indent();
            if(ImGui::Checkbox("アニメーションするか", &model_->isAnimation_)){
                if(model_->isAnimation_){
                    model_->StartAnimation(0, true);
                }
            }

            if(model_->isAnimation_){
                // アニメーションの編集
                std::string selected = model_->animationName_;
                if(ImFunc::ComboText("アニメーション選択", selected, model_->GetAnimationNames())){
                    model_->StartAnimation(selected, model_->isAnimationLoop_, model_->animationSpeedRate_);
                }
                ImGui::Checkbox("ループ再生", &model_->isAnimationLoop_);
                ImGui::DragFloat("再生速度倍率", &model_->animationSpeedRate_, 0.01f);
                ImGui::SliderFloat("アニメーション補間時間", &model_->kAnimLerpTime_, 0.0f, 1.0f);
                ImGui::Text("%.2f/%.2f秒", model_->animationTime_, model_->animationDuration_);
                ImGui::Text("補間%.2f%", model_->progressOfAnimLerp_);
            }

            ImGui::Unindent();
        }
    }

    ImGui::Unindent();
#endif // _DEBUG
}


////////////////////////////////////////////////////////////////////////////
// jsonデータの取得
////////////////////////////////////////////////////////////////////////////
nlohmann::json ModelRenderComponent::GetJsonData() const{
    nlohmann::json j;

    // モデルの情報を書き込み
    j["componentType"] = "ModelRender";
    j.update(IComponent::GetJsonData());
    j["modelFilePath"] = model_->modelName_;
    j["transform"]["translate"] = model_->transform_.translate;
    j["transform"]["rotate"] = model_->transform_.rotate;
    j["transform"]["scale"] = model_->transform_.scale;
    // レンダリング設定
    j["masterColor"] = model_->masterColor_;
    j["lightingType"] = (int)model_->lightingType_;
    j["blendMode"] = (int)model_->blendMode_;
    j["cullMode"] = (int)model_->cullMode_;
    // アニメーション設定
    j["isAnimation"] = model_->isAnimation_;
    j["animationName"] = model_->animationName_;
    j["isAnimationLoop"] = model_->isAnimationLoop_;
    j["animationSpeedRate"] = model_->animationSpeedRate_;
    j["kAnimLerpTime"] = model_->kAnimLerpTime_;

    return j;
}

////////////////////////////////////////////////////////////////////////////
// jsonデータからの読み込み
////////////////////////////////////////////////////////////////////////////
void ModelRenderComponent::LoadFromJson(const nlohmann::json& jsonData){
    // モデルのファイルパスを取得
    std::string modelFilePath = jsonData["modelFilePath"];

    // モデルの初期化
    Initialize(modelFilePath);
    // 基礎情報
    componentTag_ = jsonData["componentTag"];
    // Transformの情報を読み込み
    model_->transform_.translate = jsonData["transform"]["translate"];
    model_->transform_.rotate = jsonData["transform"]["rotate"];
    model_->transform_.scale = jsonData["transform"]["scale"];
    // レンダリング設定
    model_->masterColor_ = jsonData["masterColor"];
    model_->lightingType_ = static_cast<LIGHTING_TYPE>(jsonData["lightingType"]);
    model_->blendMode_ = static_cast<BlendMode>(jsonData["blendMode"]);
    model_->cullMode_ = static_cast<D3D12_CULL_MODE>(jsonData["cullMode"]);
    // アニメーション設定
    model_->isAnimation_ = jsonData["isAnimation"];
    model_->animationName_ = jsonData["animationName"];
    model_->isAnimationLoop_ = jsonData["isAnimationLoop"];
    model_->animationSpeedRate_ = jsonData["animationSpeedRate"];
    model_->kAnimLerpTime_ = jsonData["kAnimLerpTime"];

    // モデルのワールド行列を更新
    model_->UpdateMatrix();

    // フラグがあればアニメーションを開始
    if(model_->isAnimation_){
        model_->StartAnimation(model_->animationName_, model_->isAnimationLoop_, model_->animationSpeedRate_);
    }
}
