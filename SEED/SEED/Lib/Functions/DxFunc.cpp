#include <SEED/Lib/Functions/DxFunc.h>
#include <windows.h>
#include <format>
#include <cassert>
#include <SEED/Source/Manager/DxManager/DxManager.h>


// ログを出力する関数--------------------------------------------------------------------------------------
void Log(const std::string& message){
    OutputDebugStringA(message.c_str());
}

void Log(const std::wstring& message){
    OutputDebugStringA(MyFunc::ConvertString(message).c_str());
}

// アライメントを合わせる関数
uint32_t Align(uint32_t size, uint32_t alignment){
    return (size + (alignment - 1)) & ~(alignment - 1);
};


//--------------------------------- HLSLをコンパイルする関数------------------------------------------//
ComPtr<IDxcBlob> CompileShader(
    const std::wstring& filePath,
    const wchar_t* profile,
    const wchar_t* entryPoint,
    IDxcUtils* dxcUtils,
    IDxcCompiler3* dxcCompiler,
    IDxcIncludeHandler* includeHandler
){

    // これからシェーダーをコンパイルする旨をログに出す
    Log(MyFunc::ConvertString(std::format(L"Begin CompileShader, path: {}, profile : {}\n", filePath, profile)));

    /*------------------------ ファイルを読み込む --------------------------*/

    // hlslファイルを読む
    ComPtr<IDxcBlobEncoding> shaderSource = nullptr;
    HRESULT hr = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);
    // 読めなかったら止める
    assert(SUCCEEDED(hr));
    // 読み込んだファイルの内容を設定する
    DxcBuffer shaderSourceBuffer{};
    shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
    shaderSourceBuffer.Size = shaderSource->GetBufferSize();
    shaderSourceBuffer.Encoding = DXC_CP_UTF8; // UTF8のコードであることを通知

    /*-------------------------- コンパイルする ----------------------------*/

    LPCWSTR arguments[] = {
    filePath.c_str(), // コンパイル対象のhlslファイル名
        L"-E", entryPoint, // エントリーポイントの指定。基本的にmain以外にはしない
        L"-T", profile, // ShaderProfileの設定
        L"-Zi", L"-Qembed_debug",// デバッグ用の情報を埋め込む
        L"-Od", //最適化を外しておく
        L"-Zpr", // メモリレイアウトは行優先
    };

    // 実際にShaderをコンパイルする
    ComPtr<IDxcResult> shaderResult = nullptr;
    hr = dxcCompiler->Compile(
        &shaderSourceBuffer,// 読み込んだファイル
        arguments,// コンパイルオプション
        _countof(arguments),// コンパイルオプションの数
        includeHandler,// includeが含まれた諸々
        IID_PPV_ARGS(&shaderResult) // コンパイル結果
    );

    // コンパイルエラーではなくdxcが起動できないなど致命的な状況
    assert(SUCCEEDED(hr));

    /*---------------------------- エラー確認 ------------------------------*/

    // 警告・エラーが出てたらログに出して止める
    ComPtr<IDxcBlobUtf8> shaderError = nullptr;
    shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
    if(shaderError != nullptr && shaderError->GetStringLength() != 0) {
        Log(shaderError->GetStringPointer());
        // 警告・エラーダメゼッタイ
        // エラーの内容
        const char* errorMessage = reinterpret_cast<const char*>(shaderError->GetBufferPointer());
        errorMessage;
        assert(false);
    }

    /*-------------------- コンパイル結果を受け取って返す ----------------------*/

    // コンパイル結果から実行用のバイナリ部分を取得
    ComPtr<IDxcBlob> shaderBlob = nullptr;
    hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
    assert(SUCCEEDED(hr));
    // 成功したログを出す
    Log(MyFunc::ConvertString(std::format(L"Compile Succeeded, path: {}, profile:{}\n", filePath, profile)));
    // もう使わないリソースを解放
    shaderSource->Release();
    shaderResult->Release();
    if(shaderError){
        shaderError->Release();
    }

    // 実行用のバイナリを返却
    return shaderBlob;

}

// ----------------------------------DescriptorHeap作成関数-----------------------------------------//

ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible){

    // ディスクリプタヒープとディスクリプターを格納する変数
    ComPtr<ID3D12DescriptorHeap> descriptorHeap = nullptr;
    D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};

    descriptorHeapDesc.Type = heapType; // ヒープタイプ設定
    descriptorHeapDesc.NumDescriptors = numDescriptors; // ディスクリプターの数を設定
    descriptorHeapDesc.Flags = shaderVisible ?// visibleかどうか
        D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    // ディスクリプタヒープを作成
    HRESULT hr;
    hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));

    // ディスクリプタヒープが作れなかったので起動できない
    assert(SUCCEEDED(hr));

    return descriptorHeap;
}

