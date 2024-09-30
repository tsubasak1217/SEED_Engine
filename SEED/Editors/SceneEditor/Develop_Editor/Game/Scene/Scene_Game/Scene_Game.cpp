#include "Scene_Game.h"
#include <GameState_Play.h>
#include <GameState_Enter.h>
#include <SEED.h>
#include <Environment.h>

Scene_Game::Scene_Game(SceneManager* pSceneManager){
    pSceneManager_ = pSceneManager;
    ChangeState(new GameState_Play(this));
    Initialize();
};

Scene_Game::~Scene_Game(){}

void Scene_Game::Initialize(){
    nodes_.emplace_back(std::make_unique<MainScene>());
}

void Scene_Game::Finalize(){}

void Scene_Game::Update(){
    /*======================= 前フレームの値保存 ======================*/


    /*========================== ImGui =============================*/

#ifdef _DEBUG

    ///////////////////////////////////////////////////////
    // 開始時処理
    ///////////////////////////////////////////////////////

    BeginEditor();

    ///////////////////////////////////////////////////////
    // ノード・リンクの描画
    ///////////////////////////////////////////////////////

    for(auto& node : nodes_){
        node->Draw();
    }

    ImNodes::MiniMap();
    DrawLink();

    ///////////////////////////////////////////////////////
    // 終了時処理
    ///////////////////////////////////////////////////////

    EndEditor();

    // リンクの更新
    UpdateLink();

#endif

    /*========================= 各状態のの更新 ==========================*/

    currentState_->Update();

}



void Scene_Game::Draw(){
    currentState_->Draw();
}



void Scene_Game::BeginEditor(){
    // ウィンドウの位置とサイズを固定
    ImGui::SetNextWindowPos(ImVec2(0, 0));     // ウィンドウの左上の位置を指定
    ImGui::SetNextWindowSize(ImVec2(kWindowSizeX, kWindowSizeY));  // ウィンドウの大きさを指定


    ImGui::Begin(
        "SceneEditor", nullptr,
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar |
        ImGuiWindowFlags_NoTitleBar
    );

    DisplayMenu();

    // エディターのコンテキストの作成・設定
    //editorContext_ = ImNodes::EditorContextCreate();
    //ImNodes::EditorContextSet(editorContext_);

    // エディターの背景色を変更
    ImNodes::PushColorStyle(ImNodesCol_GridBackground, IM_COL32(5, 5, 5, 255));  // 背景色をグレーに変更
    ImNodes::PushColorStyle(ImNodesCol_GridLine, IM_COL32(25, 25, 25, 255));

    // エディターの開始
    ImNodes::BeginNodeEditor();
}



void Scene_Game::DrawLink(){
    // 既存のリンクを描画
    for(auto& link : links_)
    {
        ImNodes::PushColorStyle(ImNodesCol_Link, IM_COL32(255, 200, 0, 255));
        ImNodes::Link(link->id, link->start_attr, link->end_attr);
        ImNodes::PopColorStyle();
    }
}



void Scene_Game::UpdateLink(){

    // マウス操作による新しいリンクが作成されたかを確認
    int start_attr, end_attr;
    if(ImNodes::IsLinkCreated(&start_attr, &end_attr))
    {
        // 新しいリンクを追加
        links_.emplace_back(std::make_unique<NodeLink>());

        // 情報の入力
        links_.back()->id = NodeLink::nextID;
        NodeLink::nextID++;
        links_.back()->start_attr = start_attr;
        links_.back()->end_attr = end_attr;
    }


    // リンク上で右クリックが発生したかをチェック
    for(auto& link : links_){
        if(ImNodes::IsLinkHovered(&link->id) && ImGui::IsMouseClicked(1)) {
            ImGui::OpenPopup("link_context_menu");
            hoveredLinkID_ = link->id;
            break;
        }
    }

    // ポップアップメニューで「Delete Link」を表示
    if(ImGui::BeginPopup("link_context_menu")) {
        if(ImGui::MenuItem("Delete Link")) {

            // リンクの削除処理をここで実行
            for(auto it = links_.begin(); it != links_.end();){
                if(it->get()->id == hoveredLinkID_){
                    links_.erase(it);
                    break;
                } else{
                    it++;
                }
            }
        }

        ImGui::EndPopup();
    }
}



void Scene_Game::EndEditor(){
    // エディターの終了
    ImNodes::EndNodeEditor();
    ImNodes::PopColorStyle();
    ImNodes::PopColorStyle();
    // コンテキストの破棄
    //ImNodes::EditorContextFree(editorContext_);
    // imguiの終了
    ImGui::End();
}

void Scene_Game::DisplayMenu(){
    // メニューバーを開始
    if(ImGui::BeginMenuBar())
    {
        //////////////////////////////////////////////////////////
        //
        //////////////////////////////////////////////////////////
        if(ImGui::BeginMenu("AddScene")){

            if(ImGui::MenuItem("MainScene")){
                nodes_.emplace_back(std::make_unique<MainScene>());
            }
            
            if(ImGui::MenuItem("EventScene")){

            }

            if(ImGui::MenuItem("InScene")){
                nodes_.emplace_back(std::make_unique<InScene>());
            }

            if(ImGui::MenuItem("OutScene")){

            }

            ImGui::EndMenu();

        }
        
        //////////////////////////////////////////////////////////
        //
        //////////////////////////////////////////////////////////
        if(ImGui::BeginMenu("AddEvent")){

            if(ImGui::MenuItem("Button")){
            }

            if(ImGui::MenuItem("TimeLimit")){

            }

            if(ImGui::MenuItem("Custom")){

            }

            ImGui::EndMenu();

        }


        //////////////////////////////////////////////////////////
        //
        //////////////////////////////////////////////////////////
        if(ImGui::BeginMenu("File")){

            if(ImGui::MenuItem("Export json")){
            }

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar(); // メニューバーを終了
    }
}
