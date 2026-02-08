#include "GameObject.h"
#include <Game/GameSystem.h>
#include <SEED/Source/Basic/Scene/Scene_Base.h>
#include <SEED/Source/Manager/Hierarchy/Hierarchy.h>
#include <SEED/Source/Basic/Components/ComponentRegister.h>


namespace SEED{
    //////////////////////////////////////////////////////////////////////////
    // static変数
    //////////////////////////////////////////////////////////////////////////
    uint32_t GameObject::nextID_ = 0;



    //////////////////////////////////////////////////////////////////////////
    // コンストラクタ ・ デストラクタ
    //////////////////////////////////////////////////////////////////////////
    GameObject::GameObject(Scene_Base* pScene){
        objectID_ = nextID_++;
        objectName_ = "Object_" + std::to_string(objectID_);
        Initialize();
        // シーンのヒエラルキーに登録
        pScene->RegisterToHierarchy(this);
        UpdateMatrix();
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

        // 生成されてからの時間を更新
        aliveTime_ += ClockManager::DeltaTime();
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
    void GameObject::UpdateMatrix(bool isUpdateChildren){

        // ローカル変換行列の更新
        localMat_ = Methods::Matrix::AffineMatrix(
            localTransform_.scale * aditionalTransform_.scale,
            aditionalTransform_.rotate * localTransform_.rotate,
            localTransform_.translate + aditionalTransform_.translate
        );

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

                Matrix4x4 cancelMat = Methods::Matrix::IdentityMat4();

                // 親の行列から取り出した要素を打ち消す行列を作成
                if(!isParentScale_){
                    Vector3 inverseScale = Vector3(1.0f, 1.0f, 1.0f) / Methods::Matrix::ExtractScale(parentMat);
                    cancelMat = cancelMat * Methods::Matrix::ScaleMatrix(inverseScale);
                }

                if(!isParentRotate_){
                    Vector3 inverseRotate = Methods::Matrix::ExtractRotation(parentMat) * -1.0f;
                    cancelMat = cancelMat * Methods::Matrix::RotateMatrix(inverseRotate);
                }

                if(!isParentTranslate_){
                    Vector3 inverseTranslate = Methods::Matrix::ExtractTranslation(parentMat) * -1.0f;
                    cancelMat = cancelMat * Methods::Matrix::TranslateMatrix(inverseTranslate);
                }

                Matrix4x4 canceledMat = cancelMat * parentMat;
                worldMat_ = (localMat_ * parentMat) * cancelMat;
                //worldMat_ = localMat_ * canceledMat;
            }
        } else{
            worldMat_ = localMat_;
        }

        // ワールド変換情報の更新
        worldTransform_.translate = Methods::Matrix::ExtractTranslation(worldMat_);
        worldTransform_.rotate = Quaternion::ToQuaternion(Methods::Matrix::ExtractRotation(worldMat_));
        worldTransform_.scale = Methods::Matrix::ExtractScale(worldMat_);

