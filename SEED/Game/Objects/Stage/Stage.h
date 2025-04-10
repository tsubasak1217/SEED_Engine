#pragma once
// stl
#include <memory>
#include <vector>
#include <cstdint>
#include <string>

// file
#include <json.hpp>
#include <iostream>

// GameObject
#include <Game/Objects/Stage/Blocks/Block_Base.h>

class Stage{
public:
    Stage();
    ~Stage();
    void Initialize();
    void Initialize(const std::string& filePath);
    void BeginFrame();
    void EndFrame();
    void Update();
    void Draw();
    void Reset();

public:
    void UpdateMatrix();
    void DrawStageArea();
    bool RequestPlayerMove(const Vector2i& address, Block_Base* pBlock);// プレイヤーが移動を要求
    void TryInsertBlock(const Vector2i& address, BlockType blockType);
    void DeleteBlock(const Vector2i& address);
    void CreateDoppelganger(const Vector2i address);// ドッペルゲンガーを生成

public:
    void LoadStage(const std::string& filePath);// ステージの読み込み
    void OutputStage();
    void ResizeStage();// ステージのリサイズ

private:
    void SolveStage();// ステージの状態の解決
    void MoveBlock(const Vector2i& from, const Vector2i& to);// ブロックを動かす

public:
    void SetIsCharacterMoved(bool isCharacterMoved){ isCharacterMoved_ = isCharacterMoved; }
    int32_t GetStageNo()const{ return stageNo_; }
    void SetStageNo(int32_t stageNo){ stageNo_ = stageNo; }
    Vector2i GetStageSize()const{ return stageSize_; }
    void SetStageSize(const Vector2i& stageSize){ stageSize_ = stageSize; }
    float GetCameraHeight()const{ return cameraHeight_; }
    void SetCameraHeight(float cameraHeight){ cameraHeight_ = cameraHeight; }

private:
    std::vector<std::unique_ptr<Block_Base>> blocks_;
    std::vector<std::vector<std::optional<std::pair<Block_Base*,size_t>>>> blockMap_;
    std::vector<std::vector<std::vector<Block_Base*>>> collisionMap_;
    std::vector<std::vector<std::optional<std::pair<Block_Base*, size_t>>>> preBlockMap_;
    int32_t stageNo_ = -1;
    Vector2i stageSize_ = { 10,10 };// ステージのサイズ
    float cameraHeight_ = 100.0f;
    bool isCharacterMoved_ = false;
    std::string filePath_;
};