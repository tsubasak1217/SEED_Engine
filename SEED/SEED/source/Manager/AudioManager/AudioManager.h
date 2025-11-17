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

// stl
#include <fstream>
#include <cstdint>
#include <unordered_map>
// ComPtr
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;
// dict
#include <SEED/Source/Basic/Dictionary/AudioDictionary.h>


using AudioHandle = int32_t;


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


/// <summary>
/// 音声管理クラス
/// </summary>
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
    static void BeginFrame();
    static void StartUpLoad();
    HRESULT InitializeMediaFoundation();

public:// エンジンで利用できる関数

    static AudioHandle PlayAudio(const std::string& filename,bool loop,float volume = 1.0f,float time = 0.0f);
    static AudioHandle GetAudioHandle(const std::string& filename);
    static void EndAudio(AudioHandle handle);
    static void EndAllAudio();
    static void PauseAudio(AudioHandle handle);
    static void PauseAll();
    static void RestartAudio(AudioHandle handle);
    static void RestartAll();
    static void SetAudioVolume(AudioHandle handle, float volume);
    static bool IsPlayingAudio(AudioHandle handle);
    static bool IsPlayingAudio(const std::string& filename);
    static void LoadAudio(const std::string& filename);
    static void UnloadAudio(const std::string& filename);
    static void UnloadAllAudio();

private:
    AudioHandle PlayAudio(
        IXAudio2* xAudio2,const SoundData& soundData, 
        const std::string& filename, bool loop, float volume, float time
    );
    SoundData LoadWave(const char* filename);
    SoundData LoadMP3(const wchar_t* filename);
    SoundData LoadMP4(const wchar_t* filename);
    void UnloadAudio(SoundData* soundData);

private:
    static AudioManager* instance_;
    ComPtr<IXAudio2> xAudio2_;
    IXAudio2MasteringVoice* masteringVoice_;
    static float systemVolumeRate_;
    AudioHandle nextAudioHandle_ = 0;

private:
    std::unordered_map<std::string, SoundData>audios_;// データそのもの。複数鳴らしても1つでOK
    std::unordered_map<AudioHandle, IXAudio2SourceVoice*>sourceVoices_;// 同じ音源でも、鳴らす数だけ必要
    std::unordered_map<AudioHandle, bool>isPlaying_;
    std::unordered_map<AudioHandle, float>volumeMap_;
    std::unordered_map < AudioHandle, bool> isAlreadyPaused_;
    std::unordered_map<std::string, AudioHandle> filenameToHandle_;

private:
    static const std::string directoryPath_;
};