#include "VideoPlayer.h"
#include <SEED/Source/Manager/ClockManager/ClockManager.h>
#include <SEED/Source/Manager/ImGuiManager/ImGuiManager.h>
#include <SEED/Source/Manager/DxManager/DxManager.h>
#include <SEED/Source/Manager/VideoManager/VideoManager.h>
#include <SEED/Source/Manager/DxManager/PSO/PSOManager.h>
#include <SEED/Lib/Functions/DxFunc.h>
#include <SEED/Source/SEED.h>

// Media Foundation で実際の Plane サイズを取得する関数
static void GetNV12PlaneSizes(IMFMediaType* mediaType, UINT32* yWidth, UINT32* yHeight, UINT32* uvWidth, UINT32* uvHeight){
    UINT32 width = 0, height = 0;
    MFGetAttributeSize(mediaType, MF_MT_FRAME_SIZE, &width, &height);

    *yWidth = width;
    *yHeight = height;
    *uvWidth = (width + 1) / 2; // 偶数じゃない場合に備えて +1
    *uvHeight = (height + 1) / 2;
}

VideoPlayer::~VideoPlayer(){
    if(SEED::GetIsEndAplication()){ return; }
    if(videoItem_){
        videoItem_->removeOrder = true; // 削除フラグを立てる
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ビデオの読み込み
//////////////////////////////////////////////////////////////////////////////////////////////////
void VideoPlayer::LoadVideo(const std::string& filename, bool isUseAudio){
    auto* pDevice = DxManager::GetInstance()->GetDevice();
    videoData_.filePath = directory_ + filename;

    // マネージャに登録
    VideoManager::GetInstance()->Register(this);

    // 現在のリソースやビューを解放
    videoItem_ = &VideoManager::GetInstance()->videoItems_[this];

    if(videoItem_->frameTextureY_.GetSRVIndex() != -1){
        ViewManager::UnloadView(HEAP_TYPE::SRV_CBV_UAV, videoItem_->frameTextureY_.GetSRVIndex());
        ViewManager::UnloadView(HEAP_TYPE::SRV_CBV_UAV, videoItem_->frameTextureUV_.GetSRVIndex());
        ViewManager::UnloadView(HEAP_TYPE::SRV_CBV_UAV, videoItem_->frameTextureRGBA_.GetUAVIndex());
    }

    // Media Foundationの初期化
    HRESULT hr;
    hr = MFStartup(MF_VERSION);
    assert(SUCCEEDED(hr));

    // ソースリーダーの作成
    CreateReader(pDevice, videoData_.filePath);

    // 出力形式 NV12
    SetMediaFormat(MFVideoFormat_NV12);

    // メディア情報取得
    GetVideoInfo();

    // リソースの作成
    CreateResources(pDevice);

    // 再生コンテキスト
    context_.currentTime = 0.0f;
    context_.decodedTime = context_.currentTime;
    context_.frameAccumulator = 0.0f;
    context_.isLoop = false;
    context_.playSpeedRate = 1.0f;
    context_.isPlaying = false;
    context_.isUseAudio = isUseAudio;
    context_.isAudioStarted = false;

    // NV12からRGBAへ変換するシェーダーのパイプライン情報を初期化
    PSOManager::CreatePipelines("Video/NV12ToRGBA.CS.pip");

    // Audioの読み込み
    if(context_.isUseAudio){
        videoItem_->audioFilePath = "../Videos/" + filename;
        AudioManager::LoadAudio(videoItem_->audioFilePath);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 更新処理 (フレームレートに沿った安定再生)
//////////////////////////////////////////////////////////////////////////////////////////////////
void VideoPlayer::Update(){
    if(!context_.isPlaying || !videoItem_->reader_) return;

    // フレーム時間 (秒)
    float frameDuration = 1.0f / videoData_.frameRate;

    // DeltaTime を進める
    context_.frameAccumulator += ClockManager::DeltaTime() * context_.playSpeedRate;

    // フレームが進むべきタイミングで ReadSample
    while(context_.frameAccumulator >= frameDuration){

        DWORD streamIndex = 0;
        DWORD flags = 0;
        LONGLONG timestamp = 0;
        ComPtr<IMFSample> sample;

        HRESULT hr = videoItem_->reader_->ReadSample(
            (DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM,
            0,
            &streamIndex,
            &flags,
            &timestamp,
            &sample
        );

        // 終端処理
        if(flags & MF_SOURCE_READERF_ENDOFSTREAM){
            if(context_.isLoop){
                PROPVARIANT var;
                PropVariantInit(&var);
                var.vt = VT_I8;
                var.hVal.QuadPart = 0; // 先頭に戻す
                videoItem_->reader_->SetCurrentPosition(GUID_NULL, var);
                PropVariantClear(&var);

                context_.frameAccumulator = 0.0f;
            } else{
                context_.isPlaying = false;
            }
            return;
        }

        if(FAILED(hr)){ return; }
        if(!sample){ return; }

        // timestamp を秒に変換
        context_.decodedTime = static_cast<float>(timestamp) / 10000000.0f;

        // フレームをテクスチャにコピー
        CopyFrameToTextures(sample.Get());

        // フレーム進行分を減算
        context_.frameAccumulator -= frameDuration;
        context_.currentTime += frameDuration;
    }
}


//////////////////////////////////////////////////////////////////////////////////////////////////
// 再生・一時停止・解除
//////////////////////////////////////////////////////////////////////////////////////////////////
void VideoPlayer::Play(float time, float speedRate, bool loop){
    // 再生位置を設定
    SetStartPosition(time);
    context_.currentTime = std::clamp(time, 0.0f, videoData_.duration);
    context_.decodedTime = context_.currentTime;
    context_.playSpeedRate = speedRate;
    context_.isLoop = loop;
    context_.isPlaying = true;
}

void VideoPlayer::Pause(){
    context_.isPlaying = false;
    if(videoItem_->audioHandle.has_value()){
        AudioManager::PauseAudio(videoItem_->audioHandle.value());
    }
}

void VideoPlayer::Resume(){
    context_.isPlaying = true;
    if(videoItem_->audioHandle.has_value()){
        AudioManager::RestartAudio(videoItem_->audioHandle.value());
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 音声の開始
//////////////////////////////////////////////////////////////////////////////////////////////////
void VideoPlayer::StartAudio(){
    // Audioの再生
    if(videoItem_->audioHandle.has_value()){
        AudioManager::EndAudio(videoItem_->audioHandle.value()); // 既存の再生を終了
    }

    videoItem_->audioHandle = AudioManager::PlayAudio(videoItem_->audioFilePath, context_.isLoop, 0.5f, context_.currentTime);
    context_.isAudioStarted = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 描画処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void VideoPlayer::Draw(Quad2D quad){
    // NV12からRGBAへ変換
    NV12ToRGBA();
    // Quadのテクスチャを設定
    quad.GH = videoItem_->frameTextureRGBA_.GetSRVIndex();
    // 描画
    SEED::DrawQuad2D(quad);
}

void VideoPlayer::Draw(Quad quad){
    // NV12からRGBAへ変換
    NV12ToRGBA();
    // Quadのテクスチャを設定
    quad.GH = videoItem_->frameTextureRGBA_.GetSRVIndex();
    quad.lightingType = LIGHTINGTYPE_NONE; // ライティングを無効化
    // 描画
    SEED::DrawQuad(quad);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// VideoReaderの作成
//////////////////////////////////////////////////////////////////////////////////////////////////
void VideoPlayer::CreateReader(ID3D12Device* pDevice, const std::string& filePath){
    // DXGI Device Manager
    ComPtr<IMFDXGIDeviceManager> dxgiDeviceManager;
    UINT resetToken = 0;
    HRESULT hr = MFCreateDXGIDeviceManager(&resetToken, &dxgiDeviceManager);
    if(FAILED(hr)) throw std::runtime_error("MFCreateDXGIDeviceManager failed");

    hr = dxgiDeviceManager->ResetDevice(pDevice, resetToken);
    if(FAILED(hr)) throw std::runtime_error("ResetDevice failed");

    // Attributes
    ComPtr<IMFAttributes> attr;

    MFCreateAttributes(&attr, 1);
    attr->SetUINT32(MF_READWRITE_DISABLE_CONVERTERS, FALSE);
    attr->SetUINT32(MF_SOURCE_READER_DISABLE_DXVA, TRUE);  // ← GPUデコード無効化

    // SourceReader の作成はこれ一度だけ
    hr = MFCreateSourceReaderFromURL(
        MyFunc::ConvertString(filePath).c_str(),
        attr.Get(),
        &videoItem_->reader_
    );
    assert(SUCCEEDED(hr));
}


//////////////////////////////////////////////////////////////////////////////////////////////////
// メディアフォーマットの設定
//////////////////////////////////////////////////////////////////////////////////////////////////
void VideoPlayer::SetMediaFormat(GUID format){
    ComPtr<IMFMediaType> mediaTypeOut;
    HRESULT hr;
    hr = MFCreateMediaType(&mediaTypeOut);
    assert(SUCCEEDED(hr));
    mediaTypeOut->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
    mediaTypeOut->SetGUID(MF_MT_SUBTYPE, format);
    videoItem_->reader_->SetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, nullptr, mediaTypeOut.Get());
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ビデオ情報の取得
//////////////////////////////////////////////////////////////////////////////////////////////////
void VideoPlayer::GetVideoInfo(){

    HRESULT hr;
    ComPtr<IMFMediaType> mediaType;
    videoItem_->reader_->GetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, &mediaType);
    MFGetAttributeSize(mediaType.Get(), MF_MT_FRAME_SIZE, &videoData_.widthY, &videoData_.heightY);
    UINT32 num = 0, den = 0;
    if(SUCCEEDED(MFGetAttributeRatio(mediaType.Get(), MF_MT_FRAME_RATE, &num, &den)) && den != 0){
        videoData_.frameRate = float(num) / float(den);
    }


    // durationを取得(秒単位)
    PROPVARIANT var;
    PropVariantInit(&var);
    hr = videoItem_->reader_->GetPresentationAttribute(
        (DWORD)MF_SOURCE_READER_MEDIASOURCE,
        MF_PD_DURATION,
        &var
    );
    if(SUCCEEDED(hr) && var.vt == VT_UI8){
        // 100ns単位を秒に変換
        videoData_.duration = static_cast<float>(var.uhVal.QuadPart) / 10000000.0f;
    }
    PropVariantClear(&var);


    // サイズを取得
    UINT32 yWidth, yHeight, uvWidth, uvHeight;
    GetNV12PlaneSizes(mediaType.Get(), &yWidth, &yHeight, &uvWidth, &uvHeight);
    videoData_.widthY = yWidth;
    videoData_.heightY = yHeight;
    videoData_.widthUV = uvWidth;
    videoData_.heightUV = uvHeight;

}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 再生開始位置の設定
//////////////////////////////////////////////////////////////////////////////////////////////////
void VideoPlayer::SetStartPosition(float time){
    PROPVARIANT start;
    PropVariantInit(&start);
    start.vt = VT_I8;
    start.hVal.QuadPart = static_cast<LONGLONG>(time * 10000000.0f);
    videoItem_->reader_->SetCurrentPosition(GUID_NULL, start);
    PropVariantClear(&start);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// リソースの作成
//////////////////////////////////////////////////////////////////////////////////////////////////
void VideoPlayer::CreateResources(ID3D12Device* pDevice){

    // --- Y Plane ---
    {
        D3D12_RESOURCE_DESC desc = {};
        desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        desc.Width = videoData_.widthY;
        desc.Height = videoData_.heightY;
        desc.DepthOrArraySize = 1;
        desc.MipLevels = 1;
        desc.Format = DXGI_FORMAT_R8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

        CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);
        pDevice->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &desc,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
            nullptr,
            IID_PPV_ARGS(&videoItem_->frameTextureY_.resource)
        );

        videoItem_->frameTextureY_.srvDesc.Format = desc.Format;
        videoItem_->frameTextureY_.srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        videoItem_->frameTextureY_.srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        videoItem_->frameTextureY_.srvDesc.Texture2D.MipLevels = 1;
        videoItem_->frameTextureY_.srvDesc.Texture2D.PlaneSlice = 0;
        videoItem_->frameTextureY_.CreateSRV("VideoFrameYSRV");
        videoItem_->frameTextureY_.InitState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    }

    // Upload用のテクスチャのバッファを作成(YPlane)
    {
        D3D12_RESOURCE_DESC desc = {};
        desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        desc.Width = videoData_.widthY * videoData_.heightY + videoData_.widthUV * videoData_.heightUV; // YとUVのサイズを合わせる
        desc.Height = 1;
        desc.DepthOrArraySize = 1;
        desc.MipLevels = 1;
        desc.Format = DXGI_FORMAT_UNKNOWN; // バッファなのでフォーマットは不要
        desc.SampleDesc.Count = 1;
        desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        desc.Flags = D3D12_RESOURCE_FLAG_NONE;
        CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
        pDevice->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &desc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&videoItem_->uploadTextureY_.resource)
        );
    }

    // --- UV Plane ---
    {
        D3D12_RESOURCE_DESC desc = {};
        desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        desc.Width = videoData_.widthUV;
        desc.Height = videoData_.heightUV;
        desc.DepthOrArraySize = 1;
        desc.MipLevels = 1;
        desc.Format = DXGI_FORMAT_R8G8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

        CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);
        pDevice->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &desc,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
            nullptr,
            IID_PPV_ARGS(&videoItem_->frameTextureUV_.resource)
        );

        videoItem_->frameTextureUV_.srvDesc.Format = desc.Format;
        videoItem_->frameTextureUV_.srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        videoItem_->frameTextureUV_.srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        videoItem_->frameTextureUV_.srvDesc.Texture2D.MipLevels = 1;
        videoItem_->frameTextureUV_.srvDesc.Texture2D.PlaneSlice = 0;
        videoItem_->frameTextureUV_.CreateSRV("VideoFrameUVSRV");
        videoItem_->frameTextureUV_.InitState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    }

    // Upload用のテクスチャのバッファを作成(UVPlane)
    {
        D3D12_RESOURCE_DESC desc = {};
        desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        desc.Width = videoData_.widthY * videoData_.heightY + videoData_.widthUV * videoData_.heightUV; // UVのサイズ
        desc.Height = 1;
        desc.DepthOrArraySize = 1;
        desc.MipLevels = 1;
        desc.Format = DXGI_FORMAT_UNKNOWN; // バッファなのでフォーマットは不要
        desc.SampleDesc.Count = 1;
        desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        desc.Flags = D3D12_RESOURCE_FLAG_NONE;
        CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
        pDevice->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &desc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&videoItem_->uploadTextureUV_.resource)
        );
    }

    // RGBA Plane
    {
        D3D12_RESOURCE_DESC desc = {};
        desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        desc.Width = videoData_.widthY;
        desc.Height = videoData_.heightY;
        desc.DepthOrArraySize = 1;
        desc.MipLevels = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

        CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);
        pDevice->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &desc,
            D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
            nullptr,
            IID_PPV_ARGS(&videoItem_->frameTextureRGBA_.resource)
        );

        // UAVの設定
        videoItem_->frameTextureRGBA_.uavDesc.Format = desc.Format;
        videoItem_->frameTextureRGBA_.uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
        videoItem_->frameTextureRGBA_.uavDesc.Texture2D.MipSlice = 0;
        videoItem_->frameTextureRGBA_.CreateUAV("VideoFrameRGBATexture");

        // SRVの設定
        videoItem_->frameTextureRGBA_.srvDesc.Format = desc.Format;
        videoItem_->frameTextureRGBA_.srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        videoItem_->frameTextureRGBA_.srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        videoItem_->frameTextureRGBA_.srvDesc.Texture2D.MipLevels = 1;
        videoItem_->frameTextureRGBA_.srvDesc.Texture2D.PlaneSlice = 0;
        videoItem_->frameTextureRGBA_.CreateSRV("VideoFrameRGBASRV");

        // 初期状態を設定
        videoItem_->frameTextureRGBA_.InitState(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    }

}


