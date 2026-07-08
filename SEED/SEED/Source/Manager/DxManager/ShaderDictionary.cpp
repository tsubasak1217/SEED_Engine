#include "ShaderDictionary.h"
#include <SEED/Lib/Functions/FileFunc.h>
#include <SEED/Lib/Functions/ErrorLog.h>

namespace SEED{
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
        SEED_CHECK_RETURN(SUCCEEDED(hr), "ShaderDictionary::Initialize: DxcCreateInstance(DxcUtils) failed. dxcompiler.dll may be missing/corrupted.");
        hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(dxcCompiler.GetAddressOf()));
        SEED_CHECK_RETURN(SUCCEEDED(hr), "ShaderDictionary::Initialize: DxcCreateInstance(DxcCompiler) failed. dxcompiler.dll may be missing/corrupted.");

        // 現時点でincludeはしないが、 includeに対応するための設定を行っておく
        hr = dxcUtils->CreateDefaultIncludeHandler(includeHandler.GetAddressOf());
        SEED_CHECK_RETURN(SUCCEEDED(hr), "ShaderDictionary::Initialize: CreateDefaultIncludeHandler failed.");
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
        auto fileNames = Methods::File::GetFileList(directoryPath, { ".hlsl" });
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
                SEED_CHECK(SUCCEEDED(hr), "ShaderDictionary::LoadFromDirectory: CheckFeatureSupport(OPTIONS7) failed.");


            } else if(strFileName.find(".CS") != std::string::npos){// CS
                shaderProfile = L"cs_6_0";
                entryPoint = L"CSMain";

            } else if(strFileName.find(".AS") != std::string::npos){// AS
                shaderProfile = L"as_6_5";

            } else{
                SEED::Methods::LogCriticalError("ShaderDictionary::LoadFromDirectory: unrecognized shader file name \"" + strFileName + "\". Skipping.");
                continue;
            }

            // 6.5に対応しているか確認
            if(shaderProfile == L"ms_6_5" || shaderProfile == L"as_6_5"){
                D3D12_FEATURE_DATA_SHADER_MODEL shaderModel = { D3D_SHADER_MODEL_6_5 };
                hr = device->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &shaderModel, sizeof(shaderModel));
                SEED_CHECK(SUCCEEDED(hr), "ShaderDictionary::LoadFromDirectory: CheckFeatureSupport(SHADER_MODEL) failed.");
            }

            // コンパイルする
            blobs_[strFileName] = Methods::DxFunc::CompileShader(
                Methods::String::ConvertString(directoryPath + strFileName),
                shaderProfile.c_str(),
                entryPoint.c_str(),
                dxcUtils.Get(),
                dxcCompiler.Get(),
                includeHandler.Get()
            );
            if(blobs_[strFileName] == nullptr){
                // コンパイル失敗(シェーダーソース欠落・dxcompiler.dll不整合など) → このシェーダーはスキップして続行する
                SEED::Methods::LogCriticalError("ShaderDictionary::LoadFromDirectory: failed to compile shader \"" + strFileName + "\". This shader will be unavailable.");
                blobs_.erase(strFileName);
                continue;
            }

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
            SEED_CHECK(SUCCEEDED(hr), "ShaderDictionary::LoadFromDirectory: CreateReflection failed for \"" + strFileName + "\".");
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
}