void CreateDescriptorHeap(ID3D12Device* device, ComPtr<ID3D12DescriptorHeap>& heap, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible){
    D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
    descriptorHeapDesc.Type = heapType;
    descriptorHeapDesc.NumDescriptors = numDescriptors;
    descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    HRESULT hr;
    hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&heap));
    assert(SUCCEEDED(hr));
}

//------------------------------テクスチャを読み込む関数---------------------------------------//

DirectX::ScratchImage LoadTextureImage(const std::string& filePath){

    DirectX::ScratchImage image{};
    DirectX::ScratchImage mipImages{};

    std::wstring filePathW = MyFunc::ConvertString(filePath);// wstring型に変換
    // ファイルを読み込む
    HRESULT hr = S_FALSE;
    
    if(!filePathW.ends_with(L".dds")){
        // 通常ファイルを読み込む  
        DirectX::LoadFromWICFile(
            filePathW.c_str(),
            DirectX::WIC_FLAGS_DEFAULT_SRGB,
            nullptr,
            image
        );
    } else{
        // DDSファイルを読み込む
        hr = DirectX::LoadFromDDSFile(
            filePathW.c_str(),
            DirectX::DDS_FLAGS_NONE,
            nullptr,
            image
        );
    }

    assert(SUCCEEDED(hr));

    /*-----------------------------*/
    // ミップマップの作成
    /*-----------------------------*/

    // 圧縮されている場合
    if(DirectX::IsCompressed(image.GetMetadata().format)){
        // 圧縮されている場合はミップマップを作成しない
        return image;
    }

    // 1x1のテクスチャはミップマップを作成しない
    if(image.GetMetadata().width > 1 && image.GetMetadata().height > 1) {
        hr = DirectX::GenerateMipMaps(
            image.GetImages(),
            image.GetImageCount(),
            image.GetMetadata(),
            DirectX::TEX_FILTER_SRGB,
            0,
            mipImages
        );
    } else{// サイズが1x1のときはここ
        return image;
    }

    assert(SUCCEEDED(hr));

    return mipImages;
}

// 埋め込みテクスチャを読み込む関数
DirectX::ScratchImage LoadEmbeddedTextureImage(const aiTexture* embeddedTexture){
    DirectX::ScratchImage image{};
    DirectX::ScratchImage mipImages{};
    HRESULT hr;

    if(embeddedTexture->mHeight == 0){
        // 圧縮されたフォーマットの場合
        std::string formatHint = embeddedTexture->achFormatHint;
        if(formatHint == "jpg" || formatHint == "jpeg" || formatHint == "png" || formatHint == "bmp" || formatHint == "gif"){
            hr = DirectX::LoadFromWICMemory(
                reinterpret_cast<const uint8_t*>(embeddedTexture->pcData),
                embeddedTexture->mWidth,
                DirectX::WIC_FLAGS_DEFAULT_SRGB,
                nullptr,
                image
            );
            assert(SUCCEEDED(hr));
        } else{
            assert(false && "Unsupported embedded texture format.");
        }
    } else{
        // Uncompressed RAW format
        hr = image.Initialize2D(
            DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
            embeddedTexture->mWidth,
            embeddedTexture->mHeight,
            1,
            1
        );
        assert(SUCCEEDED(hr));

        // ピクセルデータをコピー
        uint8_t* destPixels = image.GetPixels();
        memcpy(destPixels, embeddedTexture->pcData, image.GetPixelsSize());
    }


    // ミップマップの作成
    if(image.GetMetadata().width > 1 && image.GetMetadata().height > 1){
        hr = DirectX::GenerateMipMaps(
            image.GetImages(),
            image.GetImageCount(),
            image.GetMetadata(),
            DirectX::TEX_FILTER_SRGB,
            0,
            mipImages
        );
        assert(SUCCEEDED(hr));
        return mipImages;
    } else{
        return image;
    }
}

