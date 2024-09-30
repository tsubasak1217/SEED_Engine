#include "EventScene.h"

EventScene::EventScene(){

    // IDの割り当て
    nodeID_ = nodeID_next_;
    nodeID_next_++;

    // 名前・色・初期座標の設定
    nodeColor_ = IM_COL32(255, 112, 0, 255);
    position_ = { 580,300 };
    ImNodes::SetNodeGridSpacePos(nodeID_, position_);

    // 前のシーン・次のシーン
    nextSceneID_["nextScene_" + std::to_string((int)nextSceneID_.size() + 1)] = pinID_next_;
    pinID_next_++;
    prevSceneID_ = pinID_next_;
    pinID_next_++;

    // フェードイン・フェードアウト
    inputID_["inScene"] = pinID_next_;
    pinID_next_++;
    inputID_["outScene"] = pinID_next_;
    pinID_next_++;
}

EventScene::~EventScene(){

}

////////////////////////////////////////////////////////////////////
//                             描画
////////////////////////////////////////////////////////////////////

void EventScene::Draw(){

    // 開始時処理
    BeginNode();

    // 名前の入力フィールド
    Draw_SceneSetting();

    // 前のシーンにつなぐピン
    Draw_Pin_PrevScene();

    // 次のシーンにつなぐピン
    Draw_Pin_NextScenes();

    // シーンの入り・終わりのシーンピン
    Draw_Pin_InOutScene();

    // 終了
    EndNode();
}


////////////////////////////////////////////////////////////////////
//                          開始時処理
////////////////////////////////////////////////////////////////////

void EventScene::BeginNode(){

    // 色の設定
    ImNodes::PushColorStyle(ImNodesCol_TitleBar, nodeColor_);
    ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, nodeColor_);

    // タイトルバーの描画
    ImNodes::BeginNode(nodeID_);
    ImNodes::BeginNodeTitleBar();
    ImGui::TextUnformatted("Event Scene");
    ImNodes::EndNodeTitleBar();
}

////////////////////////////////////////////////////////////////////
//                    シーン名の入力フィールド
////////////////////////////////////////////////////////////////////

void EventScene::Draw_SceneSetting(){

    ImGui::Text("------------ Scene Setting -----------");

    // シーンの名前入力
    ImGui::Text("Scene Name");
    ImGui::SetNextItemWidth(200);
    ImGui::InputText("##Scene Name", name_, IM_ARRAYSIZE(name_));

}

////////////////////////////////////////////////////////////////////
//                     前のシーンにつなぐピン
////////////////////////////////////////////////////////////////////

void EventScene::Draw_Pin_PrevScene(){

    ImGui::Dummy({ 0.0f,10.0f });
    ImGui::Text("------------ Previous Scene -----------");

    ImGui::Text("Previous Scene");
    ImNodes::BeginInputAttribute(prevSceneID_);
    ImNodes::EndInputAttribute();

}

////////////////////////////////////////////////////////////////////
//                     次のシーンにつなぐピン
////////////////////////////////////////////////////////////////////

void EventScene::Draw_Pin_NextScenes(){

    ImGui::Dummy({ 0.0f,10.0f });
    ImGui::Text("------------- Next Scenes -------------");

    for(auto& nextScene : nextSceneID_){
        ImGui::Text(nextScene.first.c_str());
        ImNodes::BeginOutputAttribute(nextScene.second);
        ImNodes::EndOutputAttribute();
    }

    // ボタンが押されたらピンを増やす
    if(ImGui::Button("Add Pin##1")){
        nextSceneID_["nextScene_" + std::to_string((int)nextSceneID_.size() + 1)] = pinID_next_;
        pinID_next_++;
    };
}


////////////////////////////////////////////////////////////////////
//                   シーンの入り・終わりシーンのピン
////////////////////////////////////////////////////////////////////

void EventScene::Draw_Pin_InOutScene(){

    ImGui::Dummy({ 0.0f,10.0f });
    ImGui::Text("------------- In/Out Scene -------------");

    ImGui::Text("InScene");
    ImNodes::BeginInputAttribute(inputID_["inScene"]);
    ImNodes::EndInputAttribute();

    ImGui::Dummy({ 0.0f,10.0f });

    ImGui::Text("OutScene");
    ImNodes::BeginInputAttribute(inputID_["outScene"]);
    ImNodes::EndInputAttribute();
}

////////////////////////////////////////////////////////////////////
//                          終了処理
////////////////////////////////////////////////////////////////////

void EventScene::EndNode(){
    ImNodes::EndNode();
    ImNodes::PopColorStyle();
    ImNodes::PopColorStyle();
}

