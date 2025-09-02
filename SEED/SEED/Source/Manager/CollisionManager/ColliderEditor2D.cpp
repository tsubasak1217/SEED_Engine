#include "ColliderEditor2D.h"
#include <SEED/Source/Basic/Object/GameObject.h>
#include <SEED/Source/Basic/Object/GameObject2D.h>
#include <SEED/Source/Manager/CollisionManager/CollisionManager.h>

////////////////////////////////////////////////////////////
// コンストラクタ ・ デストラクタ
////////////////////////////////////////////////////////////

ColliderEditor2D::ColliderEditor2D(const std::string& className, GameObject2D* parent){
    className_ = className;
    parentObject_ = parent;
    if(parent){
        parentMat_ = parent->GetWorldMatPtr();
    }

    // ファイルの一覧を取得
    colliderFileNames_.clear();
    // ファイルの一覧を取得
    std::string path = "Resources/jsons/Colliders/2D";
    for(const auto& entry : std::filesystem::directory_iterator(path)){
        colliderFileNames_.push_back(entry.path().filename().string());
    }
}

ColliderEditor2D::~ColliderEditor2D(){}

////////////////////////////////////////////////////////////
// コライダーの編集
////////////////////////////////////////////////////////////
void ColliderEditor2D::Edit(){

#ifdef _DEBUG

    // フレームの開始
    for(auto& collider : colliders_){
        collider->BeginFrame();
    }

    // コライダーの追加
    AddColliderOnGUI();

    // jsonファイルからの読み込み
    InputOnGUI();

    // jsonファイルへの出力
    OutputOnGUI();

    ImGui::Separator();
    ImGui::Dummy({ 0.0f,10.0f });


    // 各コライダーの編集
    for(int i = 0; i < colliders_.size(); i++){
        // ヘッダー名
        std::string colliderID = "##" + std::to_string(colliders_[i]->GetColliderID());
        std::string colliderName = "Collider( " + std::to_string(i) + " )" + colliderID;

        // ヘッダーの表示
        if(ImGui::CollapsingHeader(colliderName.c_str())){
            ImGui::Indent();

            // コライダーの削除
            if(DeleteColliderOnGUI(i)){
                break;// 削除されたらループを抜ける
            }

            // コライダーの編集
            colliders_[i]->Edit();

            ImGui::Unindent();
            ImGui::Separator();
        }
    }
    // コライダーを渡す
    HandOverColliders();

#endif // _DEBUG
}


////////////////////////////////////////////////////////////
// コライダーの追加関連のGUI
////////////////////////////////////////////////////////////
void ColliderEditor2D::AddColliderOnGUI(){
#ifdef _DEBUG
    // 追加するコライダーの形状を選択
    ImGui::Combo(
        "ColliderType", (int*)&addColliderType_,
        "Sphere\0AABB\0OBB\0Line\0Capsule\0Plane\0"
    );

    if(ImGui::Button("Add Collider")){
        switch(addColliderType_){
        case ColliderType2D::Circle:
        {
            //colliders_.push_back(std::make_unique<Collider_Sphere>());
            //Collider_Sphere* sphere = dynamic_cast<Collider_Sphere*>(colliders_.back().get());
            //sphere->SetRadius(1.0f);
            break;
        }
        case ColliderType2D::Quad:
        {
            //colliders_.push_back(std::make_unique<Collider_AABB>());
            //Collider_AABB* aabb = dynamic_cast<Collider_AABB*>(colliders_.back().get());
            //aabb->SetSize(Vector3(1.0f, 1.0f, 1.0f));
            break;
        }
        default:
            break;
        }

        // 行列に親子付け
        colliders_.back()->SetOwnerObject(parentObject_);
        colliders_.back()->SetParentMatrix(parentMat_);
        colliders_.back()->objectType_ = ObjectType::Editor;
        colliders_.back()->isEdit_ = true;
    }

#endif // _DEBUG
}


