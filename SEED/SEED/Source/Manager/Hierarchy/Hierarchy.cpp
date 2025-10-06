#include "Hierarchy.h"
#include <Game/GameSystem.h>
#include <SEED/Source/Basic/Components/utils/DragInfo_Joint.h>

Hierarchy::Hierarchy(){
    isEndHierarchy_ = false;
}

Hierarchy::~Hierarchy(){
    isEndHierarchy_ = true;
    Finalize();
}

/////////////////////////////////////////////////////////////////////
// ゲームオブジェクトの登録・削除
/////////////////////////////////////////////////////////////////////
void Hierarchy::RegisterGameObject(GameObject* gameObject){
    // ゲームオブジェクトを登録
    gameObjects_.push_back(gameObject);
    existObjectIdMap_.insert(gameObject->GetObjectID());
}

void Hierarchy::RegisterGameObject(GameObject2D* gameObject2D){
    // 2Dゲームオブジェクトを登録
    gameObjects2D_.push_back(gameObject2D);
    existObjectIdMap2D_.insert(gameObject2D->GetObjectID());
}

void Hierarchy::RemoveGameObject(GameObject* gameObject){
    // ゲームオブジェクトを削除
    if(isEndHierarchy_){ return; }
    existObjectIdMap_.erase(gameObject->GetObjectID());
    gameObjects_.remove(gameObject);

    // 親子関係の再構築
    if(gameObject->GetParent()){
        auto childrenCopy = gameObject->children_;
        for(auto* child : childrenCopy){
            // 親を結び直す
            child->ReleaseParent();
            child->SetParent(gameObject->GetParent());
        }
        gameObject->parent_->RemoveChild(gameObject);
    } else{
        // 親がいない場合
        auto childrenCopy = gameObject->children_;
        for(auto* child : childrenCopy){
            child->ReleaseParent();
        }
        gameObject->ReleaseChildren();
    }

    // ヒエラルキーから削除
    if(selectedObject_ == gameObject){
        selectedObject_ = nullptr; // 選択中のオブジェクトをクリア
    }
}

void Hierarchy::RemoveGameObject(GameObject2D* gameObject){
    // ゲームオブジェクトを削除
    if(isEndHierarchy_){ return; }
    existObjectIdMap2D_.erase(gameObject->GetObjectID());
    gameObjects2D_.remove(gameObject);

    // 親子関係の再構築
    if(gameObject->GetParent()){
        auto childrenCopy = gameObject->children_;
        for(auto* child : childrenCopy){
            // 親を結び直す
            child->ReleaseParent();
            child->SetParent(gameObject->GetParent());
        }
        gameObject->parent_->RemoveChild(gameObject);
    } else{
        // 親がいない場合
        auto childrenCopy = gameObject->children_;
        for(auto* child : childrenCopy){
            child->ReleaseParent();
        }
        gameObject->ReleaseChildren();
    }

    // ヒエラルキーから削除
    if(selectedObject2D_ == gameObject){
        selectedObject2D_ = nullptr; // 選択中のオブジェクトをクリア
    }
}

void Hierarchy::EraseObject(GameObject* gameObject){
    if(isEndHierarchy_){ return; }
    auto childrenCopy = gameObject->GetAllChildren();
    existObjectIdMap_.erase(gameObject->GetObjectID());

    for(auto* child : childrenCopy){
        // selfCreatedObjectにあるか確認
        auto it = std::find_if(selfCreateObjects_.begin(), selfCreateObjects_.end(),
            [child](const std::unique_ptr<GameObject>& obj){ return obj.get() == child; });

        // selfCreateObjects_から解放
        if(it != selfCreateObjects_.end()){
            selfCreateObjects_.erase(it); // 自分で生成したオブジェクトのリストから削除

        } else{// 直接解放
            delete child; // 子オブジェクトを削除
            child = nullptr; // ポインタをクリア
        }
    }

    // selfCreatedObjectにあるか確認
    auto it = std::find_if(selfCreateObjects_.begin(), selfCreateObjects_.end(),
        [gameObject](const std::unique_ptr<GameObject>& obj){ return obj.get() == gameObject; });

    // selfCreateObjects_から解放
    if(it != selfCreateObjects_.end()){
        selfCreateObjects_.erase(it); // 自分で生成したオブジェクトのリストから削除

    } else{// 直接解放
        delete gameObject; // 自分自身を削除
        gameObject = nullptr; // ポインタをクリア
    }
}

