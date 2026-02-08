#include "Collision3DComponent.h"

namespace SEED{
    //////////////////////////////////////////////////////////////////////////
    // コンストラクタ
    //////////////////////////////////////////////////////////////////////////
    Collision3DComponent::Collision3DComponent(GameObject* pOwner, const std::string& tagName)
        : IComponent(pOwner, tagName){

        if(tagName == ""){
            componentTag_ = "Collision3D_ID:" + std::to_string(componentID_);
        }

    #ifdef _DEBUG
        // コライダーエディターの初期
        colliderEditor_ = std::make_unique<ColliderEditor>(owner_.owner3D->GetName(), owner_.owner3D);
    #endif // _DEBUG
    }

    //////////////////////////////////////////////////////////////////////////
    // 初期化
    //////////////////////////////////////////////////////////////////////////
    void Collision3DComponent::Initialize(const std::string& fileName, ObjectType objectType){
        // コライダーの初期化
        InitColliders(fileName, objectType);
    }


    //////////////////////////////////////////////////////////////////////////
    // フレーム開始時の処理
    //////////////////////////////////////////////////////////////////////////
    void Collision3DComponent::BeginFrame(){
        // コライダーの開始処理
        for(auto& collider : colliders_){
            collider->BeginFrame();
        }

        // アクティブな場合のみコライダーを渡す
        if(isActive_){
            HandOverColliders();

        #ifdef _DEBUG
            colliderEditor_->HandOverColliders();
        #endif // _DEBUG
        }
    }

    //////////////////////////////////////////////////////////////////////////
    // 更新処理
    //////////////////////////////////////////////////////////////////////////
    void Collision3DComponent::Update(){
        // コライダーの更新
        for(auto& collider : colliders_){
            collider->Update();
        }
    }

    //////////////////////////////////////////////////////////////////////////
    // 描画処理
    //////////////////////////////////////////////////////////////////////////
    void Collision3DComponent::Draw(){
        // コライダーの描画
        if(isColliderVisible_){
            for(auto& collider : colliders_){
                collider->Draw();
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////
    // フレーム終了時の処理
    //////////////////////////////////////////////////////////////////////////
    void Collision3DComponent::EndFrame(){
        // コライダーの更新
        EraseCheckColliders();
        // コライダーをCollisionManagerに渡す
        HandOverColliders();
    }


    //////////////////////////////////////////////////////////////////////////
    // 終了処理
    //////////////////////////////////////////////////////////////////////////
    void Collision3DComponent::Finalize(){
    }

    //////////////////////////////////////////////////////////////////////////
    // GUI編集
    //////////////////////////////////////////////////////////////////////////
    void Collision3DComponent::EditGUI(){
    #ifdef _DEBUG
        ImGui::Indent();

        // コライダーエディターのGUI編集
        colliderEditor_->Edit();

        ImGui::Unindent();
    #endif
    }


    //////////////////////////////////////////////////////////////////////////
    // コライダーの追加
    //////////////////////////////////////////////////////////////////////////
    void Collision3DComponent::AddCollider(Collider* collider){
        colliders_.emplace_back(std::make_unique<Collider>());
        colliders_.back().reset(collider);
    }


    //////////////////////////////////////////////////////////////////////////
    // コライダーの読み込み
    //////////////////////////////////////////////////////////////////////////
    void Collision3DComponent::LoadColliders(nlohmann::json json, ObjectType objectType){
        // コライダーの読み込み
        auto colliders = ColliderEditor::LoadColliderData(json);

        for(auto& collider : colliders){

            // コライダーの所有者を設定
            collider->SetOwnerObject(owner_.owner3D);
            collider->SetParentMatrix(owner_.owner3D->GetWorldMatPtr());
            // コライダーを追加
            colliders_.emplace_back(std::move(collider));
        }

        // オブジェクトの属性を取得
        for(auto& collider : colliders_){
            collider->SetObjectType(objectType);
        }
    }



    //////////////////////////////////////////////////////////////////////////
    // コライダーの初期化
    //////////////////////////////////////////////////////////////////////////
    void Collision3DComponent::InitColliders(nlohmann::json json, ObjectType objectType){
        colliders_.clear();
        LoadColliders(json, objectType);
    }



    //////////////////////////////////////////////////////////////////////////
    // コライダーの削除チェック
    //////////////////////////////////////////////////////////////////////////
    void Collision3DComponent::EraseCheckColliders(){

        // 終了した要素の削除
        for(int i = 0; i < colliders_.size(); ++i){
            if(colliders_[i]->IsEndAnimation()){
                colliders_.erase(colliders_.begin() + i);
                --i;
            }
        }
    }


    //////////////////////////////////////////////////////////////////////////
    // コライダーのリセット
    //////////////////////////////////////////////////////////////////////////
    void Collision3DComponent::ResetCollider(){
        colliders_.clear();
    }


    //////////////////////////////////////////////////////////////////////////
    // コライダーをCollisionManagerに渡す
    //////////////////////////////////////////////////////////////////////////
    void Collision3DComponent::HandOverColliders(){

        // 衝突情報の保存・初期化
        preIsCollide_ = isCollide_;
        isCollide_ = false;

        if(!isActive_){
            return;
        }
        // キャラクターの基本コライダーを渡す
        for(auto& collider : colliders_){
            CollisionManager::AddCollider(collider.get());
        }
    }

    //////////////////////////////////////////////////////////////////////////
    // コライダーの判定スキップリストの追加
    //////////////////////////////////////////////////////////////////////////
    void Collision3DComponent::AddSkipPushBackType(ObjectType skipType){
        for(auto& collider : colliders_){
            collider->AddSkipPushBackType(skipType);
        }
    }


    //////////////////////////////////////////////////////////////////////////
    // Jsonデータの読み込み
    //////////////////////////////////////////////////////////////////////////
    void Collision3DComponent::LoadFromJson(const nlohmann::json& jsonData){

        IComponent::LoadFromJson(jsonData);
        if(jsonData.contains("objectType")){
            objectType_ = static_cast<ObjectType>(jsonData["objectType"].get<uint32_t>());
        }

        if(jsonData.contains("colliders")){
            LoadColliders(jsonData, objectType_);
        }
    }


    //////////////////////////////////////////////////////////////////////////
    // Jsonデータの出力
    //////////////////////////////////////////////////////////////////////////
    nlohmann::json Collision3DComponent::GetJsonData() const{
        nlohmann::ordered_json jsonData;
        jsonData["componentType"] = "Collision3D";
        jsonData.update(IComponent::GetJsonData());
        jsonData["objectType"] = static_cast<uint32_t>(objectType_);

        // コライダーのデータを出力
        for(const auto& collider : colliders_){
            jsonData["colliders"].push_back(collider->GetJsonData());
        }

        for(const auto& collider : colliderEditor_->colliders_){
            jsonData["colliders"].push_back(collider->GetJsonData());
        }

        return jsonData;
    }
}