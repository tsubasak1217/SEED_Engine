#pragma once
// stl
#include <unordered_map>
#include <string>
// directX
#include <wrl/client.h>
#include <d3d12shader.h>
#include <d3dcompiler.h> 
using Microsoft::WRL::ComPtr;
//local
#include <SEED/Lib/Functions/DxFunc.h>

enum class ShaderType{
    VS = 0,
    MS,
    PS,
    AS,
    CS
};

/// <summary>
/// 読み込んだシェーダーの辞書を作成するクラス
/// </summary>
class ShaderDictionary{
private:
    ShaderDictionary() = default;
    static ShaderDictionary* instance_;
    // コピー・ムーブ禁止
    ShaderDictionary(const ShaderDictionary&) = delete;
    void operator=(const ShaderDictionary&) = delete;
    void operator=(ShaderDictionary&&) = delete;

public:
    // デストラクタ
    ~ShaderDictionary();
    /// インスタンスを取得
    static ShaderDictionary* GetInstance();
    // 初期化
    void Initialize();
    // 解放
    void Release();

private:
    // VertexShader
    std::unordered_map<std::string, ComPtr<IDxcBlob>> blobs_;
    std::unordered_map<std::string, ComPtr<ID3D12ShaderReflection>> reflections_;

    // dxcCompiler(HLSLをコンパイルするのに必要なもの)
    ComPtr<IDxcUtils> dxcUtils = nullptr;
    ComPtr<IDxcCompiler3> dxcCompiler = nullptr;
    ComPtr<IDxcIncludeHandler> includeHandler = nullptr;

public:
    // ディレクトリ内のシェーダーを読み込む関数
    void LoadFromDirectory(const std::string& directoryPath, ID3D12Device* device);

    // Blobを取得する関数
    IDxcBlob* GetShaderBlob(const std::string& name);
    // リフレクションを取得する関数
    ID3D12ShaderReflection* GetReflection(const std::string& name);
};