void Hierarchy::EraseObject(GameObject2D* gameObject){
    if(isEndHierarchy_){ return; }
    auto childrenCopy = gameObject->GetAllChildren();
    existObjectIdMap2D_.erase(gameObject->GetObjectID());

    for(auto* child : childrenCopy){
        // selfCreatedObjectにあるか確認
        auto it = std::find_if(selfCreateObjects2D_.begin(), selfCreateObjects2D_.end(),
            [child](const std::unique_ptr<GameObject2D>& obj){ return obj.get() == child; });

        // selfCreateObjects_から解放
        if(it != selfCreateObjects2D_.end()){
            selfCreateObjects2D_.erase(it); // 自分で生成したオブジェクトのリストから削除

        } else{// 直接解放
            delete child; // 子オブジェクトを削除
            child = nullptr; // ポインタをクリア
        }
    }

    // selfCreatedObjectにあるか確認
    auto it = std::find_if(selfCreateObjects2D_.begin(), selfCreateObjects2D_.end(),
        [gameObject](const std::unique_ptr<GameObject2D>& obj){ return obj.get() == gameObject; });

    // selfCreateObjects_から解放
    if(it != selfCreateObjects2D_.end()){
        selfCreateObjects2D_.erase(it); // 自分で生成したオブジェクトのリストから削除

    } else{// 直接解放
        delete gameObject; // 自分自身を削除
        gameObject = nullptr; // ポインタをクリア
    }
}

///////////////////////////////////////////////////////////////////
// コンストラクタ・デストラクタ
///////////////////////////////////////////////////////////////////
void Hierarchy::BeginFrame(){
    // 親子関係の再構築
    RebuildParentInfo();

    // ゲームオブジェクトのフレーム開始処理
    for(auto& gameObject : gameObjects_){
        if(!gameObject->GetIsActive()){ continue; }
        gameObject->BeginFrame();
    }

    for(auto& gameObject2D : gameObjects2D_){
        if(!gameObject2D->GetIsActive()){ continue; }
        gameObject2D->BeginFrame();
    }
}

///////////////////////////////////////////////////////////////////
// 更新処理
///////////////////////////////////////////////////////////////////
void Hierarchy::Update(){
    // GUIでの編集処理
    EditGUI();

    // ゲームオブジェクトの更新
    for(auto& gameObject : gameObjects_){
        if(!gameObject->GetIsActive()){ continue; }
        gameObject->Update();
    }

    for(auto& gameObject2D : gameObjects2D_){
        if(!gameObject2D->GetIsActive()){ continue; }
        gameObject2D->Update();
    }
}

/////////////////////////////////////////////////////////////////////
// 描画処理
/////////////////////////////////////////////////////////////////////
void Hierarchy::Draw(){
    // ゲームオブジェクトの描画
    for(auto& gameObject : gameObjects_){
        if(!gameObject->GetIsActive() && !gameObject->GetIsMustDraw()){ continue; }
        gameObject->Draw();
    }

    for(auto& gameObject2D : gameObjects2D_){
        if(!gameObject2D->GetIsActive() && !gameObject2D->GetIsMustDraw()){ continue; }
        gameObject2D->Draw();
    }
}

/////////////////////////////////////////////////////////////////////
// フレーム終了処理
/////////////////////////////////////////////////////////////////////
void Hierarchy::EndFrame(){
    // ゲームオブジェクトのフレーム終了処理
    for(auto& gameObject : gameObjects_){
        if(!gameObject->GetIsActive()){ continue; }
        gameObject->EndFrame();
    }

    for(auto& gameObject2D : gameObjects2D_){
        if(!gameObject2D->GetIsActive()){ continue; }
        gameObject2D->EndFrame();
    }
}


