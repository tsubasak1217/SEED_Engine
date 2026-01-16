#pragma once
//stl
#include <deque>
#include <optional>
// Media Foundation
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#pragma comment(lib, "Mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "Mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")

// DirectX用
#include <SEED/Lib/Functions/DxFunc.h>

// SEED
#include <SEED/Source/Manager/AudioManager/AudioManager.h>
#include <SEED/Lib/Shapes/Quad.h>
#include <SEED/Lib/Functions/ShapeMath.h>
#include <SEED/Source/Manager/DxManager/DxResource.h>


// file
#include <fstream>
// ComPtr
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;
using VideoHandle = uint32_t;

namespace SEED{

    /// <summary>
    /// ビデオ情報を格納する構造体
    /// </summary>
    struct VideoData{
        std::string filePath;
        uint32_t widthY = 0;
        uint32_t heightY = 0;
        uint32_t widthUV = 0;
        uint32_t heightUV = 0;
        float frameRate = 0.0f;
        float duration = 0.0f;
        uint64_t frameCount = 0;
        GUID videoFormat;
    };

    /// <summary>
    /// フレーム情報
    /// </summary>
    struct Frame{
        double timePoint; // 表示時刻(秒)
        ComPtr<IMFSample> sample;
    };

    /// <summary>
    ///　動画プレイヤーの詳細情報
    /// </summary>
    struct VideoPlayerContext{
        float decodedTime = 0.0f; // デコード済みの時間
        float currentTime = 0.0f;
        float frameAccumulator = 0.0f; // フレームの累積時間
        bool isLoop = false;
        bool isPlaying = true;
        bool isAudioStarted = false; // 音声が開始されたかどうか
        bool isUseAudio = true; // 音声を再生するかどうか
        float playSpeedRate = 1.0f;
    };


    /// <summary>
    /// 動画用のバッファなどを保持する構造体
    /// </summary>
    struct VideoItems{
        bool removeOrder = false;
        ComPtr<IMFSourceReader> reader_;
        // Defaultヒープに設置するYUVテクスチャ
        DxResource frameTextureY_;
        DxResource frameTextureUV_;
        // CPU側からGPU側にアップロードするためのテクスチャ
        DxResource uploadTextureY_;
        DxResource uploadTextureUV_;
        // 最終結果結合用のテクスチャ
        DxResource frameTextureRGBA_;
        // 音声再生用のハンドル
        std::string audioFilePath;
        std::optional<AudioHandle> audioHandle;
    };


    /// <summary>
    /// 動画再生をするクラス
    /// </summary>
    class VideoPlayer{
        friend class DxManager;
        friend class VideoManager;

    public:
        VideoPlayer() = default;
        VideoPlayer(const std::string& filename, bool isUseAudio = true) : VideoPlayer(){
            LoadVideo(filename, isUseAudio);
        }
        ~VideoPlayer();

    public: // 基本関数
        void Draw(Topology::Quad2D quad);
        void Draw(Topology::Quad quad);
        void Unload();
        void LoadVideo(const std::string& filename, bool isUseAudio = true);
        void DrawGUI();


    public:// 再生制御用関数・アクセッサ
        void Play(float time = 0.0f, float speedRate = 1.0f, bool loop = false);
        void Pause();
        void Resume();
        Vector2 GetVideoSize() const;
        int32_t GetVideoGH() const;

    private:// VideoManagerが呼び出す内部関数
        void Update();
        void StartAudio();

    private:// 内部関数
        void CreateReader(ID3D12Device* pDevice, const std::string& filePath);
        void SetMediaFormat(GUID format);
        void GetVideoInfo();
        void SetStartPosition(float time);
        void CreateResources(ID3D12Device* pDevice);
        void CopyFrameToTextures(IMFSample* sample);
        void NV12ToRGBA();

    private:
        static inline std::string directory_ = "Resources/Videos/";
        VideoData videoData_;
        VideoPlayerContext context_;
        VideoItems* videoItem_ = nullptr;
    };

} // namespace SEED