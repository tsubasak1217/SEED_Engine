#include <SEED/Source/Manager/AudioManager/AudioManager.h>
#include <SEED/Source/Manager/InputManager/InputManager.h>
#include <SEED/Lib/Functions/MyFunc/DxFunc.h>
#include <Game/Manager/AudioDictionary.h>
#include <cassert>

// static変数初期化
AudioManager* AudioManager::instance_ = nullptr;
float AudioManager::systemVolumeRate_ = 1.f;

const std::string AudioManager::directoryPath_ = "resources/audios/";

// デストラクタ
AudioManager::~AudioManager(){
    if(instance_){
        delete instance_;
        instance_ = nullptr;
    }
}

// インスタンス取得関数
const AudioManager* AudioManager::GetInstance(){
    if(!instance_){
        instance_ = new AudioManager();
    }

    return instance_;
}

/////////////////////////////////////////////////////////////////////////////////////

/*                                 初期化・終了処理関数                               */

/////////////////////////////////////////////////////////////////////////////////////

void AudioManager::Initialize(){
    // インスタンスなければ作成
    GetInstance();

    // 初期化
    HRESULT hr;
    hr = XAudio2Create(&instance_->xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
    assert(SUCCEEDED(hr));

    hr = instance_->xAudio2_->CreateMasteringVoice(&instance_->masteringVoice_);
    assert(SUCCEEDED(hr));

    // mp3用の初期化
    //CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    hr = instance_->InitializeMediaFoundation();
    assert(SUCCEEDED(hr));

    StartUpLoad();
}



HRESULT AudioManager::InitializeMediaFoundation(){
    HRESULT hr = MFStartup(MF_VERSION);
    if(FAILED(hr)){
        return hr;
    }
    return S_OK;
}

/////////////////////////////////////////////////////////////////////////////////////
// 
//                              フレーム開始時の処理
// 
/////////////////////////////////////////////////////////////////////////////////////
void AudioManager::BeginFrame(){
    // システムボリュームの設定
    if(Input::IsPressKey(DIK_V)){

        if(Input::IsTriggerKey(DIK_F3)){// ボリュームを上げる
            systemVolumeRate_ += 0.1f;
        }

        if(Input::IsTriggerKey(DIK_F2)){// ボリュームを下げる
            systemVolumeRate_ -= 0.1f;
        }

        if(Input::IsTriggerKey(DIK_F1)){// ミュート/ミュート解除
            if(systemVolumeRate_ == 0.0f){
                systemVolumeRate_ = 1.0f;
            } else{
                systemVolumeRate_ = 0.0f;
            }
        }

        systemVolumeRate_ = (std::max)(systemVolumeRate_, 0.0f);

        // 流れている音声の音量を変更
        for(auto& [handle, sourceVoice] : instance_->sourceVoices_){
            if(sourceVoice != nullptr){
                sourceVoice->SetVolume(instance_->volumeMap_[handle] * systemVolumeRate_);
            }
        }
    }

    // 終了している音声の削除
    std::erase_if(instance_->sourceVoices_,
        [](auto& kv){
        XAUDIO2_VOICE_STATE state{};
        kv.second->GetState(&state);
        return state.BuffersQueued == 0;
    });
}


/////////////////////////////////////////////////////////////////////////////////////

/*                           起動時まとめてロードする関数                               */

/////////////////////////////////////////////////////////////////////////////////////

void AudioManager::StartUpLoad(){

    // マップに使用ファイルを登録
    AudioDictionary::Initialize();

    // 使用するファイルをすべて読み込む
    for(const auto& fileName : std::views::values(AudioDictionary::dict)){

        LoadAudio(fileName);
    }
}


/////////////////////////////////////////////////////////////////////////////////////

/*                                  音声再生関数                                    */

/////////////////////////////////////////////////////////////////////////////////////

AudioHandle AudioManager::PlayAudio(
    IXAudio2* xAudio2, const SoundData& soundData,
    const std::string& filename, bool loop, float volume, float time
){
    HRESULT hr;
    std::string tmpFilename = filename;

    // ソースボイスの作成
    hr = xAudio2->CreateSourceVoice(&sourceVoices_[nextAudioHandle_], &soundData.wfex);
    assert(SUCCEEDED(hr));

    // 秒 → サンプル位置へ変換
    uint32_t sampleRate = soundData.wfex.nSamplesPerSec;
    uint32_t channels = soundData.wfex.nChannels;
    uint32_t bitsPerSample = soundData.wfex.wBitsPerSample;

    uint32_t bytesPerSample = bitsPerSample / 8;
    uint32_t bytesPerFrame = bytesPerSample * channels;

    uint32_t totalSamples = soundData.bufferSize / bytesPerFrame;
    uint32_t sampleOffset = static_cast<uint32_t>(std::clamp(time, 0.0f, FLT_MAX) * sampleRate);

    // 安全な上限チェック：再生可能な最大位置にクランプ
    if(sampleOffset >= totalSamples){
        if(totalSamples == 0){
            // 音声が壊れてるなど → 再生不能
            sourceVoices_[nextAudioHandle_]->DestroyVoice();
            sourceVoices_.erase(nextAudioHandle_);
            assert(0);
        }
        sampleOffset = totalSamples - 1;
    }

    // 再生バッファの設定
    XAUDIO2_BUFFER buf{};
    buf.pAudioData = soundData.pBuffer;
    buf.AudioBytes = soundData.bufferSize;
    buf.Flags = XAUDIO2_END_OF_STREAM;

    // ここがポイント！
    buf.PlayBegin = sampleOffset; // ← 再生開始位置を設定（サンプル単位）

    if(loop){
        buf.LoopCount = XAUDIO2_LOOP_INFINITE;
    }

    // Submit & 再生
    hr = sourceVoices_[nextAudioHandle_]->SubmitSourceBuffer(&buf);
    assert(SUCCEEDED(hr));
    hr = sourceVoices_[nextAudioHandle_]->SetVolume(volume);
    volumeMap_[nextAudioHandle_] = volume;
    assert(SUCCEEDED(hr));
    hr = sourceVoices_[nextAudioHandle_]->Start();
    assert(SUCCEEDED(hr));

    return nextAudioHandle_++;
}

/// <summary>
/// 音声を再生する関数
/// </summary>
/// <param name="filename">ファイル名</param>
/// <param name="loop">ループ可否</param>
AudioHandle AudioManager::PlayAudio(const std::string& filename, bool loop, float volume, float time){
    // 指定要素がなければアサート
    if(instance_->audios_.find(filename) == instance_->audios_.end()){
        LoadAudio(filename);
    }

    // 再生
    AudioHandle handle = instance_->PlayAudio(instance_->xAudio2_.Get(), instance_->audios_[filename], filename, loop, volume, time);

    // 再生フラグを立てる
    instance_->isPlaying_[handle] = true;
    // ファイル名を記録
    instance_->filenameToHandle_[filename] = handle;


    return handle;
}

/// <summary>
/// 再生中の音声ハンドルをファイル名から取得する関数
/// </summary>
/// <param name="filename">ファイル名</param>
/// <param name="loop">ループ可否</param>
AudioHandle AudioManager::GetAudioHandle(const std::string& filename){

    if (instance_->filenameToHandle_.find(filename) == instance_->filenameToHandle_.end()) {
        return UINT32_MAX;
    }
    return instance_->filenameToHandle_[filename];
}

/////////////////////////////////////////////////////////////////////////////////////

/*                                  音声停止・再開関数                                    */

/////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// 音声の再生を終了する
/// </summary>
/// <param name="filename">ファイル名</param>
void AudioManager::EndAudio(AudioHandle handle){
    // 指定要素がなければアサート
    if(instance_->sourceVoices_.find(handle) == instance_->sourceVoices_.end()){
        return;
    };
    if(IsPlayingAudio(handle) == false){
        return;
    }

    // 停止、終了
    HRESULT hr;
    hr = instance_->sourceVoices_[handle]->Stop();
    hr = instance_->sourceVoices_[handle]->FlushSourceBuffers();
    instance_->sourceVoices_[handle]->DestroyVoice();

    // 要素の削除
    instance_->sourceVoices_.erase(handle);
    instance_->isPlaying_.erase(handle);
    instance_->volumeMap_.erase(handle);

    // ファイル名マッピングを削除
    for (auto it = instance_->filenameToHandle_.begin(); it != instance_->filenameToHandle_.end();) {
        if (it->second == handle) {
            it = instance_->filenameToHandle_.erase(it);
        } else {
            ++it;
        }
    }

    assert(SUCCEEDED(hr));
}

/// <summary>
/// すべての音声を終了する
/// </summary>
void AudioManager::EndAllAudio(){
    // すべての音声を停止

    for(auto& sourceVoice : instance_->sourceVoices_){
        HRESULT hr;
        hr = sourceVoice.second->Stop();
        hr = sourceVoice.second->FlushSourceBuffers();
        sourceVoice.second->DestroyVoice();
    }

    // 要素の削除
    instance_->sourceVoices_.clear();
    instance_->isPlaying_.clear();
    instance_->volumeMap_.clear();
}

/// <summary>
/// 再生中の音声を一時停止する
/// </summary>
/// <param name="filename">ファイル名</param>
void AudioManager::PauseAudio(AudioHandle handle){
    // 指定要素がなければ何もしない
    if(instance_->sourceVoices_.find(handle) == instance_->sourceVoices_.end()){ return; }
    // 既に停止中なら何もしない
    if(!instance_->isPlaying_[handle]){ return; }

    //　停止
    HRESULT hr;
    hr = instance_->sourceVoices_[handle]->Stop();
    assert(SUCCEEDED(hr));

    //再生フラグをおろす
    instance_->isPlaying_[handle] = false;
}

/// <summary>
/// すべての音声を一時停止する
/// </summary>
void AudioManager::PauseAll(){
    instance_->isAlreadyPaused_.clear(); // 一時停止済みフラグをクリア
    for(auto& item : instance_->sourceVoices_){
        if(instance_->isPlaying_[item.first]){ // 再生中の音声のみ一時停止
            PauseAudio(item.first);
        } else{
            instance_->isAlreadyPaused_[item.first] = true;
        }
    }
}

/// <summary>
/// 音声の再生を再開する
/// </summary>
/// <param name="filename">ファイル名</param>
void AudioManager::RestartAudio(AudioHandle handle){
    // 指定要素がなければ何もしない
    if(instance_->sourceVoices_.find(handle) == instance_->sourceVoices_.end()){ return; }
    // 既に再生中なら何もしない
    if(instance_->isPlaying_[handle]){ return; }

    // 再開
    if(instance_->sourceVoices_[handle] != nullptr){
        instance_->sourceVoices_[handle]->Start();
    }

    // 再生フラグを立てる
    instance_->isPlaying_[handle] = true;
}


void AudioManager::RestartAll(){
    for(auto& item : instance_->sourceVoices_){
        // 元々停止していた音声は再開しない
        if(instance_->isAlreadyPaused_.find(item.first) != instance_->isAlreadyPaused_.end()){
            continue;
        }

        // 再開
        RestartAudio(item.first);
    }
}


/// <summary>
/// 音声の音量を設定する。
/// </summary>
/// <param name="filename">ファイル名</param>
/// <param name="volume">音量。0.0: 無音  1.0: デフォルト。 1より大きくしすぎると音割れポッターになる</param>
void AudioManager::SetAudioVolume(AudioHandle handle, float volume){
    // 指定要素がなければアサート
    assert(instance_->sourceVoices_.find(handle) != instance_->sourceVoices_.end());
    // 設定
    instance_->sourceVoices_[handle]->SetVolume(volume * systemVolumeRate_);
    instance_->volumeMap_[handle] = volume;
}


/// <summary>
/// 音声が再生中かどうか取得
/// </summary>
/// <param name="filename">ファイル名</param>
/// <returns>音声が再生されているか</returns>
bool AudioManager::IsPlayingAudio(AudioHandle handle){
    // 指定要素がなければfalse
    if(instance_->isPlaying_.find(handle) == instance_->isPlaying_.end()){
        return false;
    }

    // 再生しているかどうか返す
    return instance_->isPlaying_[handle];
}

/// <summary>
/// 音声が再生中かどうか取得
/// </summary>
/// <param name="filename">ファイル名</param>
/// <returns>音声が再生されているか</returns>
bool AudioManager::IsPlayingAudio(const std::string& filename){

    if (instance_->filenameToHandle_.find(filename) == instance_->filenameToHandle_.end()) {
        return false;
    }
    AudioHandle handle = instance_->filenameToHandle_[filename];
    return IsPlayingAudio(handle);
}

/////////////////////////////////////////////////////////////////////////////////////

/*                                 音声ロード関数                                    */

/////////////////////////////////////////////////////////////////////////////////////

// WAV,MP3,M4Aファイルに対応。
void AudioManager::LoadAudio(const std::string& filename){
    // 要素がなければ
    if(instance_->audios_.find(filename) == instance_->audios_.end()){
        // 音をロードして追加
        std::string correctPath = directoryPath_ + filename;

        // 拡張子を取得
        size_t pos = filename.find_last_of('.');
        if(pos == std::string::npos || pos == filename.length() - 1){ assert(false); }
        std::string extention = filename.substr(pos + 1);

        // 拡張子に応じて処理
        if(extention == "wav"){
            instance_->audios_[filename] = instance_->LoadWave(correctPath.c_str());

        } else if(extention == "mp3" or extention == "m4a"){
            instance_->audios_[filename] = instance_->LoadMP3(MyFunc::ConvertString(correctPath).c_str());

        } else if(extention == "mp4"){
            instance_->audios_[filename] = instance_->LoadMP4(MyFunc::ConvertString(correctPath).c_str());

        } else{
            assert(false);
        }
    }
}

SoundData AudioManager::LoadWave(const char* filename){
    std::ifstream file;

    // ファイルをバイナリで開く
    file.open(filename, std::ios_base::binary);
    assert(file.is_open());

    // waveの読み込み
    RiffHeader riff;
    file.read((char*)&riff, sizeof(riff));

    // アサートチェック
    assert(strncmp(riff.chunk.id, "RIFF", 4) == 0);
    assert(strncmp(riff.type, "WAVE", 4) == 0);

    // フォーマットチャンクの読み込み
    FormatChunk format = {};
    while(true){

        ChunkHeader chunkHeader;

        file.read((char*)&chunkHeader, sizeof(ChunkHeader));
        if(strncmp(chunkHeader.id, "fmt ", 4) == 0){
            format.chunk = chunkHeader;
            file.read((char*)&format.fmt, format.chunk.size);
            assert(format.chunk.size <= sizeof(format.fmt));
            break;

        } else{
            file.seekg(chunkHeader.size, std::ios_base::cur);
        }

        // ファイルの終端に達した場合アサート
        if(file.eof()){
            assert(0 && "Reached end of file without finding 'fmt ' chunk");
            return SoundData{};
        }
    }

    // データチャンクの読み込み
    ChunkHeader data;
    while(true){
        file.read((char*)&data, sizeof(data));

        if(strncmp(data.id, "data", 4) == 0){
            break;
        } else{
            // 不要なチャンクをスキップ
            file.seekg(data.size, std::ios_base::cur);
        }

        // ファイルの終端に達した場合アサート
        if(file.eof()){
            assert(0 && "Reached end of file without finding 'fmt ' chunk");
            return SoundData{};
        }
    }

    // データ部の読み込み
    char* pCbuffer = new char[data.size];
    file.read(pCbuffer, data.size);

    // 閉じる
    file.close();

    // 読み込み結果を格納
    SoundData soundData{};
    soundData.wfex = format.fmt;
    soundData.pBuffer = reinterpret_cast<BYTE*>(pCbuffer);
    soundData.bufferSize = data.size;

    return soundData;
}


// MP3ファイルの読み込み
SoundData AudioManager::LoadMP3(const wchar_t* filename){

    HRESULT hr = InitializeMediaFoundation();
    if(FAILED(hr)){
        throw std::runtime_error("Media Foundation initialization failed.");
    }

    IMFSourceReader* pReader = nullptr;
    IMFMediaType* pOutputType = nullptr;

    // MP3ファイルのSource Readerを作成
    hr = MFCreateSourceReaderFromURL(filename, nullptr, &pReader);
    if(FAILED(hr)){
        MFShutdown();
        throw std::runtime_error("Failed to create Source Reader.");
    }

    // 出力タイプをPCM (WAV) に設定
    hr = MFCreateMediaType(&pOutputType);
    if(FAILED(hr)){
        pReader->Release();
        MFShutdown();
        throw std::runtime_error("Failed to create output media type.");
    }

    hr = pOutputType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
    hr = pOutputType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
    hr = pReader->SetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, pOutputType);

    if(FAILED(hr)){
        pOutputType->Release();
        pReader->Release();
        MFShutdown();
        throw std::runtime_error("Failed to set media type.");
    }

    pOutputType->Release();
    pOutputType = nullptr;
    pReader->GetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, &pOutputType);

    // サンプルを読み取ってデータを抽出
    std::vector<BYTE> audioData;

    while(true){
        IMFSample* pMFSample{ nullptr };
        DWORD dwStreamFlags{ 0 };
        pReader->ReadSample((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, nullptr, &dwStreamFlags, nullptr, &pMFSample);

        if(dwStreamFlags & MF_SOURCE_READERF_ENDOFSTREAM){
            break;
        }

        IMFMediaBuffer* pMFMediaBuffer{ nullptr };
        pMFSample->ConvertToContiguousBuffer(&pMFMediaBuffer);

        BYTE* pBuffer{ nullptr };
        DWORD cbCurrentLength{ 0 };
        pMFMediaBuffer->Lock(&pBuffer, nullptr, &cbCurrentLength);

        audioData.resize(audioData.size() + cbCurrentLength);
        memcpy(audioData.data() + audioData.size() - cbCurrentLength, pBuffer, cbCurrentLength);

        pMFMediaBuffer->Unlock();

        pMFMediaBuffer->Release();
        pMFSample->Release();
    }

    // SoundDataにデータを格納
    SoundData soundData;
    soundData.pBuffer = new BYTE[audioData.size()];
    std::copy(audioData.begin(), audioData.end(), soundData.pBuffer);
    soundData.bufferSize = static_cast<uint32_t>(audioData.size());

    // フォーマット情報を取得
    hr = pOutputType->GetUINT32(MF_MT_AUDIO_NUM_CHANNELS, (UINT32*)&soundData.wfex.nChannels);
    hr = pOutputType->GetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, (UINT32*)&soundData.wfex.nSamplesPerSec);
    hr = pOutputType->GetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, (UINT32*)&soundData.wfex.wBitsPerSample);

    soundData.wfex.wFormatTag = WAVE_FORMAT_PCM;
    soundData.wfex.nBlockAlign = soundData.wfex.nChannels * soundData.wfex.wBitsPerSample / 8;
    soundData.wfex.nAvgBytesPerSec = soundData.wfex.nSamplesPerSec * soundData.wfex.nBlockAlign;

    // 後始末
    pOutputType->Release();
    pReader->Release();
    MFShutdown();

    return soundData;
}