/////////////////////////////////////////////////////////////////////
// 終了処理
/////////////////////////////////////////////////////////////////////
void Hierarchy::Finalize(){
    gameObjects_.clear();
    gameObjects2D_.clear();
    grandParentObjects_.clear();
    grandParentObjects2D_.clear();
}

/////////////////////////////////////////////////////////////////////
// オブジェクトの存在確認
/////////////////////////////////////////////////////////////////////
bool Hierarchy::IsExistObject(uint32_t id) const{
    return existObjectIdMap_.find(id) != existObjectIdMap_.end();
}

bool Hierarchy::IsExistObject2D(uint32_t id) const{
    return existObjectIdMap2D_.find(id) != existObjectIdMap2D_.end();
}

/////////////////////////////////////////////////////////////////////
// オブジェクトのソート
/////////////////////////////////////////////////////////////////////
void Hierarchy::SortObject2DByTranslate(ObjSortMode sortMode){
    gameObjects2D_.sort([sortMode](const GameObject2D* a, const GameObject2D* b){

        const Vector2& ta = a->GetWorldTranslate();
        const Vector2& tb = b->GetWorldTranslate();

        switch(sortMode){
        case ObjSortMode::AscendX:
            return ta.x < tb.x;
        case ObjSortMode::DescendX:
            return ta.x > tb.x;
        case ObjSortMode::AscendY:
            return ta.y < tb.y;
        case ObjSortMode::DescendY:
            return ta.y > tb.y;
        default:
            return false; // Zはここでは無視
        }
    }
    );
}

////////////////////////////////////////////////////////////////////
// IDからオブジェクトを取得
////////////////////////////////////////////////////////////////////
GameObject* Hierarchy::GetGameObject(uint32_t id) const{
    for(auto* gameObject : gameObjects_){
        if(gameObject->GetObjectID() == id){
            return gameObject;
        }
    }
    return nullptr;
}

GameObject2D* Hierarchy::GetGameObject2D(uint32_t id) const{
    for(auto* gameObject2D : gameObjects2D_){
        if(gameObject2D->GetObjectID() == id){
            return gameObject2D;
        }
    }
    return nullptr;
}

////////////////////////////////////////////////////////////////////
// 名前からオブジェクトを取得
////////////////////////////////////////////////////////////////////
GameObject* Hierarchy::GetGameObject(const std::string& name) const{
    for(auto* gameObject : gameObjects_){
        if(gameObject->GetName() == name){
            return gameObject;
        }
    }
    return nullptr;
}

GameObject2D* Hierarchy::GetGameObject2D(const std::string& name) const{
    for(auto* gameObject2D : gameObjects2D_){
        if(gameObject2D->GetName() == name){
            return gameObject2D;
        }
    }
    return nullptr;
}

/////////////////////////////////////////////////////////////////////
// 親子関係の再構築
/////////////////////////////////////////////////////////////////////
void Hierarchy::RebuildParentInfo(){
    // 親のリストをクリア(毎フレーム初めに再構築)
    grandParentObjects_.clear();
    grandParentObjects2D_.clear();

    // 親子関係の再構築(親のいないオブジェクトだけのリストを作成)
    std::unordered_set<GameObject*> addedObjects;
    for(auto* gameObject : gameObjects_){
        while(true){
            GameObject* parent = gameObject->GetParent();
            if(!parent){
                if(addedObjects.find(gameObject) != addedObjects.end()){
                    break;
                }
                grandParentObjects_.push_back(gameObject);
                addedObjects.insert(gameObject);
                break;
            }

            gameObject = parent; // 親に移動
        }
    }

    std::unordered_set<GameObject2D*> addedObjects2D;
    for(auto* gameObject2D : gameObjects2D_){
        while(true){
            GameObject2D* parent = gameObject2D->GetParent();
            if(!parent){
                if(addedObjects2D.find(gameObject2D) != addedObjects2D.end()){
                    break;
                }
                grandParentObjects2D_.push_back(gameObject2D);
                addedObjects2D.insert(gameObject2D);
                break;
            }
            gameObject2D = parent; // 親に移動
        }
    }
}

