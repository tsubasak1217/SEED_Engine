#include "GameObject.h"
#include <Game/GameSystem.h>
#include <SEED/Source/Basic/Scene/Scene_Base.h>
#include <SEED/Source/Manager/Hierarchy/Hierarchy.h>


//////////////////////////////////////////////////////////////////////////
// static変数
//////////////////////////////////////////////////////////////////////////
uint32_t GameObject::nextID_ = 0;



//////////////////////////////////////////////////////////////////////////
// コンストラクタ ・ デストラクタ
//////////////////////////////////////////////////////////////////////////
GameObject::GameObject(Scene_Base* pScene){
    objectID_ = nextID_++;
    objectName_ = "BaseObject" + std::to_string(objectID_);
    Initialize();
    // シーンのヒエラルキーに登録
    pScene->RegisterToHierarchy(this);
}

GameObject::~GameObject(){
    // シーンのヒエラルキーから削除
    GameSystem::GetScene()->RemoveFromHierarchy(this);
    // 親子関係の解除
    ReleaseParent();
    // 子オブジェクトのリセット
    ReleaseChildren();

    // コンポーネントの終了処理
    for(auto& component : components_){
        component->Finalize();
    }

    components_.clear();
}


//////////////////////////////////////////////////////////////////////////
// 初期化処理
//////////////////////////////////////////////////////////////////////////
void GameObject::Initialize(){
}



//////////////////////////////////////////////////////////////////////////
// 更新処理
//////////////////////////////////////////////////////////////////////////
void GameObject::Update(){

    // コンポーネントの更新
    for(auto& component : components_){
        if(!component->isActive_){ continue; }
        component->Update();
    }

    // マトリックスの更新
    UpdateMatrix();
}



//////////////////////////////////////////////////////////////////////////
// 描画処理
//////////////////////////////////////////////////////////////////////////
void GameObject::Draw(){
    // コンポーネントの描画
    for(auto& component : components_){
        if(!component->isActive_){ continue; }
        component->Draw();
    }
}



//////////////////////////////////////////////////////////////////////////
// フレーム開始時処理
//////////////////////////////////////////////////////////////////////////
void GameObject::BeginFrame(){

    // 座標の保存
    prePos_ = GetWorldTranslate();

    // コンポーネントの開始処理
    for(auto& component : components_){
        if(!component->isActive_){ continue; }
        component->BeginFrame();
    }

    // velocityを加算
    AddWorldTranslate(velocity_);
}



//////////////////////////////////////////////////////////////////////////
// フレーム終了時処理
//////////////////////////////////////////////////////////////////////////
void GameObject::EndFrame(){

    // コンポーネントの終了処理
    for(auto& component : components_){
        if(!component->isActive_){ continue; }
        component->EndFrame();
    }
}

//////////////////////////////////////////////////////////////////////////
// マトリックスの更新
//////////////////////////////////////////////////////////////////////////
void GameObject::UpdateMatrix(){

    // ローカル変換行列の更新
    localMat_ = AffineMatrix(localTransform_.scale, localTransform_.rotate.ToEuler(), localTransform_.translate);

    // ワールド行列の更新
    if(parent_){

        Matrix4x4 parentMat;
        
        // コンポーネントに親子付けするかどうかで処理を分ける
        if(parentComponentInfo_.pComponent){
            parentMat = *parentComponentInfo_.pMatrix;
        } else{
            parentMat = parent_->worldMat_;
        }

        if(isParentRotate_ + isParentScale_ + isParentTranslate_ == 3){
            worldMat_ = localMat_ * parentMat;

        } else{

            Matrix4x4 cancelMat = IdentityMat4();

            // 親の行列から取り出した要素を打ち消す行列を作成
            if(!isParentScale_){
                Vector3 inverseScale = Vector3(1.0f, 1.0f, 1.0f) / ExtractScale(parentMat);
                cancelMat = cancelMat * ScaleMatrix(inverseScale);
            }

            if(!isParentRotate_){
                Vector3 inverseRotate = ExtractRotation(parentMat) * -1.0f;
                cancelMat = cancelMat * RotateMatrix(inverseRotate);
            }

            if(!isParentTranslate_){
                Vector3 inverseTranslate = ExtractTranslation(parentMat) * -1.0f;
                cancelMat = cancelMat * TranslateMatrix(inverseTranslate);
            }

            Matrix4x4 canceledMat = cancelMat * parentMat;
            worldMat_ = (localMat_ * parentMat) * cancelMat;
            //worldMat_ = localMat_ * canceledMat;
        }
    } else{
        worldMat_ = localMat_;
    }

    worldTransform_.translate = ExtractTranslation(worldMat_);
    worldTransform_.rotate = Quaternion::ToQuaternion(ExtractRotation(worldMat_));
    worldTransform_.scale = ExtractScale(worldMat_);
}


