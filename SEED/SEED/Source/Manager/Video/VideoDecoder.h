#pragma once
//stl
#include <deque>
// Media Foundation
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#pragma comment(lib, "Mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "Mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")

// DirectX用
#include <SEED/Lib/Functions/MyFunc/DxFunc.h>

// SEED
#include <SEED/Lib/Shapes/Quad.h>
#include <SEED/Lib/Functions/MyFunc/ShapeMath.h>

// file
#include <fstream>
// ComPtr
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;
using VideoHandle = uint32_t;

struct VideoFrame{
    ComPtr<ID3D12Resource> texture; // 描画用フレーム
    uint64_t timestamp = 0;
};

struct VideoData{
    std::wstring filePath;
    uint32_t width = 0;
    uint32_t height = 0;
    float frameRate = 0.0f;
    float duration = 0.0f;
    uint64_t frameCount = 0;
    GUID videoFormat;
};

struct Video{
public:
    void Update(){};
    void Draw(const Quad2D& quad){ quad; };
    void Draw(const Quad& quad){ quad; };

public:
    VideoData* videoData;
    float currentTime = 0.0f;
    bool isLoop = false;
    bool isPlaying = true;
    float playSpeedRate = 1.0f;

private:
    Microsoft::WRL::ComPtr<IMFSourceReader> sourceReader;
    ComPtr<ID3D12Resource> currentFrameTexture;
};


class VideoManager{
private:
    VideoManager() = default;
    VideoManager(const VideoManager&) = delete;
    void operator=(const VideoManager&) = delete;

public:
    ~VideoManager();
    static const VideoManager* GetInstance();
    static void Initialize();

public: // 操作用

    static void LoadVideo(const std::string& filename);

private:
    static inline VideoManager* instance_ = nullptr;
    std::unordered_map<std::string, VideoData> videoDatas_;// 映像そのもののデータ。1つのみ
};