#pragma once
#include <memory>
#include <array>
#include <string>
#include <SEED/Source/Basic/Object/GameObject.h>
#include <SEED/Source/Basic/Object/GameObject2D.h>
#include <SEED/Lib/Structs/VideoPlayer.h>
#include <SEED/Lib/Structs/Timer.h>
#include <SEED/Lib/Shapes/Quad.h>

class TutorialObjectManager{
public:
    TutorialObjectManager();
    TutorialObjectManager(const SEED::Timer* pSongPlayTimer);
    ~TutorialObjectManager();
    void Initialize();
    void Update();
    void Draw();
    void Finalize();

private:
    enum TutorialName{
        Tap = 0,
        Hold,
        Wheel,
        Frame,
        kTutorialCount
    };

    struct TutorialData{
        std::string tutorialVideoFilePath;
        float startTime;
        SEED::Timer tutorialTimer;
        SEED::Timer videoScalingTimer = SEED::Timer(1.5f);
        SEED::Topology::Quad2D videoQuad;
        std::unique_ptr<SEED::VideoPlayer> videoPlayer;
        SEED::GameObject2D* tutorialTitleObject = nullptr;
        SEED::GameObject2D* tutorialTextObject = nullptr;
        bool isPlaying = false;
        std::string text;
        std::string explainText;
        static inline float scalingStartTime = 1.0f;
    };

private:

    // 曲再生タイマーのポインタ
    const SEED::Timer* pSongPlayTimer_ = nullptr;

    // チュートリアルを管理する変数
    std::array<TutorialData, kTutorialCount> tutorials_;
};