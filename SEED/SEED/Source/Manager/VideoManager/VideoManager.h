#pragma once
#include <unordered_map>

struct VideoItems;

class VideoManager{
    friend class VideoPlayer;

private:
    // privateコンストラクタ
    VideoManager() = default;
    // コピー禁止
    VideoManager(const VideoManager&) = delete;
    void operator=(const VideoManager&) = delete;
    // インスタンス
    static inline VideoManager* instance_ = nullptr;

public:
    static VideoManager* GetInstance();
    void Update();
    void Release();
    void EndFrame();

private:
    void Register(VideoPlayer* video);

private:
    std::unordered_map<VideoPlayer*, VideoItems> videoItems_; // 動画ファイル名とそのリソースのマッピング
};