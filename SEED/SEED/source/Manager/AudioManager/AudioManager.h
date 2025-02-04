#pragma once

// audio
#include <xaudio2.h>
#pragma comment(lib,"xaudio2.lib")

#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#pragma comment(lib, "Mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "Mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")

// file
#include <fstream>
// ComPtr
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;
// cint
#include <cstdint>
// map
#include <unordered_map>




// 読み込みに必要な構造体
struct ChunkHeader{
    char id[4];
    int32_t size;
};

struct RiffHeader{
    ChunkHeader chunk;
    char type[4];
};

struct FormatChunk{
    ChunkHeader chunk;
    WAVEFORMATEX fmt;
};

struct SoundData{
    // 波形データ
    WAVEFORMATEX wfex;
    // バッファの先頭アドレス
    BYTE* pBuffer;
    // バッファのサイズ
    uint32_t bufferSize;
};


class AudioManager{

private:
    
    // privateコンストラクタ
    AudioManager() = default;

    // コピー禁止
    AudioManager(const AudioManager&) = delete;
    void operator=(const AudioManager&) = delete;

public:

    ~AudioManager();
    static const AudioManager* GetInstance();

public:// 初期化に関する関数

    static void Initialize();
    static void StartUpLoad();
    HRESULT InitializeMediaFoundation();

public:// エンジンで利用できる関数

    static void PlayAudio(const std::string& filename,bool loop,float volume = 1.0f);
    static void EndAudio(const std::string& filename);
    static void PauseAudio(const std::string& filename);
    static void RestertAudio(const std::string& filename);
    static void SetAudioVolume(const std::string& filename, float volume);
    static bool IsPlayingAudio(const std::string& filename);
    static void LoadAudio(const std::string& filename);
    static void UnloadAudio(const std::string& filename);
    static void UnloadAllAudio();

private:
    void PlayAudio(
        IXAudio2* xAudio2,const SoundData& soundData, 
        const std::string& filename, bool loop, float volume
    );
    SoundData LoadWave(const char* filename);
    SoundData LoadMP3(const wchar_t* filename);
    void UnloadAudio(SoundData* soundData);

private:
    static AudioManager* instance_;
    ComPtr<IXAudio2> xAudio2_;
    IXAudio2MasteringVoice* masteringVoice_;
    static float systemVolumeRate_;
private:
    std::unordered_map<std::string, SoundData>audios_;
    std::unordered_map<std::string, IXAudio2SourceVoice*>sourceVoices_;
    std::unordered_map<std::string, bool>isPlaying_;

private:
    static const std::string directoryPath_;
};