// mp4ファイルから音声を取り出して読み込む関数
SoundData AudioManager::LoadMP4(const wchar_t* filename){
    // MP4ファイルの読み込みはMedia Foundationを利用する
    HRESULT hr = InitializeMediaFoundation();
    if(FAILED(hr)){
        throw std::runtime_error("Media Foundation initialization failed.");
    }
    IMFSourceReader* pReader = nullptr;
    IMFMediaType* pOutputType = nullptr;
    // MP4ファイルのSource Readerを作成
    hr = MFCreateSourceReaderFromURL(filename, nullptr, &pReader);
    if(FAILED(hr)){
        MFShutdown();
        throw std::runtime_error("Failed to create Source Reader.");
    }
    // 出力タイプをPCM (WAV) に設定
    hr = MFCreateMediaType(&pOutputType);
    if(FAILED(hr)){
        pReader->Release();
        MFShutdown();
        throw std::runtime_error("Failed to create output media type.");
    }
    hr = pOutputType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
    hr = pOutputType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
    hr = pReader->SetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, pOutputType);
    if(FAILED(hr)){
        pOutputType->Release();
        pReader->Release();
        MFShutdown();
        throw std::runtime_error("Failed to set media type.");
    }
    pOutputType->Release();
    pOutputType = nullptr;
    pReader->GetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, &pOutputType);
    // サンプルを読み取ってデータを抽出
    std::vector<BYTE> audioData;
    while(true){
        IMFSample* pMFSample{ nullptr };
        DWORD dwStreamFlags{ 0 };
        pReader->ReadSample((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, nullptr, &dwStreamFlags, nullptr, &pMFSample);
        if(dwStreamFlags & MF_SOURCE_READERF_ENDOFSTREAM){
            break;
        }
        IMFMediaBuffer* pMFMediaBuffer{ nullptr };
        pMFSample->ConvertToContiguousBuffer(&pMFMediaBuffer);
        BYTE* pBuffer{ nullptr };
        DWORD cbCurrentLength{ 0 };
        pMFMediaBuffer->Lock(&pBuffer, nullptr, &cbCurrentLength);
        audioData.resize(audioData.size() + cbCurrentLength);
        memcpy(audioData.data() + audioData.size() - cbCurrentLength, pBuffer, cbCurrentLength);
        pMFMediaBuffer->Unlock();
        pMFMediaBuffer->Release();
        pMFSample->Release();
    }
    // SoundDataにデータを格納
    SoundData soundData;
    soundData.pBuffer = new BYTE[audioData.size()];
    std::copy(audioData.begin(), audioData.end(), soundData.pBuffer);
    soundData.bufferSize = static_cast<uint32_t>(audioData.size());
    // フォーマット情報を取得
    hr = pOutputType->GetUINT32(MF_MT_AUDIO_NUM_CHANNELS, (UINT32*)&soundData.wfex.nChannels);
    hr = pOutputType->GetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, (UINT32*)&soundData.wfex.nSamplesPerSec);
    hr = pOutputType->GetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, (UINT32*)&soundData.wfex.wBitsPerSample);
    soundData.wfex.wFormatTag = WAVE_FORMAT_PCM;
    soundData.wfex.nBlockAlign = soundData.wfex.nChannels * soundData.wfex.wBitsPerSample / 8;
    soundData.wfex.nAvgBytesPerSec = soundData.wfex.nSamplesPerSec * soundData.wfex.nBlockAlign;
    // 後始末
    pOutputType->Release();
    pReader->Release();
    MFShutdown();
    return soundData;
}

/////////////////////////////////////////////////////////////////////////////////////

/*                                  音声解放関数                                    */

/////////////////////////////////////////////////////////////////////////////////////

void AudioManager::UnloadAudio(const std::string& filename){
    // 指定要素がなければアサート
    assert(instance_->audios_.find(filename) != instance_->audios_.end());
    // 解放
    instance_->UnloadAudio(&instance_->audios_[filename]);
    instance_->audios_.erase(instance_->audios_.find(filename));
}

void AudioManager::UnloadAllAudio(){
    for(auto& audio : instance_->audios_){
        instance_->UnloadAudio(&audio.second);
    }

    instance_->audios_.clear();
    instance_->sourceVoices_.clear();
}


void AudioManager::UnloadAudio(SoundData* soundData){
    delete[] soundData->pBuffer;
    soundData->pBuffer = 0;
    soundData->bufferSize = 0;
    soundData->wfex = {};
}
