#include "Buttons.h"

/////////////////////////////////////////////////////////
// コンストラクタ
/////////////////////////////////////////////////////////

Buttons::Buttons(){

    // IDの割り当て
    nodeID_ = nodeID_next_;
    nodeID_next_++;

    // 名前・色・初期座標の設定
    nodeColor_ = IM_COL32(0, 188, 3, 255);
    position_ = { 580,300 };
    ImNodes::SetNodeGridSpacePos(nodeID_, position_);

    // ピンのID割り当て
    pin_in_ID_ = BaseNode::pinID_next_++;
    pin_out_ID_ = BaseNode::pinID_next_++;
}


/////////////////////////////////////////////////////////
// 描画
/////////////////////////////////////////////////////////

void Buttons::Draw(){

    // 開始時処理
    BeginNode();

    // ここから描画
    ImGui::Dummy({ 0.0f,10.0f });
    ImGui::Text("In/Out");
    ImNodes::BeginInputAttribute(pin_in_ID_);
    ImNodes::EndInputAttribute();
    ImNodes::BeginOutputAttribute(pin_out_ID_);
    ImNodes::EndOutputAttribute();


    ///============================ キーボード =============================//

    ImGui::Dummy({ 0.0f,5.0f });
    ImGui::Text("-------- Keyboard Buttons -------");

    // 現在登録したキーボードボタンの一覧
    ImGui::Dummy({ 0.0f,5.0f });
    for(auto& key : keyboardButtons_){
        ImGui::Text(key.second.c_str());
    }


    // 遷移ボタンの選択フィールド
    ImGui::Dummy({ 0.0f,5.0f });
    ImGui::SetNextItemWidth(100);
    if(ImGui::Combo("##Key Buttons", &currentSelectKeyName_, keyButtonNames, IM_ARRAYSIZE(keyButtonNames)))
    {
        keyboardButtons_[keyButtonNames[currentSelectKeyName_]] = keyButtonNames[currentSelectKeyName_];
    }

    ///============================ パッド =============================//

    ImGui::Dummy({ 0.0f,5.0f });
    ImGui::Text("---------- Pad Buttons ----------");

    // 現在登録したキーボードボタンの一覧
    ImGui::Dummy({ 0.0f,5.0f });
    for(auto& padButton : padButtons_){
        ImGui::Text(padButton.second.c_str());
    }


    // 遷移ボタンの選択フィールド
    ImGui::Dummy({ 0.0f,5.0f });
    ImGui::SetNextItemWidth(100);
    if(ImGui::Combo("##Pad Buttons", &currentSelectPadName_, padButtonNames, IM_ARRAYSIZE(padButtonNames)))
    {
        padButtons_[padButtonNames[currentSelectPadName_]] = padButtonNames[currentSelectPadName_];
    }


    ///============================ マウス =============================//

    ImGui::Dummy({ 0.0f,5.0f });
    ImGui::Text("--------- Mouse Buttons ---------");

    // 現在登録したキーボードボタンの一覧
    ImGui::Dummy({ 0.0f,5.0f });
    for(auto& mouseButton : mouseButtons_){
        ImGui::Text(mouseButton.second.c_str());
    }


    // 遷移ボタンの選択フィールド
    ImGui::Dummy({ 0.0f,5.0f });
    ImGui::SetNextItemWidth(100);
    if(ImGui::Combo("Mouse Buttons", &currentSelectMouseName_, mouseButtonNames, IM_ARRAYSIZE(mouseButtonNames)))
    {
        mouseButtons_[mouseButtonNames[currentSelectMouseName_]] = mouseButtonNames[currentSelectMouseName_];
    }


    // 終了時処理
    EndNode();
}

////////////////////////////////////////////////////////////////////
//                         描画内の関数
////////////////////////////////////////////////////////////////////

void Buttons::BeginNode(){
    // 色の設定
    ImNodes::PushColorStyle(ImNodesCol_TitleBar, nodeColor_);
    ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, nodeColor_);

    // タイトルバーの描画
    ImNodes::BeginNode(nodeID_);
    ImNodes::BeginNodeTitleBar();
    ImGui::TextUnformatted("Key Event");
    ImNodes::EndNodeTitleBar();
}

void Buttons::EndNode(){
    ImNodes::EndNode();
    ImNodes::PopColorStyle();
    ImNodes::PopColorStyle();
}



/////////////////////////////////////////////////////////
// ボタン名
/////////////////////////////////////////////////////////

