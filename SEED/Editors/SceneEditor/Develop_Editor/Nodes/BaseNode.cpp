#include "BaseNode.h"

// static Initialize
uint32_t BaseNode::nodeID_next_ = 1;
uint32_t BaseNode::pinID_next_ = 1;

// コンストラクタ
BaseNode::BaseNode(){
    nodeID_ = nodeID_next_;
    nodeID_next_++;

    nodeColor_ = IM_COL32(5, 5, 5, 255);
    position_ = { 580,300 };

    outputID_["output"] = pinID_next_;
    pinID_next_++;
    inputID_["input"] = pinID_next_;
    pinID_next_++;

    ImNodes::SetNodeGridSpacePos(nodeID_, position_);
}

// 描画
void BaseNode::Draw(){

    // 色の設定
    ImNodes::PushColorStyle(ImNodesCol_TitleBar, nodeColor_);
    ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, nodeColor_);

    // タイトルバーの描画
    ImNodes::BeginNode(nodeID_);
    ImNodes::BeginNodeTitleBar();
    ImGui::TextUnformatted("Base Node");
    ImNodes::EndNodeTitleBar();

    // 入力ピンの描画
    for(auto& input : inputID_){
        ImGui::Text(input.first.c_str());
        ImNodes::BeginInputAttribute(input.second);
        ImNodes::EndInputAttribute();
    }

    // 出力ピンの描画
    for(auto& output : outputID_){
        ImGui::Text(output.first.c_str());
        ImNodes::BeginOutputAttribute(output.second);
        ImNodes::EndOutputAttribute();
    }

    // 終了
    ImNodes::EndNode();
    ImNodes::PopColorStyle();
    ImNodes::PopColorStyle();
}
