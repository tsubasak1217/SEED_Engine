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
    existObjectIdMap_.insert(gameObject->GetObjectID());
}

void Hierarchy::RegisterGameObject(GameObject2D* gameObject2D){
    // 2Dゲームオブジェクトを登録
    existObjectIdMap2D_.insert(gameObject2D->GetObjectID());
}

void Hierarchy::RemoveGameObject(GameObject* gameObject){
    // ゲームオブジェクトを削除
    if(isEndHierarchy_){ return; }
    existObjectIdMap_.erase(gameObject->GetObjectID());

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
        auto it = std::find_if(gameObjects_.begin(), gameObjects_.end(),
            [child](const std::unique_ptr<GameObject>& obj){ return obj.get() == child; });

        // selfCreateObjects_から解放
        if(it != gameObjects_.end()){
            gameObjects_.erase(it); // 自分で生成したオブジェクトのリストから削除

        } else{// 直接解放
            delete child; // 子オブジェクトを削除
            child = nullptr; // ポインタをクリア
        }
    }

    // selfCreatedObjectにあるか確認
    auto it = std::find_if(gameObjects_.begin(), gameObjects_.end(),
        [gameObject](const std::unique_ptr<GameObject>& obj){ return obj.get() == gameObject; });

    // selfCreateObjects_から解放
    if(it != gameObjects_.end()){
        gameObjects_.erase(it); // 自分で生成したオブジェクトのリストから削除

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
        auto it = std::find_if(gameObjects2D_.begin(), gameObjects2D_.end(),
            [child](const std::unique_ptr<GameObject2D>& obj){ return obj.get() == child; });

        // selfCreateObjects_から解放
        if(it != gameObjects2D_.end()){
            gameObjects2D_.erase(it); // 自分で生成したオブジェクトのリストから削除

        } else{// 直接解放
            delete child; // 子オブジェクトを削除
            child = nullptr; // ポインタをクリア
        }
    }

    // selfCreatedObjectにあるか確認
    auto it = std::find_if(gameObjects2D_.begin(), gameObjects2D_.end(),
        [gameObject](const std::unique_ptr<GameObject2D>& obj){ return obj.get() == gameObject; });

    // selfCreateObjects_から解放
    if(it != gameObjects2D_.end()){
        gameObjects2D_.erase(it); // 自分で生成したオブジェクトのリストから削除

    } else{// 直接解放
        delete gameObject; // 自分自身を削除
        gameObject = nullptr; // ポインタをクリア
    }
}

///////////////////////////////////////////////////////////////////
// 全オブジェクトの削除
///////////////////////////////////////////////////////////////////
void Hierarchy::EraseAllObject(){
    // 全オブジェクトを削除
    gameObjects_.clear();
    gameObjects2D_.clear();
    existObjectIdMap_.clear();
    existObjectIdMap2D_.clear();
}

///////////////////////////////////////////////////////////////////
// コンストラクタ・デストラクタ
///////////////////////////////////////////////////////////////////
void Hierarchy::BeginFrame(){
    // 親子関係の再構築
    RebuildParentInfo();

    // ゲームオブジェクトのフレーム開始処理
    for(auto& gameObject : gameObjects_){
        if(!gameObject->isActive_){ continue; }
        gameObject->BeginFrame();
    }

    for(auto& gameObject2D : gameObjects2D_){
        if(!gameObject2D->isActive_){ continue; }
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
        if(!gameObject->isActive_){ continue; }
        gameObject->Update();
    }

    for(auto& gameObject2D : gameObjects2D_){
        if(!gameObject2D->isActive_){ continue; }
        gameObject2D->Update();
    }
}

/////////////////////////////////////////////////////////////////////
// 描画処理
/////////////////////////////////////////////////////////////////////
void Hierarchy::Draw(){
    // ゲームオブジェクトの描画
    for(auto& gameObject : gameObjects_){
        if(!gameObject->isActive_ && !gameObject->isMustDraw_){ continue; }
        gameObject->Draw();
    }

    for(auto& gameObject2D : gameObjects2D_){
        if(!gameObject2D->isActive_ && !gameObject2D->isMustDraw_){ continue; }
        gameObject2D->Draw();
    }
}