ComPtr<ID3D12Resource> InitializeTextureResource(ID3D12Device* device, uint32_t width, uint32_t height, DXGI_FORMAT format, DX_RESOURCE_STATE state){
    // GPUからコピーするリソースを作成
    ComPtr<ID3D12Resource> resource;
    D3D12_HEAP_PROPERTIES heapProperties{};
    heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

    D3D12_RESOURCE_FLAGS descFlag = D3D12_RESOURCE_FLAG_NONE;

    if(state == STATE_SHADER_RESOURCE){
        descFlag = D3D12_RESOURCE_FLAG_NONE;

    } else if(state == STATE_RENDER_TARGET){
        descFlag = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

    } else if(state == STATE_UNORDERED_ACCESS){
        descFlag = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
    }

    D3D12_RESOURCE_DESC textureDesc = {};
    textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    textureDesc.Alignment = 0;
    textureDesc.Width = width;  // 適切な幅
    textureDesc.Height = height;  // 適切な高さ
    textureDesc.DepthOrArraySize = 1;  // 深さまたは配列サイズは1以上
    textureDesc.MipLevels = 1;  // ミップマップレベル
    textureDesc.Format = format;// 画像の色の形式
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    textureDesc.Flags = descFlag;


    D3D12_RESOURCE_STATES resourceState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    if(state == STATE_SHADER_RESOURCE){
        resourceState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

    } else if(state == STATE_RENDER_TARGET){
        resourceState = D3D12_RESOURCE_STATE_RENDER_TARGET;

    } else if(state == STATE_UNORDERED_ACCESS){
        resourceState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
    }


    HRESULT hr;
    hr = device->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &textureDesc,
        resourceState,
        nullptr,
        IID_PPV_ARGS(&resource)
    );

    assert(SUCCEEDED(hr));

    return resource;
}

// ------------------------------リソース作成に関する関数---------------------------------------//

// バッファ用
ComPtr<ID3D12Resource> CreateBufferResource(
    ID3D12Device* device, 
    size_t sizeInBytes,
    D3D12_HEAP_TYPE heapLocation, 
    D3D12_RESOURCE_FLAGS resourceFlag,
    D3D12_RESOURCE_STATES initialState
){

    //頂点リソース用のヒープの設定
    D3D12_HEAP_PROPERTIES uploadHeapProperties{};
    uploadHeapProperties.Type = heapLocation;

    //頂点リソースの設定
    D3D12_RESOURCE_DESC bufferResourceDesc{};
    // バッファリソース。 テクスチャの場合はまた別の設定をする
    bufferResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    bufferResourceDesc.Width = sizeInBytes;// リソースのサイズ。
    // バッファの場合はこれらは1にする決まり
    bufferResourceDesc.Height = 1;
    bufferResourceDesc.DepthOrArraySize = 1;
    bufferResourceDesc.MipLevels = 1;
    bufferResourceDesc.SampleDesc.Count = 1;
    bufferResourceDesc.Flags = resourceFlag;
    // バッファの場合はこれにする決まり
    bufferResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    // リソースの作成
    ComPtr<ID3D12Resource> bufferResource = nullptr;
    HRESULT hr;

    assert(&uploadHeapProperties);
    assert(&bufferResourceDesc);

    hr = device->CreateCommittedResource(
        &uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
        &bufferResourceDesc, initialState, nullptr,
        IID_PPV_ARGS(&bufferResource)
    );

    assert(SUCCEEDED(hr));

    return bufferResource;
}


// テクスチャ用
ComPtr<ID3D12Resource> CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata){

    // metadataをもとにResourceの設定
    D3D12_RESOURCE_DESC resourceDesc{};
    resourceDesc.Width = UINT(metadata.width); // Texture幅
    resourceDesc.Height = UINT(metadata.height); // Texture高さ
    resourceDesc.MipLevels = UINT16(metadata.mipLevels); // mipmap
    resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize); // 奥行き or Textureの配列数
    resourceDesc.Format = metadata.format; // Texture Format
    resourceDesc.SampleDesc.Count = 1; // サンプリングカウント。1固定。
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);// Textureの次元数

    // 利用するHeapの設定
    D3D12_HEAP_PROPERTIES heapProperties{};
    heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT; // デフォルト


    // リソースの作成
    ComPtr<ID3D12Resource> resource = nullptr;
    HRESULT hr;
    hr = device->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&resource)
    );

    assert(SUCCEEDED(hr));

    return resource;
}


ComPtr<ID3D12Resource> CreateRenderTargetTextureResource(ID3D12Device* device, int32_t width, int32_t height){
    D3D12_RESOURCE_DESC resourceDesc{};
    resourceDesc.Width = width;// テクスチャの幅
    resourceDesc.Height = height;// テクスチャの高さ
    resourceDesc.MipLevels = 1;// mipmapの数
    resourceDesc.DepthOrArraySize = 1;// 奥行き or 配列Tecstureの配列数
    resourceDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;// 
    resourceDesc.SampleDesc.Count = 1;// サンプリングカウント。1固定
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;// 二次元
    resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;// RenderTargetとして使う

    // 利用するHeapの設定
    D3D12_HEAP_PROPERTIES heapProperties{};
    heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;//VRAM上に作る

    // Resourceの作成
    ComPtr<ID3D12Resource> resource = nullptr;
    HRESULT hr;
    hr = device->CreateCommittedResource(
        &heapProperties,// heapの設定
        D3D12_HEAP_FLAG_ALLOW_DISPLAY,// その他特殊な設定は特になし
        &resourceDesc,// Resourceの設定
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        nullptr,// クリア値
        IID_PPV_ARGS(&resource)// 作成するリソースポインタへのポインタ
    );

    assert(SUCCEEDED(hr));

    return resource;
}