//////////////////////////////////////////////////////////////////////////////////////////////////
// フレームをテクスチャにコピーする処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void VideoPlayer::CopyFrameToTextures(IMFSample* sample){
    // 連続バッファを取得
    ComPtr<IMFMediaBuffer> buffer;
    if(FAILED(sample->ConvertToContiguousBuffer(&buffer))) return;

    // IMF2DBuffer にキャスト
    ComPtr<IMF2DBuffer> buffer2D;
    if(FAILED(buffer.As(&buffer2D))) return;

    BYTE* pYPlane = nullptr;
    LONG yStride = 0;

    // Y plane をロックして stride を取得
    if(FAILED(buffer2D->Lock2D(&pYPlane, &yStride))) return;


    // UV plane の stride は通常 Y plane stride と同じ（Media Foundation 仕様）
    LONG uvStride = yStride;

    // 動画の縦サイズを16の倍数に切り上げる
    LONG allocatedHeightY = (videoData_.heightY + 15) / 16 * 16;

    // uvPlaneの開始位置を計算
    BYTE* pUVPlane = pYPlane + yStride * allocatedHeightY;

    // GPUコピー
    ID3D12GraphicsCommandList* cmdList = DxManager::instance_->commandList.Get();

    // --- Y Plane ---
    {
        D3D12_SUBRESOURCE_DATA subresource = {};
        subresource.pData = pYPlane;
        subresource.RowPitch = yStride;
        subresource.SlicePitch = yStride * videoData_.heightY;

        videoItem_->frameTextureY_.TransitionState(D3D12_RESOURCE_STATE_COPY_DEST);
        UpdateSubresources(cmdList,
            videoItem_->frameTextureY_.resource.Get(),
            videoItem_->uploadTextureY_.resource.Get(),
            0, 0, 1, &subresource);
        videoItem_->frameTextureY_.TransitionState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    }

    // --- UV Plane ---
    {
        UINT uvHeight = (videoData_.heightY + 1) / 2;
        //UINT uvWidthBytes = ((videoData_.widthY + 1) / 2) * 2; // R8G8_UNORMなので2バイト/ピクセル

        D3D12_SUBRESOURCE_DATA subresource = {};
        subresource.pData = pUVPlane;
        subresource.RowPitch = uvStride;
        subresource.SlicePitch = uvStride * uvHeight;

        videoItem_->frameTextureUV_.TransitionState(D3D12_RESOURCE_STATE_COPY_DEST);
        UpdateSubresources(cmdList,
            videoItem_->frameTextureUV_.resource.Get(),
            videoItem_->uploadTextureUV_.resource.Get(),
            0, 0, 1, &subresource);
        videoItem_->frameTextureUV_.TransitionState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    }

    buffer2D->Unlock2D();
}


