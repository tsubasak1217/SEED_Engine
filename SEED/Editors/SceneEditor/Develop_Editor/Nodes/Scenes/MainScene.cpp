#include "MainScene.h"

MainScene::MainScene(){

    // IDの割り当て
    nodeID_ = nodeID_next_;
    nodeID_next_++;

    // 名前・色・初期座標の設定
    nodeColor_ = IM_COL32(255, 0, 195, 255);
    position_ = { 580,300 };
    ImNodes::SetNodeGridSpacePos(nodeID_, position_);

    // 前のシーン・次のシーン
    nextSceneID_["nextScene_" + std::to_string((int)nextSceneID_.size() + 1)] = pinID_next_;
    pinID_next_++;
    prevSceneID_["prevScene_" + std::to_string((int)prevSceneID_.size() + 1)] = pinID_next_;
    pinID_next_++;

    // フェードイン・フェードアウト
    inputID_["inScene"] = pinID_next_;
    pinID_next_++;
    inputID_["outScene"] = pinID_next_;
    pinID_next_++;
}

MainScene::~MainScene(){}

////////////////////////////////////////////////////////////////////
//                             描画
////////////////////////////////////////////////////////////////////

void MainScene::Draw(){

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

    // イベントシーンにつなぐピン
    Draw_Pin_EventScenes();

    // 終了
    EndNode();
}

////////////////////////////////////////////////////////////////////
//                          開始時処理
////////////////////////////////////////////////////////////////////

void MainScene::BeginNode(){

    // 色の設定
    ImNodes::PushColorStyle(ImNodesCol_TitleBar, nodeColor_);
    ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, nodeColor_);

    // タイトルバーの描画
    ImNodes::BeginNode(nodeID_);
    ImNodes::BeginNodeTitleBar();
    ImGui::TextUnformatted("Main Scene");
    ImNodes::EndNodeTitleBar();
}

////////////////////////////////////////////////////////////////////
//                    シーン名の入力フィールド
////////////////////////////////////////////////////////////////////

void MainScene::Draw_SceneSetting(){

    ImGui::Text("------------ Scene Setting -----------");

    // シーンの名前入力
    ImGui::Text("Scene Name");
    ImGui::SetNextItemWidth(200);
    ImGui::InputText("##Scene Name", name_, IM_ARRAYSIZE(name_));

    // 起動時立ち上げシーンかどうか
    ImGui::Dummy({ 0.0f,10.0f });
    ImGui::Checkbox("IsStartupScene?", &isStartupScene_);
}

////////////////////////////////////////////////////////////////////
//                     前のシーンにつなぐピン
////////////////////////////////////////////////////////////////////

void MainScene::Draw_Pin_PrevScene(){

    ImGui::Dummy({ 0.0f,10.0f });
    ImGui::Text("----------- Previous Scenes -----------");

    for(auto& prevScene : prevSceneID_){
        ImGui::Text(prevScene.first.c_str());
        ImNodes::BeginInputAttribute(prevScene.second);
        ImNodes::EndInputAttribute();
    }

    // ボタンが押されたらピンを増やす
    if(ImGui::Button("Add Pin##0")){
        prevSceneID_["prevScene_" + std::to_string((int)prevSceneID_.size() + 1)] = pinID_next_;
        pinID_next_++;
    };
}

////////////////////////////////////////////////////////////////////
//                     次のシーンにつなぐピン
////////////////////////////////////////////////////////////////////

void MainScene::Draw_Pin_NextScenes(){

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
//                      イベントシーンにつなぐピン
////////////////////////////////////////////////////////////////////

void MainScene::Draw_Pin_EventScenes(){

    ImGui::Dummy({ 0.0f,10.0f });
    ImGui::Text("------------- Event Scenes -------------");
    for(auto& eventScene : eventSceneID_){
        ImGui::Text(eventScene.first.c_str());
        ImNodes::BeginOutputAttribute(eventScene.second);
        ImNodes::EndOutputAttribute();
    }

    // ボタンが押されたらピンを増やす
    if(ImGui::Button("Add Pin##2")){
        eventSceneID_["eventScene_" + std::to_string((int)eventSceneID_.size() + 1)] = pinID_next_;
        pinID_next_++;
    };
}


////////////////////////////////////////////////////////////////////
//                   シーンの入り・終わりシーンのピン
////////////////////////////////////////////////////////////////////

void MainScene::Draw_Pin_InOutScene(){

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

void MainScene::EndNode(){
    ImNodes::EndNode();
    ImNodes::PopColorStyle();
    ImNodes::PopColorStyle();
}