/////////////////////////////////////////////////////////////////////
// GUI編集処理
/////////////////////////////////////////////////////////////////////
void Hierarchy::EditGUI(){
#ifdef _DEBUG
    std::string label;

    //==================================
    // ヒエラルキーの表示パネル
    //==================================

    ImFunc::CustomBegin("Hierarchy", MoveOnly_TitleBar);
    ImGui::Text("3Dオブジェクト");
    ImGui::Separator();

    for(auto& gameObject : grandParentObjects_){
        // ツリーノードの再帰的な作成
        RecursiveTreeNode(gameObject, 0);
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Text("2Dオブジェクト");
    ImGui::Separator();

    for(auto& gameObject2D : grandParentObjects2D_){
        // ツリーノードの再帰的な作成
        RecursiveTreeNode(gameObject2D, 0);
    }

    // オブジェクトの追加ボタン
    ImGui::Separator();
    CreateEmptyObject();

    // 入出力関連のGUI編集
    ImGui::Separator();
    InOutOnGUI();

    // コンテキストメニューの表示
    ExecuteContextMenu();

    ImGui::End();


    //==================================
    // 選択中のオブジェクトの編集パネル
    //==================================

    ImFunc::CustomBegin("ObjectEdit", MoveOnly_TitleBar);
    if(selectedObject_){

        // Guizmoに登録
        if(selectedObject_->GetParent()){
            Matrix4x4 parentWorldMat = selectedObject_->GetParent()->GetWorldMat();
            ImGuiManager::RegisterGuizmoItem(&selectedObject_->localTransform_, parentWorldMat);
        } else{
            ImGuiManager::RegisterGuizmoItem(&selectedObject_->localTransform_);
        }

        // 選択中のオブジェクトのGUI編集
        selectedObject_->EditGUI();

        OutputPrefab(selectedObject_);
    } else{

        if(selectedObject2D_){
            // Guizmoに登録
            if(selectedObject2D_->GetParent()){
                Matrix4x4 parentWorldMat = ToMat4x4(selectedObject2D_->GetParent()->GetWorldMat());
                ImGuiManager::RegisterGuizmoItem(&selectedObject2D_->localTransform_, parentWorldMat);
            } else{
                ImGuiManager::RegisterGuizmoItem(&selectedObject2D_->localTransform_);
            }

            // 選択中のオブジェクトのGUI編集
            selectedObject2D_->EditGUI();

            OutputPrefab(selectedObject2D_);
        } else{
            ImGui::Text("オブジェクトが選択されていません");
        }
    }
    ImGui::End();
#endif // _DEBUG
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 再帰的にツリーノードを作成
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Hierarchy::RecursiveTreeNode(GameObject* gameObject, int32_t depth){
    // ツリーノードの表示
    std::string nodeName = gameObject->GetName() + "##" + std::to_string(gameObject->GetObjectID());
    bool isSelected = (selectedObject_ == gameObject);
    bool isReset = false;

    // 色を変更する
    if(isSelected){
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.6f, 0.0f, 1.0f)); // オレンジ
        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(1.0f, 0.4f, 0.0f, 1.0f)); // 折りたたみヘッダ色
        isReset = false;
    }


    // TreeNode の展開状態
    bool open = ImGui::TreeNode(nodeName.c_str());

    // ダブルクリックで
    if(ImGui::IsItemClicked(0)){
        if(ImGui::IsMouseDoubleClicked(0)){
            selectedObject_ = gameObject;
            selectedObject2D_ = nullptr;
        }
    }

    // 右クリックでコンテキストメニューを開く
    if(ImGui::BeginPopupContextItem(nodeName.c_str())){

        contextMenuObject_ = gameObject;
        contextMenuObject2D_ = nullptr;

        // コンテキストメニューの表示
        if(ImGui::MenuItem("オブジェクトを削除")){
            executeMenuName_ = "Delete Object"; // 実際の削除は関数外で行う
        }
        if(ImGui::MenuItem("複製")){
            executeMenuName_ = "Copy Object"; // 実際のコピーは関数外で行う
        }

        ImGui::EndPopup();
    }

    // ドラッグ操作の送信側設定
    if(ImGui::BeginDragDropSource()){
        ImGui::SetDragDropPayload("GAMEOBJECT", &gameObject, sizeof(GameObject*));
        ImGui::Text("親を変更: %s", gameObject->GetName().c_str());
        ImGui::EndDragDropSource();
    }

    // ドラッグ操作の受信側設定
    if(ImGui::BeginDragDropTarget()){
        if(const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GAMEOBJECT")){
            if(payload->DataSize == sizeof(GameObject*)){
                GameObject* draggedObj = *(GameObject**)payload->Data;

                // 自分自身や子孫を親にしないようチェック
                if(draggedObj != gameObject && !gameObject->IsDescendant(draggedObj)){
                    // 親子付けを変更する処理
                    draggedObj->SetParent(gameObject);  // あなたの GameObject クラスに応じて関数を調整
                }
            }
        }

        if(const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("JOINT")){
            if(payload->DataSize == sizeof(DragInfo_Joint*)){
                DragInfo_Joint* draggedObj = *(DragInfo_Joint**)payload->Data;
                GameObject* owner = draggedObj->pComponent->GetOwner().owner3D;

                // 自分自身や子孫を親にしないようチェック
                if(owner != gameObject && !gameObject->IsDescendant(owner)){
                    // 親子draggedObj付けを変更する処理
                    gameObject->SetParent(owner);  // あなたの GameObject クラスに応じて関数を調整

                    // コンポーネントを親子付け情報に追加
                    ParentComponentInfo parentInfo;
                    parentInfo.pComponent = draggedObj->pComponent;
                    parentInfo.pMatrix = &draggedObj->pJoint->skeletonSpaceMatrix;
                    gameObject->SetParentComponentInfo(parentInfo);
                }
            }
        }

        ImGui::EndDragDropTarget();
    }

    // 子を表示（再帰）
    if(open){
        if(isSelected){
            ImGui::PopStyleColor(2);
            isReset = true;
        }
        for(auto& child : gameObject->GetChildren()){
            RecursiveTreeNode(child, depth + 1);
        }
        ImGui::TreePop();
    }

    if(isSelected && !isReset){
        ImGui::PopStyleColor(2);
    }
}

