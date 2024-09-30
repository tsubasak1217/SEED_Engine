#pragma once
#include "BaseNode.h"
#include "InScene.h"
#include "OutScene.h"

class EventScene : public BaseNode{

public:
    EventScene();
    ~EventScene();
    void Draw()override;

public:

    // 次のシーン・前のシーン
    BaseNode* pPrevScene_;
    uint32_t prevSceneID_;
    std::vector<BaseNode*> pNextScenes_;
    std::unordered_map<std::string, uint32_t>nextSceneID_;

    // シーンの始まり、終わり (フェードイン、アウトなど)
    std::pair<uint32_t, InScene*> inScene_;
    std::pair<uint32_t, OutScene*> outScene_;

private:
    void BeginNode();
    void Draw_SceneSetting();
    void Draw_Pin_PrevScene();
    void Draw_Pin_NextScenes();
    void Draw_Pin_InOutScene();
    void EndNode();
};