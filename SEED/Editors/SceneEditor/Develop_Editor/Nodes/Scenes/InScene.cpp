#include "InScene.h"

InScene::InScene(){
    // IDの割り当て
    nodeID_ = nodeID_next_;
    nodeID_next_++;

    // 名前・色・初期座標の設定
    nodeColor_ = IM_COL32(38, 168, 255, 255);
    position_ = { 580,300 };
    ImNodes::SetNodeGridSpacePos(nodeID_, position_);

    // ピンのID割り当て
    attachPinID_ = BaseNode::pinID_next_++;
}

////////////////////////////////////////////////////////////////////
//                             描画
////////////////////////////////////////////////////////////////////

void InScene::Draw(){

    // 開始時処理
    BeginNode();

    // ここから描画
    ImGui::Dummy({ 0.0f,10.0f });
    ImGui::Text("----------- Attach Scene -----------");

    ImGui::Text("AttachScene");
    ImNodes::BeginInputAttribute(attachPinID_);
    ImNodes::EndInputAttribute();


    ImGui::Dummy({ 0.0f,10.0f });
    ImGui::Text("----------- Attach Scene -----------");
    // プルダウンメニューを表示
    if(ImGui::Combo("Select option", &currentSelectType_, transitionTypeNames_, IM_ARRAYSIZE(transitionTypeNames_)))
    {

    }

    // 終了時処理
    EndNode();
}


////////////////////////////////////////////////////////////////////
//                         描画内の関数
////////////////////////////////////////////////////////////////////

void InScene::BeginNode(){
    // 色の設定
    ImNodes::PushColorStyle(ImNodesCol_TitleBar, nodeColor_);
    ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, nodeColor_);

    // タイトルバーの描画
    ImNodes::BeginNode(nodeID_);
    ImNodes::BeginNodeTitleBar();
    ImGui::TextUnformatted("Main Scene");
    ImNodes::EndNodeTitleBar();
}

void InScene::EndNode(){
    ImNodes::EndNode();
    ImNodes::PopColorStyle();
    ImNodes::PopColorStyle();
}
