#pragma once
#include "InputManager.h"
#include "BaseNode.h"
#include "InScene.h"
#include "OutScene.h"
#include "EventScene.h"


class MainScene : public BaseNode{

public:

    MainScene();
    ~MainScene();
    void Draw()override;

public:

    // 次のシーン・前のシーン
    std::vector<BaseNode*> pPrevScenes_;
    std::unordered_map<std::string, uint32_t>prevSceneID_;
    std::vector<BaseNode*> pNextScenes_;
    std::unordered_map<std::string, uint32_t>nextSceneID_;

    // 上のシーン以外で外部から飛んでこれるシーン一覧
    std::vector<BaseNode*>pConnectScenes_;
    std::unordered_map<std::string, uint32_t>connectSceneID_;

    // シーンの始まり、終わり (フェードイン、アウトなど)
    std::pair<uint32_t,InScene*> inScene_;
    std::pair<uint32_t,OutScene*> outScene_;

    // シーン中に押されたキーに応じて起こるイベントシーン一覧 (pauseとか)
    std::vector<EventScene*>eventScenes_;
    std::unordered_map<std::string, uint32_t>eventSceneID_;

    // 起動時最初に立ち上がるシーンかどうか
    bool isStartupScene_ = false;

private:
    void BeginNode();
    void Draw_SceneSetting();
    void Draw_Pin_PrevScene();
    void Draw_Pin_NextScenes();
    void Draw_Pin_EventScenes();
    void Draw_Pin_InOutScene();
    void EndNode();
};