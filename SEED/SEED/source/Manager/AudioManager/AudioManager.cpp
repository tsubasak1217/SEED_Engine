#include <SEED/Source/Manager/AudioManager/AudioManager.h>
#include <SEED/Source/Manager/InputManager/InputManager.h>
#include <SEED/Lib/Functions/MyFunc/DxFunc.h>
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
    if(FAILED(hr)) {
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
        for(auto& [filename, sourceVoice] : instance_->sourceVoices_){
            if(sourceVoice != nullptr){
                sourceVoice->SetVolume(instance_->volumeMap_[filename] * systemVolumeRate_);
            }
        }
    }
}


/////////////////////////////////////////////////////////////////////////////////////

/*                           起動時まとめてロードする関数                               */

/////////////////////////////////////////////////////////////////////////////////////

void AudioManager::StartUpLoad(){
    // 以下にロード処理をまとめる

    //===================== BGM =====================//
    AudioManager::LoadAudio("BGM/Title.wav");


    //===================== SE =====================//
    LoadAudio("SE/dinosaur_jump.wav");
    LoadAudio("SE/dinosaur_eat.wav");
    LoadAudio("SE/dinosaur_grow.wav");
    LoadAudio("SE/dinosaur_born.wav");
    LoadAudio("SE/goal.wav");
    LoadAudio("SE/door.wav");
    LoadAudio("SE/laver.wav");
    LoadAudio("SE/star.wav");
    LoadAudio("SE/star_shine.wav");
    LoadAudio("SE/switch.wav");
    LoadAudio("SE/walk.mp3");
    LoadAudio("SE/select.mp3");
    LoadAudio("SE/stageSelect.mp3");
    LoadAudio("SE/hit.mp3");
}


/////////////////////////////////////////////////////////////////////////////////////

/*                                  音声再生関数                                    */

/////////////////////////////////////////////////////////////////////////////////////

void AudioManager::PlayAudio(
    IXAudio2* xAudio2, const SoundData& soundData,
    const std::string& filename, bool loop, float volume
){
    HRESULT hr;
    std::string tmpFilename = filename;

    // CreateSourceVoiceを呼び出す前に、既存のSourceVoiceを解放する
    if(sourceVoices_[filename] != nullptr) {
        sourceVoices_[filename]->DestroyVoice();
    }

    // ソースボイスの作成
    if(loop){
        hr = xAudio2->CreateSourceVoice(&sourceVoices_[filename], &soundData.wfex);
    } else{
        // 同じファイル名がある場合は連番をつける
        int count = 0;
        while(sourceVoices_.find(tmpFilename) != sourceVoices_.end()){
            count++;
            tmpFilename = filename + std::to_string(count);
        }

        hr = xAudio2->CreateSourceVoice(&sourceVoices_[tmpFilename], &soundData.wfex);
    }

    assert(SUCCEEDED(hr));

    // 再生する波形データの設定
    XAUDIO2_BUFFER buf{};
    buf.pAudioData = soundData.pBuffer;
    buf.AudioBytes = soundData.bufferSize;
    buf.Flags = XAUDIO2_END_OF_STREAM;
    if(loop){
        buf.LoopCount = XAUDIO2_LOOP_INFINITE;
    }

    // 再生
    hr = sourceVoices_[tmpFilename]->SubmitSourceBuffer(&buf);
    assert(SUCCEEDED(hr));
    hr = sourceVoices_[tmpFilename]->SetVolume(volume);
    assert(SUCCEEDED(hr));
    hr = sourceVoices_[tmpFilename]->Start();
    assert(SUCCEEDED(hr));
}

/// <summary>
/// 音声を再生する関数
/// </summary>
/// <param name="filename">ファイル名</param>
/// <param name="loop">ループ可否</param>
void AudioManager::PlayAudio(const std::string& filename, bool loop, float volume){
    // 指定要素がなければアサート
    assert(instance_->audios_.find(filename) != instance_->audios_.end());

    // 再生
    instance_->PlayAudio(instance_->xAudio2_.Get(), instance_->audios_[filename], filename, loop, volume);

    // 再生フラグを立てる
    instance_->isPlaying_[filename] = true;
}


/////////////////////////////////////////////////////////////////////////////////////

/*                                  音声停止・再開関数                                    */

/////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// 音声の再生を終了する
/// </summary>
/// <param name="filename">ファイル名</param>
void AudioManager::EndAudio(const std::string& filename){
    // 指定要素がなければアサート
    assert(instance_->sourceVoices_.find(filename) != instance_->sourceVoices_.end());
    if(IsPlayingAudio(filename) == false){
        return;
    }

    // 停止、終了
    HRESULT hr;
    hr = instance_->sourceVoices_[filename]->Stop();
    hr = instance_->sourceVoices_[filename]->FlushSourceBuffers();
    instance_->sourceVoices_[filename]->DestroyVoice();
    instance_->sourceVoices_[filename] = nullptr;
    instance_->isPlaying_[filename] = false;

    assert(SUCCEEDED(hr));
}