void Hierarchy::RecursiveTreeNode(GameObject2D* gameObject, int32_t depth){
    // ツリーノードの表示
    std::string nodeName = gameObject->GetName() + "##" + std::to_string(gameObject->GetObjectID());
    bool isSelected = (selectedObject2D_ == gameObject);
    bool isReset = false;

    // 色を変更する
    if(isSelected){
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.6f, 0.0f, 1.0f)); // オレンジ
        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(1.0f, 0.4f, 0.0f, 1.0f)); // 折りたたみヘッダ色
        isReset = false;
    }


    // TreeNode の展開状態
    bool open = ImGui::TreeNode(nodeName.c_str());

    // ダブルクリックで
    if(ImGui::IsItemClicked(0)){
        if(ImGui::IsMouseDoubleClicked(0)){
            selectedObject2D_ = gameObject;
            selectedObject_ = nullptr;
        }
    }

    // 右クリックでコンテキストメニューを開く
    if(ImGui::BeginPopupContextItem(nodeName.c_str())){

        contextMenuObject2D_ = gameObject;
        contextMenuObject_ = nullptr;

        // コンテキストメニューの表示
        if(ImGui::MenuItem("オブジェクトを削除")){
            executeMenuName_ = "Delete Object"; // 実際の削除は関数外で行う
        }

        if(ImGui::MenuItem("複製")){
            executeMenuName_ = "Copy Object"; // 実際のコピーは関数外で行う
        }

        ImGui::EndPopup();
    }

    // ドラッグ操作の送信側設定
    if(ImGui::BeginDragDropSource()){
        ImGui::SetDragDropPayload("GAMEOBJECT", &gameObject, sizeof(GameObject*));
        ImGui::Text("親を変更: %s", gameObject->GetName().c_str());
        ImGui::EndDragDropSource();
    }

    // ドラッグ操作の受信側設定
    if(ImGui::BeginDragDropTarget()){
        if(const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GAMEOBJECT")){
            if(payload->DataSize == sizeof(GameObject2D*)){
                GameObject2D* draggedObj = *(GameObject2D**)payload->Data;

                // 自分自身や子孫を親にしないようチェック
                if(draggedObj != gameObject && !gameObject->IsDescendant(draggedObj)){
                    // 親子付けを変更する処理
                    draggedObj->SetParent(gameObject);  // あなたの GameObject クラスに応じて関数を調整
                }
            }
        }
        ImGui::EndDragDropTarget();
    }

    // 子を表示（再帰）
    if(open){
        if(isSelected){
            ImGui::PopStyleColor(2);
            isReset = true;
        }
        for(auto& child : gameObject->GetChildren()){
            RecursiveTreeNode(child, depth + 1);
        }
        ImGui::TreePop();
    }

    if(isSelected && !isReset){
        ImGui::PopStyleColor(2);
    }
}

