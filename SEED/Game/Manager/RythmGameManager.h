#pragma once
#include <memory>
#include <Game/Objects/Judgement/PlayField.h>
#include <Game/Objects/Judgement/Judgement.h>
#include <Game/Objects/Notes/NotesData.h>
#include <SEED/Source/Object/Camera/BaseCamera.h>

class RythmGameManager{
private:
    RythmGameManager();
    RythmGameManager(const RythmGameManager&) = delete;
    void operator=(const RythmGameManager&) = delete;
    static RythmGameManager* instance_;

public:
    ~RythmGameManager();
    static RythmGameManager* GetInstance();
    void Update();
    void Draw();

public:
    BaseCamera* GetCamera(){ return gameCamera_.get(); }

private:
    std::unique_ptr<BaseCamera> gameCamera_;// カメラ
};