//////////////////////////////////////////////////////////////////////////////////////////////////
// NV12からRGBAへの変換処理
//////////////////////////////////////////////////////////////////////////////////////////////////
void VideoPlayer::NV12ToRGBA(){

    // Resourceの状態を設定
    videoItem_->frameTextureY_.TransitionState(D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
    videoItem_->frameTextureUV_.TransitionState(D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
    videoItem_->frameTextureRGBA_.TransitionState(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

    // PSOの切り替え
    std::string pipelineName = "Video/NV12ToRGBA.CS.pip";
    auto* pso = PSOManager::GetPSO_Compute(pipelineName);
    auto& commandList = DxManager::GetInstance()->commandList;
    commandList->SetComputeRootSignature(pso->rootSignature.get()->rootSignature.Get());
    commandList->SetPipelineState(pso->pipeline.get()->pipeline_.Get());

    // リソースのバインド
    PSOManager::SetBindInfo("Video/NV12ToRGBA.CS.pip", "texY", videoItem_->frameTextureY_.GetSRVHandle());
    PSOManager::SetBindInfo("Video/NV12ToRGBA.CS.pip", "texUV", videoItem_->frameTextureUV_.GetSRVHandle());
    PSOManager::SetBindInfo("Video/NV12ToRGBA.CS.pip", "texRGBA", videoItem_->frameTextureRGBA_.GetUAVHandle());
    PSOManager::SetBindInfo("Video/NV12ToRGBA.CS.pip", "videoWidth", &videoData_.widthY);
    PSOManager::SetBindInfo("Video/NV12ToRGBA.CS.pip", "videoHeight", &videoData_.heightY);
    pso->rootSignature->BindAll(commandList.Get(), true);

    // 画面をグリッドに分割して、縦横それぞれのGroup数を計算
    UINT dispatchX = (videoData_.widthY + 15) / 16;
    UINT dispatchY = (videoData_.heightY + 15) / 16;

    // 実行
    commandList->Dispatch(dispatchX, dispatchY, 1);

    // リソースの状態を更新
    videoItem_->frameTextureY_.TransitionState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    videoItem_->frameTextureUV_.TransitionState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    videoItem_->frameTextureRGBA_.TransitionState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}


//////////////////////////////////////////////////////////////////////////////////////////////////
// 編集用の関数
//////////////////////////////////////////////////////////////////////////////////////////////////
void VideoPlayer::DrawGUI(){
    ImFunc::CustomBegin("Video Player", MoveOnly_TitleBar);
    {
        ImGui::Text("File: %s", videoData_.filePath.c_str());
        ImGui::Text("Frame Rate: %.2f FPS", videoData_.frameRate);
        ImGui::Text("Duration: %.2f seconds", videoData_.duration);
        ImGui::Text("Current Time: %.2f seconds", context_.currentTime);
        ImGui::Text("Decoded Time: %.2f seconds", context_.decodedTime);
        if(ImGui::Button(context_.isPlaying ? "Pause" : "Play")){
            context_.isPlaying = !context_.isPlaying;
        }

        ImGui::SliderFloat("Play Speed", &context_.playSpeedRate, 0.1f, 4.0f);

        ImGui::End();
    }
}