/////////////////////////////////////////////////////////////////////
// フレーム終了処理
/////////////////////////////////////////////////////////////////////
void Hierarchy::EndFrame(){
    // ゲームオブジェクトのフレーム終了処理
    for(auto& gameObject : gameObjects_){
        if(!gameObject->isActive_){ continue; }
        gameObject->EndFrame();
    }

    for(auto& gameObject2D : gameObjects2D_){
        if(!gameObject2D->isActive_){ continue; }
        gameObject2D->EndFrame();
    }

    // aliveでないオブジェクトの削除
    for(auto it = gameObjects_.begin(); it != gameObjects_.end(); ){
        if(!(*it)->isAlive_){
            //消す前に次のイテレータを保存しておく
            auto nextIt = std::next(it);
            EraseObject((*it).get());
            it = nextIt; // イテレータを次に進める
        } else{
            ++it;
        }
    }

    for(auto it = gameObjects2D_.begin(); it != gameObjects2D_.end(); ){
        if(!(*it)->isAlive_){
            //消す前に次のイテレータを保存しておく
            auto nextIt = std::next(it);
            EraseObject((*it).get());
            it = nextIt; // イテレータを次に進める
        } else{
            ++it;
        }
    }
}


/////////////////////////////////////////////////////////////////////
// 終了処理
/////////////////////////////////////////////////////////////////////
void Hierarchy::Finalize(){
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
    gameObjects2D_.sort([sortMode](const std::unique_ptr<GameObject2D>& a, const std::unique_ptr<GameObject2D>& b){

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
    });
}

////////////////////////////////////////////////////////////////////
// IDからオブジェクトを取得
////////////////////////////////////////////////////////////////////
GameObject* Hierarchy::GetGameObject(uint32_t id) const{
    for(auto& gameObject : gameObjects_){
        if(gameObject->GetObjectID() == id){
            return gameObject.get();
        }
    }
    return nullptr;
}

GameObject2D* Hierarchy::GetGameObject2D(uint32_t id) const{
    for(auto& gameObject2D : gameObjects2D_){
        if(gameObject2D->GetObjectID() == id){
            return gameObject2D.get();
        }
    }
    return nullptr;
}

////////////////////////////////////////////////////////////////////
// 名前からオブジェクトを取得
////////////////////////////////////////////////////////////////////
GameObject* Hierarchy::GetGameObject(const std::string& name) const{
    for(auto& gameObject : gameObjects_){
        if(gameObject->GetName() == name){
            return gameObject.get();
        }
    }
    return nullptr;
}

