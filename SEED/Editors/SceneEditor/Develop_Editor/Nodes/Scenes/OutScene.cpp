#include "OutScene.h"

OutScene::OutScene(){
    // IDの割り当て
    nodeID_ = nodeID_next_;
    nodeID_next_++;

    // 名前・色・初期座標の設定
    nodeColor_ = IM_COL32(255, 0, 38, 255);
    position_ = { 580,300 };
    ImNodes::SetNodeGridSpacePos(nodeID_, position_);

    // ピンのID割り当て
    attachPinID_ = BaseNode::pinID_next_++;
}

////////////////////////////////////////////////////////////////////
//                             描画
////////////////////////////////////////////////////////////////////

void OutScene::Draw(){

    // 開始時処理
    BeginNode();

    // ここから描画
    ImGui::Dummy({ 0.0f,10.0f });
    ImGui::Text("----------- Attach Scene -----------");


    ImGui::Text("AttachScene");
    ImNodes::BeginOutputAttribute(attachPinID_);
    ImNodes::EndOutputAttribute();


    ImGui::Dummy({ 0.0f,10.0f });
    ImGui::Text("-------- Transition Setting --------");

    // 遷移タイプの選択フィールド
    ImGui::SetNextItemWidth(100);
    if(ImGui::Combo("Transition Type", &currentSelectType_, transitionTypeNames_, IM_ARRAYSIZE(transitionTypeNames_)))
    {
        type_ = (TransitionType)currentSelectType_;
    }

    // 遷移時間の入力(秒)
    ImGui::Text("Transition Time");
    ImGui::SetNextItemWidth(50);
    ImGui::InputFloat("Time (sec)", &time_);

    if(currentSelectType_ == (int)TransitionType::FADE){
        ImGui::Text("Transition Color");
        ImGui::SetNextItemWidth(200);
        ImGui::ColorEdit4("", &transitionColor_.x);
    }

    // 終了時処理
    EndNode();
}


////////////////////////////////////////////////////////////////////
//                         描画内の関数
////////////////////////////////////////////////////////////////////

void OutScene::BeginNode(){
    // 色の設定
    ImNodes::PushColorStyle(ImNodesCol_TitleBar, nodeColor_);
    ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, nodeColor_);

    // タイトルバーの描画
    ImNodes::BeginNode(nodeID_);
    ImNodes::BeginNodeTitleBar();
    ImGui::TextUnformatted("Out Scene");
    ImNodes::EndNodeTitleBar();
}

void OutScene::EndNode(){
    ImNodes::EndNode();
    ImNodes::PopColorStyle();
    ImNodes::PopColorStyle();
}
