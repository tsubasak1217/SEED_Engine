#pragma once

// stl
#include <memory>
#include <vector>
#include <cstdint>
#include <string>
#include <unordered_set>

// Engine

// GameObject
#include <Game/Objects/Stage/Stage.h>

class StageManager{
private:
    // プライベートコンストラクタ
    StageManager() = default;

    // コピー、ムーブ禁止
    StageManager(const StageManager&) = delete;
    StageManager& operator=(const StageManager&) = delete;
    StageManager(StageManager&&) = delete;
    StageManager& operator=(StageManager&&) = delete;

    // instance
    static StageManager* instance_;

public:
    static StageManager* GetInstance();
    ~StageManager();
    void Initialize();
    void BeginFrame();
    void Update();
    void Draw();

public:
    void EditStage();
    void LoadStages();
    void OutputStage();
    void OutputStage(Stage* stage);
private:
    void LoadStageToEditor(const std::string& filePath);

private:
    void SetCameraPosition(Stage* stage);
    Vector2i SelectAddressByMouse();

public:
    int32_t GetCurrentStageNo()const{ return currentStageNo_; }
    int32_t GetMaxStageNo()const{ return kMaxStageNo_; }
    Stage* GetCurrentStage()const;
    float GetBlockSize()const{ return kBlockSize_; }

private:
    int32_t currentStageNo_;
    int32_t kMaxStageNo_;
    float kBlockSize_ = 10.0f;
    bool isEditMode_ = false;
    bool isTestPlay_ = false;
    std::vector<std::unique_ptr<Stage>> stages_;
    std::unique_ptr<Stage> editStage_;
    const std::string directoryPath_ = "Resources/jsons/Stages/";
    std::unordered_map<std::string,BlockType> blockNameMap_;
    std::vector<std::string> filePaths_;

};