GameObject2D* Hierarchy::GetGameObject2D(const std::string& name) const{
    for(auto& gameObject2D : gameObjects2D_){
        if(gameObject2D->GetName() == name){
            return gameObject2D.get();
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
    for(auto& gameObject : gameObjects_){
        GameObject* currentObject = gameObject.get();
        while(true){
            GameObject* parent = currentObject->GetParent();
            if(parent == nullptr){
                if(addedObjects.find(currentObject) != addedObjects.end()){
                    break;
                }
                grandParentObjects_.push_back(currentObject);
                addedObjects.insert(currentObject);
                break;
            }

            currentObject = parent; // 親に移動
        }
    }

    std::unordered_set<GameObject2D*> addedObjects2D;
    for(auto& gameObject2D : gameObjects2D_){
        GameObject2D* currentObject = gameObject2D.get();
        while(true){
            GameObject2D* parent = currentObject->GetParent();
            if(!parent){
                if(addedObjects2D.find(currentObject) != addedObjects2D.end()){
                    break;
                }
                grandParentObjects2D_.push_back(currentObject);
                addedObjects2D.insert(currentObject);
                break;
            }
            currentObject = parent; // 親に移動
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

    for(auto* gameObject : grandParentObjects_){
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
    AddEmptyObjectGUI();

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
    ImFunc::BeginDrag("GAMEOBJECT", gameObject, "親を変更: " + gameObject->GetName());


    // ドロップで親子付けを変更
    if(auto payload = ImFunc::GetDroppedData<GameObject*>("GAMEOBJECT")){
        // 自分自身や子孫を親にしないようチェック
        if(*payload != gameObject && !gameObject->IsDescendant(*payload)){
            // 親子付けを変更する処理
            payload.value()->SetParent(gameObject);
        }
    }

    // ドロップでジョイントの親子付けを変更
    if(auto payload = ImFunc::GetDroppedData<DragInfo_Joint*>("JOINT")){
        GameObject* owner = payload.value()->pComponent->GetOwner().owner3D;

        // 自分自身や子孫を親にしないようチェック
        if(owner != gameObject && !gameObject->IsDescendant(owner)){
            // 親子draggedObj付けを変更する処理
            gameObject->SetParent(owner);

            // コンポーネントを親子付け情報に追加
            ParentComponentInfo parentInfo;
            parentInfo.pComponent = payload.value()->pComponent;
            parentInfo.pMatrix = &payload.value()->pJoint->skeletonSpaceMatrix;
            gameObject->SetParentComponentInfo(parentInfo);
        }
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
    ImFunc::BeginDrag("GAMEOBJECT", gameObject, "親を変更: " + gameObject->GetName());

    // ドロップで親子付けを変更
    if(auto payload = ImFunc::GetDroppedData<GameObject2D*>("GAMEOBJECT")){
        // 自分自身や子孫を親にしないようチェック
        if(*payload != gameObject && !gameObject->IsDescendant(*payload)){
            // 親子付けを変更する処理
            payload.value()->SetParent(gameObject);
        }
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

void Hierarchy::AddEmptyObjectGUI(){
#ifdef _DEBUG
    if(ImGui::Button("空の3Dオブジェクトを追加")){
        // 空のゲームオブジェクトを作成
        auto& newObj = gameObjects_.emplace_back(std::unique_ptr<GameObject>(new GameObject(GameSystem::GetScene())));
        newObj.get()->UpdateMatrix();
    }

    if(ImGui::Button("空の2Dオブジェクトを追加")){
        // 空のゲームオブジェクトを作成
        auto& newObj = gameObjects2D_.emplace_back(std::unique_ptr<GameObject2D>(new GameObject2D(GameSystem::GetScene())));
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
            filename = MyFunc::OpenSaveFileDialog(outputFileDirectory, ".scene", "拡張子は不要です");

            // ダイアログ上で保存ボタンが押されたら保存処理を行う
            if(!filename.empty()){
                // 3D,2Dオブジェクトを同じjsonにまとめる
                nlohmann::json j = OutputToJson(grandParentObjects_);
                nlohmann::json j2 = OutputToJson(grandParentObjects2D_);
                nlohmann::json result;
                if(!j.empty()){ result.update(j); }
                if(!j2.empty()){ result.update(j2); }
                // 出力
                MyFunc::CreateJsonFile(filename,result);
                // filenameをファイル名のみに戻す
                filename = std::filesystem::path(filename).filename().stem().string();
            }
        }
    }


    // Jsonファイルからの読み込み
    if(ImGui::CollapsingHeader("Jsonファイルから読み込み")){

        // ファイルを選択して読み込み
        {
            ImGui::Text("シーンの読み込み");
            static std::filesystem::path prefabPath = "Resources/jsons/Scenes/";
            std::string selectedFile = ImFunc::FolderView("Scene", prefabPath, false, { ".scene" }, "Resources/jsons/Scenes/");

            if(selectedFile != ""){
                LoadScene(selectedFile);
            }
        }
        {
            ImGui::Text("Prefabの読み込み");
            static std::filesystem::path prefabPath = "Resources/jsons/Prefabs/";
            std::string selectedFile = ImFunc::FolderView("Prefab", prefabPath, false, { ".prefab" }, "Resources/jsons/Prefabs/");
            if(selectedFile != ""){
                // 2Dか3Dか判別できないので両方実行して確実に読み込む
                if(!LoadObject(selectedFile)){
                    LoadObject2D(selectedFile);
                }
            }
        }
    }
#endif
}

void Hierarchy::OutputPrefab(GameObject* gameObject){
    // Jsonファイルへの出力
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

        // 保存設定
        ImGui::SeparatorText("出力詳細設定");
        ImGui::Checkbox("ワールド原点に保存", &isSaveOnWorldOrigin);

        // 出力ボタン
        if(ImGui::Button("出力", ImVec2(120, 0))){
            filename.clear();
            filename = MyFunc::OpenSaveFileDialog(outputFileDirectory, ".prefab", "拡張子は不要です");

            // ダイアログ上で保存ボタンが押されたら保存処理を行う
            if(!filename.empty()){
                std::list<GameObject*> outputObject;
                outputObject.push_back(gameObject);
                MyFunc::CreateJsonFile(filename, OutputToJson(outputObject, isSaveOnWorldOrigin));
                // filenameをファイル名のみに戻す
                filename = std::filesystem::path(filename).filename().stem().string();
            }
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

void Hierarchy::OutputPrefab(GameObject2D* gameObject){
    // Jsonファイルへの出力
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

        // 保存設定
        ImGui::SeparatorText("出力詳細設定");
        ImGui::Checkbox("ワールド原点に保存", &isSaveOnWorldOrigin);

        // 出力ボタン
        if(ImGui::Button("出力", ImVec2(120, 0))){
            filename.clear();
            filename = MyFunc::OpenSaveFileDialog(outputFileDirectory, ".prefab", "拡張子は不要です");

            // ダイアログ上で保存ボタンが押されたら保存処理を行う
            if(!filename.empty()){
                std::list<GameObject2D*> outputObject;
                outputObject.push_back(gameObject);
                MyFunc::CreateJsonFile(filename, OutputToJson(outputObject, isSaveOnWorldOrigin));
                // filenameをファイル名のみに戻す
                filename = std::filesystem::path(filename).filename().stem().string();
            }
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
            std::vector<GameObject*> familyObjects = GameObject::CreateFamily(jsonData);
            // vector内の要素を自身の所有物にする
            for(auto& obj : familyObjects){
                gameObjects_.emplace_back(std::unique_ptr<GameObject>(obj));
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
                    gameObjects2D_.emplace_back(std::unique_ptr<GameObject2D>(obj));
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
nlohmann::json Hierarchy::OutputToJson(
    std::list<GameObject*> grandParentObjects,
    bool isSaveOnOrigin
) const{
    static nlohmann::json sceneData;
    sceneData.clear(); // 既存のデータをクリア

    // 各オブジェクトのjsonデータを取得
    for(const auto& grandParent : grandParentObjects){

        // ワールド原点に保存する場合、位置を(0,0)、回転を0、拡縮を1にする
        Transform originalTransform = grandParent->localTransform_;
        if(isSaveOnOrigin){
            grandParent->localTransform_.translate = { 0.0f,0.0f,0.0f };
        }
        // jsonデータを取得して追加
        sceneData["gameObjects"].push_back(grandParent->GetJsonData(0));

        // 元に戻す
        grandParent->localTransform_ = originalTransform;

    }
    return sceneData; // 出力したjsonデータを返す
}

nlohmann::json Hierarchy::OutputToJson(
    std::list<GameObject2D*> grandParentObjects,
    bool isSaveOnOrigin
) const{
    static nlohmann::json sceneData;
    sceneData.clear(); // 既存のデータをクリア

    // 各オブジェクトのjsonデータを取得
    for(const auto& grandParent : grandParentObjects){

        // ワールド原点に保存する場合、位置を(0,0)、回転を0、拡縮を1にする
        Transform2D originalTransform = grandParent->localTransform_;
        if(isSaveOnOrigin){
            grandParent->localTransform_.translate = { 0.0f,0.0f };
        }
        // jsonデータを取得して追加
        sceneData["gameObjects2D"].push_back(grandParent->GetJsonData(0));

        // 元に戻す
        grandParent->localTransform_ = originalTransform;

    }
    return sceneData; // 出力したjsonデータを返す
}

/////////////////////////////////////////////////////////////////////////
// Jsonファイルからの読み込み
/////////////////////////////////////////////////////////////////////////

// シーン全体の読み込み
LoadObjectData Hierarchy::LoadScene(const std::string& filePath, bool resetObjects){

    // 結果格納用
    LoadObjectData result;

    // ファイルパスが".scene"じゃなければ何もしない
    if(std::filesystem::path(filePath).extension() != ".scene"){
        assert(false);
        return result;
    }

    // ファイルを開く
    nlohmann::json json;
    if(filePath.starts_with("Resources")){
        json = MyFunc::GetJson(filePath);
    } else{
        json = MyFunc::GetJson(sceneJsonDirectory_ + filePath);
    }

    // 現在のシーンをクリアする
    if(resetObjects){
        gameObjects_.clear();
    }

    // 読み込む
    for(const auto& gameObjectJson : json["gameObjects"]){
        // 子を再帰的に探索し、家族グループを作成
        std::vector<GameObject*> familyObjects = GameObject::CreateFamily(gameObjectJson);
        // vector内の要素を自身の所有物にする
        for(auto& obj : familyObjects){
            gameObjects_.emplace_back(std::unique_ptr<GameObject>(obj));
            result.objects3D_.emplace_back(obj);
        }
    }

    for(const auto& gameObjectJson : json["gameObjects2D"]){
        // 子を再帰的に探索し、家族グループを作成
        std::vector<GameObject2D*> familyObjects = GameObject2D::CreateFamily(gameObjectJson);
        // vector内の要素を自身の所有物にする
        for(auto& obj : familyObjects){
            gameObjects2D_.emplace_back(std::unique_ptr<GameObject2D>(obj));
            result.objects2D_.emplace_back(obj);
        }
    }

    return result;
}


// Prefab単体の読み込み(3D)
GameObject* Hierarchy::LoadObject(const std::string& filePath){

    // ファイルが".prefab"ではなければnullptrを返す
    if(std::filesystem::path(filePath).extension() != ".prefab"){
        assert(false);
        return nullptr;
    }

    // ファイルを開く
    nlohmann::json json;
    if(filePath.starts_with("Resources")){
        json = MyFunc::GetJson(filePath);
    } else{
        json = MyFunc::GetJson(prefabJsonDirectory_ + filePath);
    }

    // 3Dの場合
    if(json.contains("gameObjects")){
        // 子を再帰的に探索し、家族グループを作成
        std::vector<GameObject*> familyObjects = GameObject::CreateFamily(json["gameObjects"][0]);
        // vector内の要素を自身の所有物にする
        for(auto& obj : familyObjects){
            gameObjects_.emplace_back(std::unique_ptr<GameObject>(obj));
        }

        // 読み込んだ結果を返す(無ければnullptr)
        if(familyObjects.empty()){ return nullptr; }
        return familyObjects.front();
    }

    return nullptr;
}


// Prefab単体の読み込み(2D)
GameObject2D* Hierarchy::LoadObject2D(const std::string& filePath){

    // ファイルが".prefab"ではなければnullptrを返す
    if(std::filesystem::path(filePath).extension() != ".prefab"){
        assert(false);
        return nullptr;
    }

    // ファイルを開く
    nlohmann::json json;
    if(filePath.starts_with("Resources")){
        json = MyFunc::GetJson(filePath);
    } else{
        json = MyFunc::GetJson(prefabJsonDirectory_ + filePath);
    }
    
    // 2Dの場合
    if(json.contains("gameObjects2D")){
        // 子を再帰的に探索し、家族グループを作成
        std::vector<GameObject2D*> familyObjects = GameObject2D::CreateFamily(json["gameObjects2D"][0]);
        // vector内の要素を自身の所有物にする
        for(auto& obj : familyObjects){
            gameObjects2D_.emplace_back(std::unique_ptr<GameObject2D>(obj));
        }
        // 読み込んだ結果を返す(無ければnullptr)
        if(familyObjects.empty()){ return nullptr; }
        return familyObjects.front();
    }
    return nullptr;
}


//////////////////////////////////////////////////////////////////////
// 空のオブジェクト作成
//////////////////////////////////////////////////////////////////////
GameObject* Hierarchy::CreateEmptyObject(){
    auto& newObj = gameObjects_.emplace_back(std::unique_ptr<GameObject>(new GameObject(GameSystem::GetScene())));
    newObj.get()->UpdateMatrix();
    return newObj.get();
}

GameObject2D* Hierarchy::CreateEmptyObject2D(){
    auto& newObj = gameObjects2D_.emplace_back(std::unique_ptr<GameObject2D>(new GameObject2D(GameSystem::GetScene())));
    newObj.get()->UpdateMatrix();
    return newObj.get();
}