// 深度ステンシルのリソースを作成する関数
ComPtr<ID3D12Resource> CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height){

    D3D12_RESOURCE_DESC resourceDesc{};
    resourceDesc.Width = width;// テクスチャの幅
    resourceDesc.Height = height;// テクスチャの高さ
    resourceDesc.MipLevels = 1;// mipmapの数
    resourceDesc.DepthOrArraySize = 1;// 奥行き or 配列Tecstureの配列数
    resourceDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;// DepthStencilとして利用可能なフォーマット
    resourceDesc.SampleDesc.Count = 1;// サンプリングカウント。1固定
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;// 二次元
    resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;// DepthStencilとして使う

    // 利用するHeapの設定
    D3D12_HEAP_PROPERTIES heapProperties{};
    heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;//VRAM上に作る

    // 深度値のクリア設定
    D3D12_CLEAR_VALUE depthClearValue{};
    depthClearValue.DepthStencil.Depth = 1.0f;// 最大値でクリア
    depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;// Resourceと合わせる

    // Resourceの作成
    ComPtr<ID3D12Resource> resource = nullptr;
    HRESULT hr;
    hr = device->CreateCommittedResource(
        &heapProperties,// heapの設定
        D3D12_HEAP_FLAG_NONE,// その他特殊な設定は特になし
        &resourceDesc,// Resourceの設定
        D3D12_RESOURCE_STATE_DEPTH_WRITE,// 深度値を書き込む状態にしておく
        &depthClearValue,// 深度のクリア値
        IID_PPV_ARGS(&resource)// 作成するリソースポインタへのポインタ
    );
    assert(SUCCEEDED(hr));

    return resource;
}


//データを転送するUploadTextureData関数
[[nodiscard]]
ComPtr<ID3D12Resource> UploadTextureData(
    ID3D12Resource* texture, const DirectX::ScratchImage& mipImages, ID3D12Device* device,
    ID3D12GraphicsCommandList* commandList
){

    std::vector<D3D12_SUBRESOURCE_DATA> subresources;
    DirectX::PrepareUpload(device, mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
    uint64_t intermediateSize = GetRequiredIntermediateSize(texture, 0, UINT(subresources.size()));
    ComPtr<ID3D12Resource> intermediateResource = CreateBufferResource(device, intermediateSize);
    UpdateSubresources(commandList, texture, intermediateResource.Get(), 0, 0, UINT(subresources.size()), subresources.data());

    //
    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = texture;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
    commandList->ResourceBarrier(1, &barrier);
    return intermediateResource;
}

// ディスクリプタのハンドルを取得する関数
D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index){
    D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
    descriptorHandle.ptr += (descriptorSize * index);
    return descriptorHandle;
}

D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index){
    D3D12_GPU_DESCRIPTOR_HANDLE descriptorHandle = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
    descriptorHandle.ptr += (descriptorSize * index);
    return descriptorHandle;
}

// Matrix4x4 を DirectX::XMMATRIX に変換する関数
DirectX::XMMATRIX ConvertToXMMATRIX(const Matrix4x4& matrix){
    return DirectX::XMMATRIX(
        matrix.m[0][0], matrix.m[0][1], matrix.m[0][2], matrix.m[0][3],
        matrix.m[1][0], matrix.m[1][1], matrix.m[1][2], matrix.m[1][3],
        matrix.m[2][0], matrix.m[2][1], matrix.m[2][2], matrix.m[2][3],
        matrix.m[3][0], matrix.m[3][1], matrix.m[3][2], matrix.m[3][3]
    );
}

void TransitionResourceState(
    ID3D12GraphicsCommandList* commandList,
    ID3D12Resource* resource,
    D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter
){
    // 同じなら処理しない
    if(stateAfter == stateBefore){ return; }

    // バリアを設定してリソースの状態を遷移させる
    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        resource,
        stateBefore,
        stateAfter
    );

    // リソースのstateを変更
    commandList->ResourceBarrier(1, &barrier);
}