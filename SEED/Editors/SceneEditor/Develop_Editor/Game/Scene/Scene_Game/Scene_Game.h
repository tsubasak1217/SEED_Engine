#pragma once
#include "Scene_Base.h"
#include "State_Base.h"

// external
#include <string>
#include <vector>
#include <memory>
#include <list>

// local
#include <Triangle.h>
#include <Model.h>
#include <State_Base.h>
#include <Sprite.h>

// Nodes
#include <BaseNode.h>
#include <NodeLink.h>
#include <MainScene.h>

class Scene_Game : public Scene_Base{

public:
    Scene_Game() = default;
    Scene_Game(SceneManager* pSceneManager);
    ~Scene_Game()override;
    void Initialize()override;
    void Finalize()override;
    void Update()override;
    void Draw()override;

private:

    void BeginEditor();
    void DrawLink();
    void UpdateLink();
    void EndEditor();
    void DisplayMenu();

private:
    std::vector<std::unique_ptr<BaseNode>>nodes_;
    std::vector<std::unique_ptr<NodeLink>>links_;
    int32_t hoveredLinkID_;
    ImNodesEditorContext* editorContext_;
};