#include "Scene_Game.h"
#include <GameState_Play.h>
#include <GameState_Enter.h>
#include <SEED.h>

Scene_Game::Scene_Game(SceneManager* pSceneManager){
    pSceneManager_ = pSceneManager;
    ChangeState(new GameState_Play(this));
    Initialize();
};

Scene_Game::~Scene_Game()
{
}

void Scene_Game::Initialize()
{

}

void Scene_Game::Finalize()
{
}

void Scene_Game::Update()
{
    /*======================= 前フレームの値保存 ======================*/


    /*========================== ImGui =============================*/

#ifdef _DEBUG

    ImGui::ShowDemoWindow();

    ImNodes::BeginNodeEditor();

    // ノード1の描画
    ImNodes::BeginNode(1);
    ImGui::Text("Node 1");
    ImNodes::BeginInputAttribute(1); // 入力ピン
    ImGui::Text("Input");
    ImNodes::EndInputAttribute();
    ImNodes::BeginOutputAttribute(2); // 出力ピン
    ImGui::Text("Output");
    ImNodes::EndOutputAttribute();
    ImNodes::EndNode();

    // ノード2の描画
    ImNodes::BeginNode(2);
    ImGui::Text("Node 2");
    ImNodes::BeginInputAttribute(3); // 入力ピン
    ImGui::Text("Input");
    ImNodes::EndInputAttribute();
    ImNodes::BeginOutputAttribute(4); // 出力ピン
    ImGui::Text("Output");
    ImNodes::EndOutputAttribute();
    ImNodes::EndNode();

    // 接続の描画
    ImNodes::Link(1, 2, 4);  // ノード1の出力からノード2の入力に接続

    ImNodes::EndNodeEditor();

#endif

    /*========================= 各状態のの更新 ==========================*/

    currentState_->Update();

}

void Scene_Game::Draw()
{
    currentState_->Draw();
}