//////////////////////////////////////////////////////////////////////////
// 親子付け関連
//////////////////////////////////////////////////////////////////////////

void GameObject::SetParent(GameObject* parent){
    ReleaseParent();
    parent_ = parent;
    parent->children_.push_back(this);
}

void GameObject::RemoveChild(GameObject* child){
    if(child){
        child->ReleaseParent();
        children_.remove(child);
    }
}

void GameObject::ReleaseParent(){
    if(parent_){
        parent_->children_.remove(this);
        parent_ = nullptr;
    }
}

void GameObject::ReleaseChildren(){
    auto childrenCopy = children_;
    for(auto* child : childrenCopy){
        child->ReleaseParent();
    }
    children_.clear();
}

// 自分の子孫かどうか確認する関数
bool GameObject::IsDescendant(GameObject* obj) const{
    if(obj == nullptr) return false;
    // 子供の中に含まれているか確認
    for(const auto& child : children_){
        if(child->IsDescendant(obj)){
            return true;
        }
    }
    return false;
}

//////////////////////////////////////////////////////////////////////////
// 親子付けとかされてても常にワールド軸基準で指定した方向に移動を追加する関数
//////////////////////////////////////////////////////////////////////////
void GameObject::AddWorldTranslate(const Vector3& addValue){
    if(parent_ != nullptr){
        Matrix4x4 invParentMat = InverseMatrix(RotateMatrix(parent_->GetWorldRotate()));
        Vector3 localAddValue = addValue * invParentMat;
        localTransform_.translate += localAddValue;
    } else{
        localTransform_.translate += addValue;
    }
}




//////////////////////////////////////////////////////////////////////////
// 衝突処理
//////////////////////////////////////////////////////////////////////////  

void GameObject::OnCollision(GameObject* other){

    if(other == nullptr){ return; }
    // 衝突フラグを立てる
    isCollide_ = true;

    // 衝突中の処理
    if(isCollide_){
        OnCollisionStay(other);
    }

    // 衝突開始時の処理
    if(isCollide_ && !preIsCollide_){
        OnCollisionEnter(other);
    }

    // 衝突終了時の処理
    if(!isCollide_ && preIsCollide_){
        OnCollisionExit(other);
    }
}

void GameObject::OnCollisionEnter(GameObject* other){
    // コンポーネントに衝突イベントを通知
    for(auto& component : components_){
        component->OnCollisionEnter(other);
    }
}

void GameObject::OnCollisionStay(GameObject* other){
    // コンポーネントに衝突イベントを通知
    for(auto& component : components_){
        component->OnCollisionStay(other);
    }
}

void GameObject::OnCollisionExit(GameObject* other){
    // コンポーネントに衝突イベントを通知
    for(auto& component : components_){
        component->OnCollisionExit(other);
    }
}


//////////////////////////////////////////////////////////////////////////
// jsonデータの取得
//////////////////////////////////////////////////////////////////////////
nlohmann::json GameObject::GetJsonData(int32_t depth){
    nlohmann::json json;

    // 全般の情報
    json["objectType"] = int(objectType_);
    json["objectName"] = objectName_;

    // 親子関係の情報
    json["isParentRotate"] = isParentRotate_;
    json["isParentScale"] = isParentScale_;
    json["isParentTranslate"] = isParentTranslate_;

    // transformの情報
    json["transform"]["translate"] = localTransform_.translate;
    json["transform"]["rotate"] = localTransform_.rotate;
    json["transform"]["scale"] = localTransform_.scale;

    // コンポーネントの情報
    for(const auto& component : components_){
        json["components"].push_back(component->GetJsonData());
    }

    // 子がいる場合は再帰的に子の情報も取得
    for(const auto& child : children_){
        json["children"].push_back(child->GetJsonData(depth + 1));
    }

    return json;
}