void Hierarchy::CreateEmptyObject(){
#ifdef _DEBUG
    if(ImGui::Button("空の3Dオブジェクトを追加")){
        // 空のゲームオブジェクトを作成
        auto& newObj = selfCreateObjects_.emplace_back(std::make_unique<GameObject>(GameSystem::GetScene()));
        newObj.get()->UpdateMatrix();
    }

    if(ImGui::Button("空の2Dオブジェクトを追加")){
        // 空のゲームオブジェクトを作成
        auto& newObj = selfCreateObjects2D_.emplace_back(std::make_unique<GameObject2D>(GameSystem::GetScene()));
        newObj.get()->UpdateMatrix();
    }

#endif
}

/////////////////////////////////////////////////////////////////////
// 入出力関連のGUI編集
/////////////////////////////////////////////////////////////////////
void Hierarchy::InOutOnGUI(){
#ifdef _DEBUG
    // Jsonファイルへの出力
    {
        static std::string outputFileDirectory = "Resources/jsons/Scenes/";
        static std::string filename = "";

        if(ImGui::Button("シーンをJsonファイルへ出力")){
            // 最前面表示
            filename.clear();
            ImGui::SetNextWindowFocus();
            ImGui::OpenPopup("OutputToJson");
        }

        // ポップアップの表示
        if(ImGui::BeginPopupModal("OutputToJson", NULL, ImGuiWindowFlags_AlwaysAutoResize)){

            ImGui::Text("シーンをJsonファイルへ出力します。");
            // 出力先のパスを入力
            ImFunc::InputText(".json", filename);

            // 出力ボタン
            if(ImGui::Button("出力", ImVec2(120, 0))){

                nlohmann::json j = OutputToJson(outputFileDirectory + filename + ".json", grandParentObjects_);
                nlohmann::json j2 = OutputToJson(outputFileDirectory + filename + ".json", grandParentObjects2D_);
                nlohmann::json result;
                if(!j.empty()){ result.update(j); }
                if(!j2.empty()){ result.update(j2); }
                MyFunc::CreateJsonFile(outputFileDirectory + filename + ".json", result);
                ImGui::CloseCurrentPopup();
            }

            // キャンセルボタン
            ImGui::SameLine();
            if(ImGui::Button("キャンセル", ImVec2(120, 0))){
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }


    // Jsonファイルからの読み込み
    if(ImGui::CollapsingHeader("Jsonファイルから読み込み")){

        // ファイルを選択して読み込み
        {
            ImGui::Text("シーンの読み込み");
            static std::filesystem::path prefabPath = "Resources/jsons/Scenes/";
            std::string selectedFile = ImFunc::FolderView("Scene", prefabPath, false, { ".json" }, "Resources/jsons/Scenes/");

            if(selectedFile != ""){
                LoadFromJson(selectedFile);
            }
        }
        {
            ImGui::Text("Prefabの読み込み");
            static std::filesystem::path prefabPath = "Resources/jsons/Prefabs/";
            std::string selectedFile = ImFunc::FolderView("Prefab", prefabPath, false, { ".json" }, "Resources/jsons/Prefabs/");
            if(selectedFile != ""){
                LoadFromJson(selectedFile, false);
            }
        }
    }




#endif
}

void Hierarchy::OutputPrefab(GameObject* gameObject){
    // Jsonファイルへの出力
    {
        static std::string outputFileDirectory = "Resources/jsons/Prefabs/";
        static std::string filename = "";
        static bool isSaveOnWorldOrigin = true;// 

        if(ImGui::Button("オブジェクトをJsonファイルへ出力")){
            // 最前面表示
            filename.clear();
            ImGui::SetNextWindowFocus();
            ImGui::OpenPopup("PrefabToJson");
        }

        // ポップアップの表示
        if(ImGui::BeginPopupModal("PrefabToJson", NULL, ImGuiWindowFlags_AlwaysAutoResize)){

            ImGui::Text("オブジェクトをJsonファイルへ出力します。");
            // 出力先のパスを入力
            ImFunc::InputText(".json", filename);

            // 出力ボタン
            if(ImGui::Button("出力", ImVec2(120, 0))){
                std::list<GameObject*> outputObject;
                outputObject.push_back(gameObject);
                OutputToJson(outputFileDirectory + filename + ".json", outputObject);
                ImGui::CloseCurrentPopup();
            }

            // キャンセルボタン
            ImGui::SameLine();
            if(ImGui::Button("キャンセル", ImVec2(120, 0))){
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }
}

void Hierarchy::OutputPrefab(GameObject2D* gameObject){
    // Jsonファイルへの出力
    {
        static std::string outputFileDirectory = "Resources/jsons/Prefabs/";
        static std::string filename = "";
        static bool isSaveOnWorldOrigin = true;// 

        if(ImGui::Button("オブジェクトをJsonファイルへ出力")){
            // 最前面表示
            filename.clear();
            ImGui::SetNextWindowFocus();
            ImGui::OpenPopup("PrefabToJson");
        }

        // ポップアップの表示
        if(ImGui::BeginPopupModal("PrefabToJson", NULL, ImGuiWindowFlags_AlwaysAutoResize)){

            ImGui::Text("オブジェクトをJsonファイルへ出力します。");
            // 出力先のパスを入力
            ImFunc::InputText(".json", filename);

            // 出力ボタン
            if(ImGui::Button("出力", ImVec2(120, 0))){
                std::list<GameObject2D*> outputObject;
                outputObject.push_back(gameObject);
                OutputToJson(outputFileDirectory + filename + ".json", outputObject);
                ImGui::CloseCurrentPopup();
            }

            // キャンセルボタン
            ImGui::SameLine();
            if(ImGui::Button("キャンセル", ImVec2(120, 0))){
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }
}

void Hierarchy::ExecuteContextMenu(){
#ifdef _DEBUG
    // 選択中のオブジェクトがある場合
    if(contextMenuObject_){
        // オブジェクトの削除
        if(executeMenuName_ == "Delete Object"){
            EraseObject(contextMenuObject_);
            contextMenuObject_ = nullptr;
            RebuildParentInfo();

        } else if(executeMenuName_ == "Copy Object"){
            // オブジェクトの複製
            nlohmann::json jsonData = contextMenuObject_->GetJsonData(0);
            // 子を再帰的に探索し、家族グループを作成
            std::vector<GameObject2D*> familyObjects = GameObject2D::CreateFamily(jsonData);
            // vector内の要素を自身の所有物にする
            for(auto& obj : familyObjects){
                selfCreateObjects2D_.emplace_back(std::unique_ptr<GameObject2D>(obj));
            }
        }

    } else{
        if(contextMenuObject2D_){
            // オブジェクトの削除
            if(executeMenuName_ == "Delete Object"){
                EraseObject(contextMenuObject2D_);
                contextMenuObject2D_ = nullptr;
                RebuildParentInfo();

            } else if(executeMenuName_ == "Copy Object"){
                // オブジェクトの複製
                nlohmann::json jsonData = contextMenuObject2D_->GetJsonData(0);
                // 子を再帰的に探索し、家族グループを作成
                std::vector<GameObject2D*> familyObjects = GameObject2D::CreateFamily(jsonData);
                // vector内の要素を自身の所有物にする
                for(auto& obj : familyObjects){
                    selfCreateObjects2D_.emplace_back(std::unique_ptr<GameObject2D>(obj));
                }
            }
        }
    }

    // 処理が終わったらクリア
    if(executeMenuName_ != ""){
        executeMenuName_ = "";
    }

#endif // _DEBUG
}


/////////////////////////////////////////////////////////////////////
// Jsonファイルへの出力
/////////////////////////////////////////////////////////////////////
nlohmann::json Hierarchy::OutputToJson(const std::string& outputFilePath, std::list<GameObject*> grandParentObjects) const{
    static nlohmann::json sceneData;
    sceneData.clear(); // 既存のデータをクリア

    // 各オブジェクトのjsonデータを取得
    for(const auto& grandParent : grandParentObjects){
        sceneData["gameObjects"].push_back(grandParent->GetJsonData(0));
    }

    // 出力
    std::ofstream ofs(outputFilePath);
    ofs << sceneData.dump(4); // 4スペースでインデント
    ofs.close();

    return sceneData; // 出力したjsonデータを返す
}

nlohmann::json Hierarchy::OutputToJson(const std::string& outputFilePath, std::list<GameObject2D*> grandParentObjects) const{
    static nlohmann::json sceneData;
    sceneData.clear(); // 既存のデータをクリア
    // 各オブジェクトのjsonデータを取得
    for(const auto& grandParent : grandParentObjects){
        sceneData["gameObjects2D"].push_back(grandParent->GetJsonData(0));
    }
    // 出力
    std::ofstream ofs(outputFilePath);
    ofs << sceneData.dump(4); // 4スペースでインデント
    ofs.close();
    return sceneData; // 出力したjsonデータを返す
}

/////////////////////////////////////////////////////////////////////////
// Jsonファイルからの読み込み
/////////////////////////////////////////////////////////////////////////
LoadObjectData Hierarchy::LoadFromJson(const std::string& filePath, bool resetObjects){

    // 結果格納用
    LoadObjectData result;

    // ファイルを開く
    std::ifstream ifs(filePath);
    if(ifs.fail()){
        assert(false);
        return result;
    }

    // jsonデータに変換
    nlohmann::json jsonData;
    ifs >> jsonData;
    ifs.close();

    // 現在のシーンをクリアする
    if(resetObjects){
        selfCreateObjects_.clear();
    }

    // 読み込む
    for(const auto& gameObjectJson : jsonData["gameObjects"]){
        // 子を再帰的に探索し、家族グループを作成
        std::vector<GameObject*> familyObjects = GameObject::CreateFamily(gameObjectJson);
        // vector内の要素を自身の所有物にする
        for(auto& obj : familyObjects){
            selfCreateObjects_.emplace_back(std::unique_ptr<GameObject>(obj));
            result.objects3D_.emplace_back(obj);
        }
    }

    for(const auto& gameObjectJson : jsonData["gameObjects2D"]){
        // 子を再帰的に探索し、家族グループを作成
        std::vector<GameObject2D*> familyObjects = GameObject2D::CreateFamily(gameObjectJson);
        // vector内の要素を自身の所有物にする
        for(auto& obj : familyObjects){
            selfCreateObjects2D_.emplace_back(std::unique_ptr<GameObject2D>(obj));
            result.objects2D_.emplace_back(obj);
        }
    }

    return result;
}