/// <summary>
/// 再生中の音声を一時停止する
/// </summary>
/// <param name="filename">ファイル名</param>
void AudioManager::PauseAudio(const std::string& filename){
    // 指定要素がなければアサート
    assert(instance_->sourceVoices_.find(filename) != instance_->sourceVoices_.end());
    //　停止
    HRESULT hr;
    hr = instance_->sourceVoices_[filename]->Stop();
    assert(SUCCEEDED(hr));

    //再生フラグをおろす
    instance_->isPlaying_[filename] = false;
}


/// <summary>
/// 音声の再生を再開する
/// </summary>
/// <param name="filename">ファイル名</param>
void AudioManager::RestertAudio(const std::string& filename){
    // 指定要素がなければアサート
    assert(instance_->sourceVoices_.find(filename) != instance_->sourceVoices_.end());

    // 再開
    if(instance_->sourceVoices_[filename] != nullptr) {
        instance_->sourceVoices_[filename]->Start();
    }

    // 再生フラグを立てる
    instance_->isPlaying_[filename] = true;
}


/// <summary>
/// 音声の音量を設定する。
/// </summary>
/// <param name="filename">ファイル名</param>
/// <param name="volume">音量。0.0: 無音  1.0: デフォルト。 1より大きくしすぎると音割れポッターになる</param>
void AudioManager::SetAudioVolume(const std::string& filename, float volume){
    // 指定要素がなければアサート
    assert(instance_->sourceVoices_.find(filename) != instance_->sourceVoices_.end());
    // 設定
    instance_->sourceVoices_[filename]->SetVolume(volume * systemVolumeRate_);
    instance_->volumeMap_[filename] = volume;
}


/// <summary>
/// 音声が再生中かどうか取得
/// </summary>
/// <param name="filename">ファイル名</param>
/// <returns>音声が再生されているか</returns>
bool AudioManager::IsPlayingAudio(const std::string& filename){
    // 指定要素がなければアサート
    assert(instance_->isPlaying_.find(filename) != instance_->isPlaying_.end());

    // 再生しているかどうか返す
    return instance_->isPlaying_[filename];
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
        if(pos == std::string::npos || pos == filename.length() - 1) { assert(false); }
        std::string extention = filename.substr(pos + 1);

        // 拡張子に応じて処理
        if(extention == "wav"){
            instance_->audios_[filename] = instance_->LoadWave(correctPath.c_str());

        } else if(extention == "mp3" or extention == "m4a"){
            instance_->audios_[filename] = instance_->LoadMP3(ConvertString(correctPath).c_str());

        } else{
            assert(false);
        }

        instance_->isPlaying_[filename] = false;
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
        if(file.eof()) {
            assert(0 && "Reached end of file without finding 'fmt ' chunk");
            return SoundData{};
        }
    }

    // データチャンクの読み込み
    ChunkHeader data;
    while(true) {
        file.read((char*)&data, sizeof(data));

        if(strncmp(data.id, "data", 4) == 0) {
            break;
        } else {
            // 不要なチャンクをスキップ
            file.seekg(data.size, std::ios_base::cur);
        }

        // ファイルの終端に達した場合アサート
        if(file.eof()) {
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
    if(FAILED(hr)) {
        throw std::runtime_error("Media Foundation initialization failed.");
    }

    IMFSourceReader* pReader = nullptr;
    IMFMediaType* pOutputType = nullptr;

    // MP3ファイルのSource Readerを作成
    hr = MFCreateSourceReaderFromURL(filename, nullptr, &pReader);
    if(FAILED(hr)) {
        MFShutdown();
        throw std::runtime_error("Failed to create Source Reader.");
    }

    // 出力タイプをPCM (WAV) に設定
    hr = MFCreateMediaType(&pOutputType);
    if(FAILED(hr)) {
        pReader->Release();
        MFShutdown();
        throw std::runtime_error("Failed to create output media type.");
    }

    hr = pOutputType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
    hr = pOutputType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
    hr = pReader->SetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, pOutputType);

    if(FAILED(hr)) {
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

    while(true)
    {
        IMFSample* pMFSample{ nullptr };
        DWORD dwStreamFlags{ 0 };
        pReader->ReadSample((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, nullptr, &dwStreamFlags, nullptr, &pMFSample);

        if(dwStreamFlags & MF_SOURCE_READERF_ENDOFSTREAM)
        {
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
    instance_->sourceVoices_[filename]->DestroyVoice();
    instance_->sourceVoices_.erase(instance_->sourceVoices_.find(filename));
    instance_->isPlaying_.erase(instance_->isPlaying_.find(filename));
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