const char* Buttons::keyButtonNames[144] = {
    "DIK_ESCAPE",
    "DIK_1",
    "DIK_2",
    "DIK_3",
    "DIK_4",
    "DIK_5",
    "DIK_6",
    "DIK_7",
    "DIK_8",
    "DIK_9",
    "DIK_0",
    "DIK_MINUS",
    "DIK_EQUALS",
    "DIK_BACK",
    "DIK_TAB",
    "DIK_Q",
    "DIK_W",
    "DIK_E",
    "DIK_R",
    "DIK_T",
    "DIK_Y",
    "DIK_U",
    "DIK_I",
    "DIK_O",
    "DIK_P",
    "DIK_LBRACKET",
    "DIK_RBRACKET",
    "DIK_RETURN",
    "DIK_LCONTROL",
    "DIK_A",
    "DIK_S",
    "DIK_D",
    "DIK_F",
    "DIK_G",
    "DIK_H",
    "DIK_J",
    "DIK_K",
    "DIK_L",
    "DIK_SEMICOLON",
    "DIK_APOSTROPHE",
    "DIK_GRAVE",
    "DIK_LSHIFT",
    "DIK_BACKSLASH",
    "DIK_Z",
    "DIK_X",
    "DIK_C",
    "DIK_V",
    "DIK_B",
    "DIK_N",
    "DIK_M",
    "DIK_COMMA",
    "DIK_PERIOD",
    "DIK_SLASH",
    "DIK_RSHIFT",
    "DIK_MULTIPLY",
    "DIK_LMENU",
    "DIK_SPACE",
    "DIK_CAPITAL",
    "DIK_F1",
    "DIK_F2",
    "DIK_F3",
    "DIK_F4",
    "DIK_F5",
    "DIK_F6",
    "DIK_F7",
    "DIK_F8",
    "DIK_F9",
    "DIK_F10",
    "DIK_NUMLOCK",
    "DIK_SCROLL",
    "DIK_NUMPAD7",
    "DIK_NUMPAD8",
    "DIK_NUMPAD9",
    "DIK_SUBTRACT",
    "DIK_NUMPAD4",
    "DIK_NUMPAD5",
    "DIK_NUMPAD6",
    "DIK_ADD",
    "DIK_NUMPAD1",
    "DIK_NUMPAD2",
    "DIK_NUMPAD3",
    "DIK_NUMPAD0",
    "DIK_DECIMAL",
    "DIK_OEM_102",
    "DIK_F11",
    "DIK_F12",
    "DIK_F13",
    "DIK_F14",
    "DIK_F15",
    "DIK_KANA",
    "DIK_ABNT_C1",
    "DIK_CONVERT",
    "DIK_NOCONVERT",
    "DIK_YEN",
    "DIK_ABNT_C2",
    "DIK_NUMPADEQUALS",
    "DIK_PREVTRACK",
    "DIK_AT",
    "DIK_COLON",
    "DIK_UNDERLINE",
    "DIK_KANJI",
    "DIK_STOP",
    "DIK_AX",
    "DIK_UNLABELED",
    "DIK_NEXTTRACK",
    "DIK_NUMPADENTER",
    "DIK_RCONTRO",
    "DIK_MUTE",
    "DIK_CALCULATOR",
    "DIK_PLAYPAUSE",
    "DIK_MEDIASTOP",
    "DIK_VOLUMEDOWN",
    "DIK_VOLUMEUP",
    "DIK_WEBHOME",
    "DIK_NUMPADCOMMA",
    "DIK_DIVIDE",
    "DIK_SYSRQ",
    "DIK_RMENU",
    "DIK_PAUSE",
    "DIK_HOME",
    "DIK_UP",
    "DIK_PRIOR",
    "DIK_LEFT",
    "DIK_RIGHT",
    "DIK_END",
    "DIK_DOWN",
    "DIK_NEXT",
    "DIK_INSERT",
    "DIK_DELETE",
    "DIK_LWIN",
    "DIK_RWIN",
    "DIK_APPS",
    "DIK_POWER",
    "DIK_SLEEP",
    "DIK_WAKE",
    "DIK_WEBSEARCH",
    "DIK_WEBFAVORITES",
    "DIK_WEBREFRESH",
    "DIK_WEBSTOP",
    "DIK_WEBFORWARD",
    "DIK_WEBBACK",
    "DIK_MYCOMPUTER",
    "DIK_MAIL",
    "DIK_MEDIASELECT",
};


// PAD
const char* Buttons::padButtonNames[16] = {
    "UP",
    "DOWN",
    "LEFT",
    "RIGHT",
    "START",
    "BACK",
    "L_STICK",
    "R_STICK",
    "LB",
    "RB",
    "A",
    "B",
    "X",
    "Y",
    "LT",
    "RT"
};


// mouse
const char* Buttons::mouseButtonNames[3] = {
    "L_CLICK",
    "R_CLICK",
    "WHEEL"
};