//////////////////////////////////////////////////////////////////////////
// jsonデータから読み込み値を設定する関数
//////////////////////////////////////////////////////////////////////////
void GameObject::LoadFromJson(const nlohmann::json& jsonData){

    // 全般の情報
    objectType_ = static_cast<ObjectType>(jsonData["objectType"]);
    objectName_ = jsonData["objectName"];
    // 親子関係の情報
    isParentRotate_ = jsonData["isParentRotate"];
    isParentScale_ = jsonData["isParentScale"];
    isParentTranslate_ = jsonData["isParentTranslate"];
    // transformの情報
    localTransform_.translate = jsonData["transform"]["translate"];
    localTransform_.rotate = jsonData["transform"]["rotate"];
    localTransform_.scale = jsonData["transform"]["scale"];

    // コンポーネントの情報
    for(const auto& componentJson : jsonData["components"]){
        std::string componentType = componentJson["componentType"];
        if(componentType == "ModelRender"){
            auto* modelComponent = AddComponent<ModelRenderComponent>();
            modelComponent->LoadFromJson(componentJson);

        } else if(componentType == "Collision"){
            auto* collisionComponent = AddComponent<CollisionComponent>();
            collisionComponent->LoadFromJson(componentJson);

        } else if(componentType == "Text"){
            auto* textComponent = AddComponent<TextComponent>();
            textComponent->LoadFromJson(componentJson);

        } else if(componentType == "SpotLight"){
            auto* spotLightComponent = AddComponent<SpotLightComponent>();
            spotLightComponent->LoadFromJson(componentJson);

        }
    }
}

////////////////////////////////////////////////////////////////////////////
// 新しいGameObjectをjsonデータから作成する関数
////////////////////////////////////////////////////////////////////////////
GameObject* GameObject::CreateFromJson(const nlohmann::json& jsonData){
    // 新しいGameObjectを作成
    GameObject* newObject = new GameObject(GameSystem::GetScene());
    // jsonデータから情報を読み込む
    newObject->LoadFromJson(jsonData);
    return newObject;
}

//////////////////////////////////////////////////////////////////////////////
// 家族を再帰的に作成する関数
//////////////////////////////////////////////////////////////////////////////
std::vector<GameObject*> GameObject::CreateFamily(const nlohmann::json& jsonData, GameObject* parent){
    static std::vector<GameObject*> familyObjects;
    if(!parent){
        familyObjects.clear(); // 最初のみ初期化
    }

    // 自分自身を作成し、追加
    GameObject* self = CreateFromJson(jsonData);
    familyObjects.push_back(self);

    // 親がいる場合は親を設定
    if(parent){
        self->SetParent(parent);
    }

    // 子オブジェクトを再帰的に作成
    if(jsonData.find("children") != jsonData.end()){
        for(const auto& childJson : jsonData["children"]){
            CreateFamily(childJson, self);
        }
    }

    // 結果を返す
    return familyObjects;
}


