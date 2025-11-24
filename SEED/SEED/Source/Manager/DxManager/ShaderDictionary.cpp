#include "ShaderDictionary.h"

////////////////////////////////////////////////////////////////
// static変数の初期化
////////////////////////////////////////////////////////////////
ShaderDictionary* ShaderDictionary::instance_ = nullptr;

////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////

ShaderDictionary::~ShaderDictionary(){
}


////////////////////////////////////////////////////////////////
// インスタンスを取得する関数
////////////////////////////////////////////////////////////////

ShaderDictionary* ShaderDictionary::GetInstance(){
    if(instance_ == nullptr){
        instance_ = new ShaderDictionary();
    }
    return instance_;
}



////////////////////////////////////////////////////////////////
// 初期化
////////////////////////////////////////////////////////////////

void ShaderDictionary::Initialize(){
    // インスタンスの作成
    HRESULT hr;
    hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(dxcUtils.GetAddressOf()));
    assert(SUCCEEDED(hr));
    hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(dxcCompiler.GetAddressOf()));
    assert(SUCCEEDED(hr));

    // 現時点でincludeはしないが、 includeに対応するための設定を行っておく
    hr = dxcUtils->CreateDefaultIncludeHandler(includeHandler.GetAddressOf());
    assert(SUCCEEDED(hr));
}

////////////////////////////////////////////////////////////////
// 解放
////////////////////////////////////////////////////////////////
void ShaderDictionary::Release(){
    // 各シェーダーのBlobを解放
    for(auto& blob : blobs_){
        blob.second.Reset();
    }

    // 各シェーダーのリフレクションを解放
    for(auto& reflection : reflections_){
        reflection.second.Reset();
    }
}


////////////////////////////////////////////////////////////////
// ディレクトリ内のシェーダーを読み込む関数
////////////////////////////////////////////////////////////////

void ShaderDictionary::LoadFromDirectory(const std::string& directoryPath, ID3D12Device* device){

    // resources/textures/ParticleTextures/ 以下の階層にあるテクスチャを自動で読む
    auto fileNames = MyFunc::GetFileList(directoryPath,{ ".hlsl" });
    HRESULT hr;

    // シェーダーの種類に応じてコンパイル
    for(auto& fileName : fileNames){

        std::wstring shaderProfile = L"";
        std::wstring entryPoint = L"main";
        std::string strFileName = fileName.string();

        // Profile情報を設定
        if(strFileName.find(".VS") != std::string::npos){// VS
            shaderProfile = L"vs_6_0";

        } else if(strFileName.find(".PS") != std::string::npos){// PS
            shaderProfile = L"ps_6_0";

        } else if(strFileName.find(".MS") != std::string::npos){// MS
            shaderProfile = L"ms_6_5";

            // メッシュシェーダーに対応しているか確認
            D3D12_FEATURE_DATA_D3D12_OPTIONS7 features = {};
            hr = device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS7, &features, sizeof(features));
            assert(SUCCEEDED(hr));


        } else if(strFileName.find(".CS") != std::string::npos){// CS
            shaderProfile = L"cs_6_0";
            entryPoint = L"CSMain";

        } else if(strFileName.find(".AS") != std::string::npos){// AS
            shaderProfile = L"as_6_5";

        } else{
            assert(false && "Shader file name is not correct");
        }

        // 6.5に対応しているか確認
        if(shaderProfile == L"ms_6_5" || shaderProfile == L"as_6_5"){
            D3D12_FEATURE_DATA_SHADER_MODEL shaderModel = { D3D_SHADER_MODEL_6_5 };
            hr = device->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &shaderModel, sizeof(shaderModel));
            assert(SUCCEEDED(hr));
        }

        // コンパイルする
        blobs_[strFileName] = CompileShader(
            MyFunc::ConvertString(directoryPath + strFileName),
            shaderProfile.c_str(),
            entryPoint.c_str(),
            dxcUtils.Get(),
            dxcCompiler.Get(),
            includeHandler.Get()
        );
        assert(blobs_[strFileName] != nullptr);

        // DxcBuffer を作成
        DxcBuffer dxcBuffer = {};
        dxcBuffer.Ptr = blobs_[strFileName]->GetBufferPointer();
        dxcBuffer.Size = blobs_[strFileName]->GetBufferSize();
        dxcBuffer.Encoding = DXC_CP_ACP; // ANSI 文字コード（基本これでOK）

        // リフレクションを作成
        ID3D12ShaderReflection* reflection = nullptr;
        hr = dxcUtils->CreateReflection(
            &dxcBuffer,
            IID_PPV_ARGS(&reflection)
        );
        assert(SUCCEEDED(hr));
        reflections_[strFileName] = reflection;

    }
}



////////////////////////////////////////////////////////////////
// Blobを取得する関数
////////////////////////////////////////////////////////////////

IDxcBlob* ShaderDictionary::GetShaderBlob(const std::string& name){

    if(blobs_.find(name) != blobs_.end()){
        return blobs_[name].Get();
    }

    return nullptr;
}

////////////////////////////////////////////////////////////////
// リフレクションを取得する関数
////////////////////////////////////////////////////////////////

ID3D12ShaderReflection* ShaderDictionary::GetReflection(const std::string& name){

    if(reflections_.find(name) != reflections_.end()){
        return reflections_[name].Get();
    }

    return nullptr;
}
