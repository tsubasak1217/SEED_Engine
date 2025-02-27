#pragma once
#include <Game/Objects/Judgement/PlayField.h>
#include <Game/Objects/Judgement/Judgement.h>
#include <Game/Objects/Notes/NotesData.h>

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
};
