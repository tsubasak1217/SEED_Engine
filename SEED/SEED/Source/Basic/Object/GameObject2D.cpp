#include "GameObject2D.h"
#include <Game/GameSystem.h>
#include <SEED/Source/Basic/Scene/Scene_Base.h>
#include <SEED/Source/Manager/Hierarchy/Hierarchy.h>


//////////////////////////////////////////////////////////////////////////
// static変数
//////////////////////////////////////////////////////////////////////////
uint32_t GameObject2D::nextID_ = 0;



//////////////////////////////////////////////////////////////////////////
// コンストラクタ ・ デストラクタ
//////////////////////////////////////////////////////////////////////////
GameObject2D::GameObject2D(Scene_Base* pScene){
    objectID_ = nextID_++;
    objectName_ = "Object2D_" + std::to_string(objectID_);
    Initialize();
    // シーンのヒエラルキーに登録
    pScene->RegisterToHierarchy(this);
    UpdateMatrix();
}

GameObject2D::~GameObject2D(){
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
void GameObject2D::Initialize(){
}



//////////////////////////////////////////////////////////////////////////
// 更新処理
//////////////////////////////////////////////////////////////////////////
void GameObject2D::Update(){

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
void GameObject2D::Draw(){
    // コンポーネントの描画
    for(auto& component : components_){
        if(!component->isActive_){ continue; }
        component->Draw();
    }
}

//////////////////////////////////////////////////////////////////////////
// フレーム開始時処理
//////////////////////////////////////////////////////////////////////////
void GameObject2D::BeginFrame(){

    // 座標の保存
    prePos_ = GetWorldTranslate();

    // 当たり判定フラグのリセットと保存
    preIsCollideAny_ = isCollideAny_;
    preIsCollideGhost_ = isCollideGhost_;
    preIsCollideSolid_ = isCollideSolid_;
    isCollideAny_ = false;
    isCollideGhost_ = false;
    isCollideSolid_ = false;

    // 接地フラグのリセット
    preIsOnGround_ = isOnGround_;
    isOnGround_ = false;

    //　天井に当たっているかのフラグのリセット
    preIsCeiling_ = isCeiling_;
    isCeiling_ = false;

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
void GameObject2D::EndFrame(){

    // コンポーネントの終了処理
    for(auto& component : components_){
        if(!component->isActive_){ continue; }
        component->EndFrame();
    }
}

//////////////////////////////////////////////////////////////////////////
// マトリックスの更新
//////////////////////////////////////////////////////////////////////////
void GameObject2D::UpdateMatrix(){

    // ローカル変換行列の更新
    localMat_ = AffineMatrix(localTransform_.scale, localTransform_.rotate, localTransform_.translate);

    // ワールド行列の更新
    if(parent_){

        Matrix3x3 parentMat = parent_->worldMat_;

        if(isParentRotate_ + isParentScale_ + isParentTranslate_ == 3){
            worldMat_ = localMat_ * parentMat;

        } else{

            Matrix3x3 cancelMat = IdentityMat3();

            // 親の行列から取り出した要素を打ち消す行列を作成
            if(!isParentScale_){
                Vector2 inverseScale = Vector2(1.0f, 1.0f) / ExtractScale(parentMat);
                cancelMat = cancelMat * ScaleMatrix(inverseScale);
            }

            if(!isParentRotate_){
                float inverseRotate = ExtractRotation(parentMat) * -1.0f;
                cancelMat = cancelMat * RotateMatrix(inverseRotate);
            }

            if(!isParentTranslate_){
                Vector2 inverseTranslate = ExtractTranslation(parentMat) * -1.0f;
                cancelMat = cancelMat * TranslateMatrix(inverseTranslate);
            }

            Matrix3x3 canceledMat = cancelMat * parentMat;
            worldMat_ = (localMat_ * parentMat) * cancelMat;
            //worldMat_ = localMat_ * canceledMat;
        }
    } else{
        worldMat_ = localMat_;
    }

    worldTransform_.translate = ExtractTranslation(worldMat_);
    worldTransform_.rotate = ExtractRotation(worldMat_);
    worldTransform_.scale = ExtractScale(worldMat_);
}


//////////////////////////////////////////////////////////////////////////
// 親子付け関連
//////////////////////////////////////////////////////////////////////////

void GameObject2D::SetParent(GameObject2D* parent){
    ReleaseParent();
    parent_ = parent;
    parent->children_.push_back(this);
}

std::list<GameObject2D*> GameObject2D::GetAllChildren() const{
    // 全ての子孫を取得する
    std::list<GameObject2D*> allChildren;
    for(const auto& child : children_){
        allChildren.push_back(child);
        auto descendants = child->GetAllChildren();
        allChildren.insert(allChildren.end(), descendants.begin(), descendants.end());
    }
    return allChildren;
}

void GameObject2D::RemoveChild(GameObject2D* child){
    if(child){
        child->ReleaseParent();
        children_.remove(child);
    }
}

void GameObject2D::ReleaseParent(){
    if(parent_){
        parent_->children_.remove(this);
        parent_ = nullptr;
    }
}

void GameObject2D::ReleaseChildren(){
    auto childrenCopy = children_;
    for(auto* child : childrenCopy){
        child->ReleaseParent();
    }
    children_.clear();
}

// 自分の子孫かどうか確認する関数
bool GameObject2D::IsDescendant(GameObject2D* obj) const{
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
// スケールの設定
//////////////////////////////////////////////////////////////////////////

void GameObject2D::SetWorldScale(const Vector2& scale){
    // 親のスケールを考慮してワールド軸基準でスケールを設定
    if(parent_ != nullptr){
        Matrix3x3 invParentMat = InverseMatrix(RotateMatrix(parent_->GetWorldRotate()));
        localTransform_.scale = scale * invParentMat;
    } else{
        localTransform_.scale = scale;
    }
}

void GameObject2D::SetLocalScale(const Vector2& scale){
    localTransform_.scale = scale;
}

//////////////////////////////////////////////////////////////////////////
// Rotateの設定
//////////////////////////////////////////////////////////////////////////
void GameObject2D::SetWorldRotate(float rotate){
    // 親の回転を考慮してワールド軸基準で回転を設定
    if(parent_ != nullptr){
        Matrix3x3 invParentMat = InverseMatrix(RotateMatrix(parent_->GetWorldRotate()));
        localTransform_.rotate = ExtractRotation(RotateMatrix(rotate) * invParentMat);
    } else{
        localTransform_.rotate = rotate;
    }
}

void GameObject2D::SetLocalRotate(float rotate){
    localTransform_.rotate = rotate;
}

//////////////////////////////////////////////////////////////////////////
// 親子付けとかされてても常にワールド軸基準で指定した方向に移動を追加する関数
//////////////////////////////////////////////////////////////////////////
void GameObject2D::AddWorldTranslate(const Vector2& addValue){
    if(parent_ != nullptr){
        Matrix3x3 invParentMat = InverseMatrix(RotateMatrix(parent_->GetWorldRotate()));
        Vector2 localAddValue = addValue * invParentMat;
        localTransform_.translate += localAddValue;
    } else{
        localTransform_.translate += addValue;
    }
}

void GameObject2D::SetWorldTranslate(const Vector2& translate){
    // 親の位置を考慮してワールド軸基準で位置を設定
    if(parent_ != nullptr){
        Matrix3x3 invParentMat = InverseMatrix(RotateMatrix(parent_->GetWorldRotate()));
        localTransform_.translate = translate * invParentMat;
    } else{
        localTransform_.translate = translate;
    }
}

void GameObject2D::SetLocalTranslate(const Vector2& translate){
    localTransform_.translate = translate;
}




//////////////////////////////////////////////////////////////////////////
// 衝突処理
//////////////////////////////////////////////////////////////////////////  

void GameObject2D::OnCollision(GameObject2D* other, Collider2D* collider){

    if(other == nullptr){ return; }
    // 衝突フラグを立てる
    isCollideAny_ = true;
    if(collider->isGhost_){
        isCollideGhost_ = true;
    } else{
        isCollideSolid_ = true;
    }

    // 衝突中の処理
    if(isCollideAny_){
        OnCollisionStay(other);
    }

    // 衝突開始時の処理
    if(isCollideAny_ && !preIsCollideAny_){
        OnCollisionEnter(other);
    }

    // 衝突したオブジェクトを保存
    preCollideObjects_.insert({ other->GetObjectID(),other});
}

void GameObject2D::CheckCollisionExit(){
    // 衝突終了時の処理
    if(!isCollideAny_ && preIsCollideAny_){
        for(auto& other : preCollideObjects_){
            if(GameSystem::GetScene()->IsExistObject2D(other.first)){
                OnCollisionExit(other.second);
            }
        }
        preCollideObjects_.clear();
    }
}

void GameObject2D::OnCollisionEnter(GameObject2D* other){
    // コンポーネントに衝突イベントを通知
    for(auto& component : components_){
        component->OnCollisionEnter(other);
    }
}

void GameObject2D::OnCollisionStay(GameObject2D* other){
    // コンポーネントに衝突イベントを通知
    for(auto& component : components_){
        component->OnCollisionStay(other);
    }
}

void GameObject2D::OnCollisionExit(GameObject2D* other){
    // コンポーネントに衝突イベントを通知
    for(auto& component : components_){
        component->OnCollisionExit(other);
    }
}


//////////////////////////////////////////////////////////////////////////
// jsonデータの取得
//////////////////////////////////////////////////////////////////////////
nlohmann::json GameObject2D::GetJsonData(int32_t depth){
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
void GameObject2D::LoadFromJson(const nlohmann::json& jsonData){

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
        if(componentType == "Collision2D"){
            auto* collisionComponent = AddComponent<Collision2DComponent>();
            collisionComponent->LoadFromJson(componentJson);

        } else if(componentType == "Text"){
            auto* textComponent = AddComponent<TextComponent>();
            textComponent->LoadFromJson(componentJson);

        } else if(componentType == "Jump"){
            auto* jumpComponent = AddComponent<JumpComponent>();
            jumpComponent->LoadFromJson(componentJson);
        
        } else if(componentType == "UI"){
            auto* uiComponent = AddComponent<UIComponent>();
            uiComponent->LoadFromJson(componentJson);

        } else{
            assert(0 && "不明なコンポーネントタイプです");
        }
    }
}

////////////////////////////////////////////////////////////////////////////
// 新しいGameObject2Dをjsonデータから作成する関数
////////////////////////////////////////////////////////////////////////////
GameObject2D* GameObject2D::CreateFromJson(const nlohmann::json& jsonData){
    // 新しいGameObject2Dを作成
    GameObject2D* newObject = new GameObject2D(GameSystem::GetScene());
    // jsonデータから情報を読み込む
    newObject->LoadFromJson(jsonData);
    return newObject;
}

//////////////////////////////////////////////////////////////////////////////
// 家族を再帰的に作成する関数
//////////////////////////////////////////////////////////////////////////////
std::vector<GameObject2D*> GameObject2D::CreateFamily(const nlohmann::json& jsonData, GameObject2D* parent){
    static std::vector<GameObject2D*> familyObjects;
    if(!parent){
        familyObjects.clear(); // 最初のみ初期化
    }

    // 自分自身を作成し、追加
    GameObject2D* self = CreateFromJson(jsonData);
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
void GameObject2D::EditGUI(){
#ifdef _DEBUG

    // オブジェクトの名前を編集
    std::string label = "オブジェクト名##" + std::to_string(objectID_);
    ImFunc::InputText(label.c_str(), objectName_);

    ImGui::Text("------------- トランスフォーム -------------");
    ImGui::DragFloat2("位置", &localTransform_.translate.x);
    ImGui::DragFloat("回転", &localTransform_.rotate, 0.05f);
    ImGui::DragFloat2("スケール", &localTransform_.scale.x, 0.05f);

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
        ImGui::Indent();
        // コンポーネントの追加
        if(ImGui::Button("CollisionComponent / 衝突判定・押し戻し")){
            AddComponent<Collision2DComponent>();
            ImGui::CloseCurrentPopup();
        }
        if(ImGui::Button("TextComponent / テキスト描画")){
            AddComponent<TextComponent>();
            ImGui::CloseCurrentPopup();
        }
        if(ImGui::Button("JumpComponent / ジャンプ")){
            AddComponent<JumpComponent>();
            ImGui::CloseCurrentPopup();
        }
        if(ImGui::Button("UIComponent / UI描画")){
            AddComponent<UIComponent>();
            ImGui::CloseCurrentPopup();
        }

        ImGui::Unindent();
        ImGui::EndPopup();
    }

#endif // _DEBUG
}


//////////////////////////////////////////////////////////////////////////////
// コンテキストメニューの表示
//////////////////////////////////////////////////////////////////////////////
void GameObject2D::ContextMenu(){
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