//////////////////////////////////////////////////////////////////////////
// GUI編集処理
//////////////////////////////////////////////////////////////////////////
void GameObject::EditGUI(){
#ifdef _DEBUG

    // オブジェクトの名前を編集
    std::string label = "オブジェクト名##" + std::to_string(objectID_);
    ImFunc::InputText(label.c_str(), objectName_);

    ImGui::Text("------------- トランスフォーム -------------");
    Vector3 info[3] = { GetWorldTranslate(), GetWorldEulerRotate(), GetWorldScale() };
    Vector3 localRotEuler = GetLocalEulerRotate();
    ImGui::Text("位置(ワールド): %.2f, %.2f, %.2f", info[0].x, info[0].y, info[0].z);
    ImGui::Text("位置(ローカル): %.2f, %.2f, %.2f", localTransform_.translate.x, localTransform_.translate.y, localTransform_.translate.z);
    ImGui::Text("回転(ワールド): %.2f, %.2f, %.2f", info[1].x, info[1].y, info[1].z);
    ImGui::Text("回転(ローカル): %.2f, %.2f, %.2f", localRotEuler.x, localRotEuler.y, localRotEuler.z);
    ImGui::Text("スケール(ワールド): %.2f, %.2f, %.2f", info[2].x, info[2].y, info[2].z);
    ImGui::Text("スケール(ローカル): %.2f, %.2f, %.2f", localTransform_.scale.x, localTransform_.scale.y, localTransform_.scale.z);

    ImGui::Text("--------------- ペアレント方式 ---------------");
    ImGui::Checkbox("回転をペアレントする", &isParentRotate_);
    ImGui::Checkbox("スケールをペアレントする", &isParentScale_);
    ImGui::Checkbox("位置をペアレントする", &isParentTranslate_);

    ImGui::Text("------------- コンポーネント一覧 -------------");

    // コンポーネントのGUI編集
    for(auto& component : components_){
        // ラベルの設定
        label = component->componentTag_ + "##" + std::to_string(component->componentID_);
        bool opened = ImGui::CollapsingHeader(label.c_str());

        // 右クリックでコンテキストメニューを表示
        if(ImGui::IsItemClicked(1)){
            contextMenuComponent_ = component.get(); // コンテキストメニューの対象を設定
            ImGui::OpenPopup("ComponentContextMenu");
        }

        // コンポーネントのGUI編集
        if(opened){
            component->EditGUI();
        }
    }

    // コンテキストメニューの表示
    ContextMenu();

    // コンポーネントの追加
    ImGui::Separator();
    if(ImGui::Button("コンポーネントを追加")){
        ImGui::OpenPopup("AddComponentPopup");
    }

    if(ImGui::BeginPopup("AddComponentPopup")){
        // コンポーネントの追加
        if(ImGui::Button("ModelRenderComponent / モデル描画")){
            AddComponent<ModelRenderComponent>();
            ImGui::CloseCurrentPopup();
        }
        if(ImGui::Button("CollisionComponent / 衝突判定・押し戻し")){
            AddComponent<CollisionComponent>();
            ImGui::CloseCurrentPopup();
        }
        if(ImGui::Button("TextComponent / テキスト描画")){
            AddComponent<TextComponent>();
            ImGui::CloseCurrentPopup();
        }
        if(ImGui::Button("SpotLightComponent / スポットライト")){
            AddComponent<SpotLightComponent>();
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

#endif // _DEBUG
}


//////////////////////////////////////////////////////////////////////////////
// コンテキストメニューの表示
//////////////////////////////////////////////////////////////////////////////
void GameObject::ContextMenu(){
#ifdef _DEBUG
    if(ImGui::BeginPopup("ComponentContextMenu")){
        // 選択中のコンポーネントがある場合
        if(contextMenuComponent_){
            // コンポーネントの削除
            if(ImGui::MenuItem("コンポーネントを削除")){
                auto it = std::find_if(components_.begin(), components_.end(),
                    [this](const std::unique_ptr<IComponent>& comp){
                    return comp.get() == contextMenuComponent_;
                });
                if(it != components_.end()){
                    IComponent* component = it->get();

                    // 自身の子供がこのコンポーネントを参照している場合子の参照を解除
                    for(auto& child : children_){
                        if(child->parentComponentInfo_.pComponent == component){
                            child->parentComponentInfo_.pComponent = nullptr;
                            child->parentComponentInfo_.pMatrix = nullptr;
                        }
                    }

                    components_.erase(it);
                }
                contextMenuComponent_ = nullptr; // コンテキストメニューオブジェクトをクリア
            }
            ImGui::Separator();
        }
        ImGui::EndPopup();
    }
#endif // _DEBUG
}