        // フラグが立っていれば子オブジェクトのマトリックスも更新
        if(isUpdateChildren){
            for(auto* child : children_){
                child->UpdateMatrix(true);
            }
        }
    }


    //////////////////////////////////////////////////////////////////////////
    // アクティブ設定
    //////////////////////////////////////////////////////////////////////////
    void GameObject::SetIsActive(bool isActive, bool isUpdateChildren){
        isActive_ = isActive;
        if(isUpdateChildren){
            for(auto* child : children_){
                child->SetIsActive(isActive, true);
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////
    // 親子付け関連
    //////////////////////////////////////////////////////////////////////////

    void GameObject::SetParent(GameObject* parent){
        ReleaseParent();
        parent_ = parent;
        parent->children_.push_back(this);
    }

    std::list<GameObject*> GameObject::GetAllChildren() const{
        // 全ての子孫を取得する
        std::list<GameObject*> allChildren;
        for(const auto& child : children_){
            allChildren.push_back(child);
            auto descendants = child->GetAllChildren();
            allChildren.insert(allChildren.end(), descendants.begin(), descendants.end());
        }
        return allChildren;
    }

    // 名前の一致する子を取得
    GameObject* GameObject::GetChild(const std::string& name){
        if(children_.size() == 0){
            return nullptr;
        }

        for(auto& child : children_){
            if(child->GetName() == name){
                return &(*child);
            }
        }

        return nullptr;
    }

    // インデックスで子を取得
    GameObject* GameObject::GetChild(uint32_t index){
        if(children_.size() == 0 || index >= children_.size()){
            return nullptr;
        }
        auto it = children_.begin();
        std::advance(it, index);
        return *it;
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
    // スケールの設定
    //////////////////////////////////////////////////////////////////////////

    void GameObject::SetWorldScale(const Vector3& scale){
        // 親のスケールを考慮してワールド軸基準でスケールを設定
        if(parent_ != nullptr){
            Matrix4x4 invParentMat = Methods::Matrix::InverseMatrix(Methods::Matrix::RotateMatrix(parent_->GetWorldRotate()));
            localTransform_.scale = scale * invParentMat;
        } else{
            localTransform_.scale = scale;
        }
    }

    void GameObject::SetLocalScale(const Vector3& scale){
        localTransform_.scale = scale;
    }

    //////////////////////////////////////////////////////////////////////////
    // 回転の加算
    //////////////////////////////////////////////////////////////////////////
    void GameObject::AddWorldRotate(const Quaternion& addValue){
        if(parent_ != nullptr){
            // 親のワールド回転を取得
            Quaternion parentWorldRot = parent_->GetWorldRotate();
            // 親の回転を逆変換してローカル空間に変換
            Quaternion localAddValue = Quaternion::Inverse(parentWorldRot) * addValue * parentWorldRot;
            // ローカル回転に加算（合成）
            localTransform_.rotate = localAddValue * localTransform_.rotate;
        } else{
            // 親がいない場合はワールド＝ローカルなのでそのまま
            localTransform_.rotate = addValue * localTransform_.rotate;
        }
    }

    void GameObject::AddWorldRotate(const Vector3& addValue){
        AddWorldRotate(Quaternion::ToQuaternion(addValue));
    }

    //////////////////////////////////////////////////////////////////////////
    // Rotateの設定
    //////////////////////////////////////////////////////////////////////////
    void GameObject::SetWorldRotate(const Quaternion& rotate){
        // 親の回転を考慮してワールド軸基準で回転を設定
        if(parent_ != nullptr){
            Matrix4x4 invParentMat = Methods::Matrix::InverseMatrix(Methods::Matrix::RotateMatrix(parent_->GetWorldRotate()));
            localTransform_.rotate = rotate * Quaternion::MatrixToQuaternion(invParentMat);
        } else{
            localTransform_.rotate = rotate;
        }
    }

    void GameObject::SetLocalRotate(const Quaternion& rotate){
        localTransform_.rotate = rotate;
    }

    //////////////////////////////////////////////////////////////////////////
    // 親子付けとかされてても常にワールド軸基準で指定した方向に移動を追加する関数
    //////////////////////////////////////////////////////////////////////////
    void GameObject::AddWorldTranslate(const Vector3& addValue){
        if(parent_ != nullptr){
            Matrix4x4 invParentMat = Methods::Matrix::InverseMatrix(Methods::Matrix::RotateMatrix(parent_->GetWorldRotate()));
            Vector3 localAddValue = addValue * invParentMat;
            localTransform_.translate += localAddValue;
        } else{
            localTransform_.translate += addValue;
        }
    }

    void GameObject::SetWorldTranslate(const Vector3& translate){
        // 親の位置を考慮してワールド軸基準で位置を設定
        if(parent_ != nullptr){
            Matrix4x4 invParentMat = Methods::Matrix::InverseMatrix(Methods::Matrix::RotateMatrix(parent_->GetWorldRotate()));
            localTransform_.translate = translate * invParentMat;
        } else{
            localTransform_.translate = translate;
        }
    }

    void GameObject::SetLocalTranslate(const Vector3& translate){
        localTransform_.translate = translate;
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

        // コンポーネントの読み込み
        if(jsonData.contains("components")){
            for(const auto& componentJson : jsonData["components"]){
                ComponentRegister::LoadComponents(this, componentJson);
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
        std::string tag = "##" + std::to_string(objectID_);
        ImFunc::InputText("オブジェクト名" + tag, objectName_);

        ImGui::Text("------------- トランスフォーム -------------");
        static Vector3 eulerAngle;
        ImGui::DragFloat3("位置" + tag, &localTransform_.translate.x, 0.1f);
        ImGui::DragFloat3("スケール" + tag, &localTransform_.scale.x, 0.05f);
        ImGui::DragFloat3("オイラー回転" + tag, &eulerAngle.x, 0.05f);
        if(ImGui::Button("オイラー角からQuaternion回転に適用" + tag)){
            localTransform_.rotate = Quaternion::ToQuaternion(eulerAngle);
        }

        ImGui::Text("--------------- ペアレント方式 ---------------");
        ImGui::Checkbox("回転をペアレントする" + tag, &isParentRotate_);
        ImGui::Checkbox("スケールをペアレントする" + tag, &isParentScale_);
        ImGui::Checkbox("位置をペアレントする" + tag, &isParentTranslate_);

        ImGui::Text("------------- コンポーネント一覧 -------------");

        // コンポーネントのGUI編集
        for(auto& component : components_){
            // ラベルの設定
            bool opened = ImFunc::CollapsingHeader(
                component->componentTag_ + "##" + std::to_string(component->componentID_),
                EditorColor::componentHeader
            );

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
            // 追加GUI
            if(ComponentRegister::RegisterGUI(this)){
                // 追加したコンポーネントを初期化し閉じる
                components_.back()->Initialize();
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
}