////////////////////////////////////////////////////////////
// コライダーの削除関連のGUI
////////////////////////////////////////////////////////////
bool ColliderEditor2D::DeleteColliderOnGUI(uint32_t index){
    // コライダーの削除
    if(ImGui::Button("Delete Collider")){
        ImGui::OpenPopup("Delete Collider");
    }

    // 削除の確認
    if(ImGui::BeginPopupModal("Delete Collider", NULL, ImGuiWindowFlags_AlwaysAutoResize)){
        ImGui::Text("Are you sure you want to delete this collider?");
        ImGui::Separator();

        // 削除ボタン
        if(ImGui::Button("OK", ImVec2(120, 0))){
            colliders_.erase(colliders_.begin() + index);
            ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
            return true;
        }
        ImGui::SameLine();
        // キャンセルボタン
        if(ImGui::Button("Cancel", ImVec2(120, 0))){
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    return false;
}


////////////////////////////////////////////////////////////
// Jsonファイルへの出力関連のGUI
////////////////////////////////////////////////////////////

void ColliderEditor2D::OutputOnGUI(){
    if(ImGui::Button("Output to Json")){
        OutputToJson();
        // ウィンドウで成功ログを表示
        ImGui::OpenPopup("Output to Json");
    }

    // 成功ログの表示
    if(ImGui::BeginPopupModal("Output to Json", NULL, ImGuiWindowFlags_AlwaysAutoResize)){
        std::string log = "[" + className_ + ".json] is output to Json.";
        ImGui::Text(log.c_str());
        if(ImGui::Button("OK", ImVec2(120, 0))){
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}


////////////////////////////////////////////////////////////
// Jsonファイルからの読み込み関連のGUI
////////////////////////////////////////////////////////////
void ColliderEditor2D::InputOnGUI(){

    // ファイルの一覧を表示
    if(ImGui::Button("Load Collider")){
        ImGui::OpenPopup("FileList");
    }

    // ファイルの一覧をポップアップで表示
    if(ImGui::BeginPopup("FileList")){
        int i = 0;
        for(i = 0; i < colliderFileNames_.size(); i++){
            if(ImGui::Selectable(colliderFileNames_[i].c_str(), selectedColliderIndex_ == i, ImGuiSelectableFlags_DontClosePopups)){
                selectedColliderIndex_ = i;
            }
        }

        if(colliderFileNames_.size()){

            ImGui::Text("Selected File : %s", colliderFileNames_[selectedColliderIndex_].c_str());

            // OKボタン
            if(ImGui::Button("OK", ImVec2(120, 0))){
                LoadFromJson(colliderFileNames_[selectedColliderIndex_]);
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            // キャンセルボタン
            if(ImGui::Button("Cancel", ImVec2(120, 0))){
                ImGui::CloseCurrentPopup();
            }

        } else{
            ImGui::Text("No files.");
            // OKボタン
            if(ImGui::Button("OK", ImVec2(120, 0))){
                ImGui::CloseCurrentPopup();
            }
        }

        ImGui::EndPopup();
    }

}


////////////////////////////////////////////////////////////
// Jsonファイルへの出力
////////////////////////////////////////////////////////////
void ColliderEditor2D::OutputToJson(){

    nlohmann::json j;

    // コライダーの数
    for(int i = 0; i < colliders_.size(); i++){
        j["colliders"][i] = colliders_[i]->GetJsonData();
    }

    // ファイルへの書き込み
    std::ofstream ofs("Resources/jsons/Colliders/2D/" + className_ + ".json");
    ofs << j.dump(4);
    ofs.close();
}

////////////////////////////////////////////////////////////
// Jsonファイルからの読み込み
////////////////////////////////////////////////////////////

void ColliderEditor2D::LoadColliders(const std::string& fileName, GameObject2D* parentObject, std::vector<std::unique_ptr<Collider2D>>* pColliderArray){
    // 無ければ読み込む
    if(colliderData_.find(fileName) == colliderData_.end()){
        LoadColliderData(fileName);
    }

    for(auto& collider : colliderData_[fileName]){

        //int colliderID = 0;

        // コライダーの形状に応じて生成
        switch(collider->GetColliderType()){
        case ColliderType2D::Circle:
        {
            //pColliderArray->push_back(std::make_unique<Collider_Sphere>());
            //Collider_Sphere* sphere = dynamic_cast<Collider_Sphere*>(pColliderArray->back().get());
            //colliderID = sphere->GetColliderID();
            //*sphere = *dynamic_cast<Collider_Sphere*>(collider.get());
            //sphere->SetColliderID(colliderID);
            break;
        }
        case ColliderType2D::Quad:
        {
            //pColliderArray->push_back(std::make_unique<Collider_AABB>());
            //Collider_AABB* aabb = dynamic_cast<Collider_AABB*>(pColliderArray->back().get());
            //colliderID = aabb->GetColliderID();
            //*aabb = *dynamic_cast<Collider_AABB*>(collider.get());
            //aabb->SetColliderID(colliderID);
            break;
        }
        }


    }

    for(auto& collider : *pColliderArray){
        // 親子付け
        collider->SetParentMatrix(parentObject->GetWorldMatPtr());
        collider->SetOwnerObject(parentObject);
    }
}

// エディター用の読み込み関数
void ColliderEditor2D::LoadFromJson(const std::string& fileName){
    // ファイルの読み込み
    std::ifstream ifs("Resources/jsons/Colliders/2D/" + fileName);
    if(ifs.fail()){
        return;
    }

    // ファイルの解析
    nlohmann::json j;
    ifs >> j;

    // コライダーの数
    for(int i = 0; i < j["colliders"].size(); i++){
        // コライダーの形状
        std::string colliderType = j["colliders"][i]["colliderType"];
        if(colliderType == "Circle"){
            //colliders_.push_back(std::make_unique<Collider2D_Circle>());
        } else if(colliderType == "Quad"){
            //colliders_.push_back(std::make_unique<Collider2D_Quad>());
        }

        // コライダーの読み込み
        colliders_.back()->LoadFromJson(j["colliders"][i]);
        colliders_.back()->SetParentMatrix(parentMat_);
    }

    ifs.close();
}


// 外部から呼び出せる読み込み関数
void ColliderEditor2D::LoadColliderData(const std::string fileName){

    // すでに読み込んでいたらリターン
    if(colliderData_.find(fileName) != colliderData_.end()){
        return;
    }

    std::vector<Collider2D*> colliders;

    // ファイルの読み込み
    std::ifstream ifs("Resources/jsons/Colliders/2D/" + fileName);
    if(ifs.fail()){
        return;
    }

    // ファイルの解析
    nlohmann::json j;
    ifs >> j;

    // コライダーの数
    for(int i = 0; i < j["colliders"].size(); i++){
        // コライダーの形状
        std::string colliderType = j["colliders"][i]["colliderType"];
        if(colliderType == "Circle"){
            //colliders.push_back(new Collider2D_Circle());
        } else if(colliderType == "Quad"){
            //colliders.push_back(new Collider2D_Quad());
        }

        // コライダーの読み込み
        colliders.back()->LoadFromJson(j["colliders"][i]);

        // コライダー情報を追加
        colliderData_[fileName].emplace_back(std::move(colliders.back()));
    }

    ifs.close();
}


std::vector<Collider2D*> ColliderEditor2D::LoadColliderData(const nlohmann::json& json){

    std::vector<Collider2D*> colliders;

    // コライダーの数
    for(int i = 0; i < json["colliders"].size(); i++){
        // コライダーの形状
        std::string colliderType = json["colliders"][i]["colliderType"];
        if(colliderType == "Circle"){
            //colliders.push_back(new Collider2D_Circle());
        } else if(colliderType == "Quad"){
            //colliders.push_back(new Collider2D_Quad());
        }

        // コライダーの読み込み
        colliders.back()->LoadFromJson(json["colliders"][i]);
    }

    return colliders;
}


////////////////////////////////////////////////////////////
// コライダーをCollisionManagerに渡す
////////////////////////////////////////////////////////////
void ColliderEditor2D::HandOverColliders(){
    // コライダーをCollisionManagerに渡す
    for(int i = 0; i < colliders_.size(); i++){
        //colliders_[i]->Update();
        CollisionManager::AddCollider(colliders_[i].get());
    }
}
