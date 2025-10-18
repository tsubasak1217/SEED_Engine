#pragma once
// stl
#include <cstdlib>// std::system。コマンドライン実行のために必要
#include <string>
// windows
#include <Windows.h>
// .NET (C#をランタイムで動かす用)
#include "dotNet/nethost.h"
#include "dotNet/hostfxr.h"
#include "dotNet/coreclr_delegates.h"
#pragma comment(lib, "nethost.lib") 
// local
#include <SEED/Lib/Functions/MyFunc.h>
#include "FunctionTypeDefines.h"

struct HostFxrContext{
    hostfxr_handle handle;
    load_assembly_and_get_function_pointer_fn loadFunc;
};

class RuntimeScriptSystem{
private:
    RuntimeScriptSystem();
    inline static RuntimeScriptSystem* instance_ = nullptr;

public:
    ~RuntimeScriptSystem() = default;
    static RuntimeScriptSystem* GetInstance();
    void CompileScripts();
    HostFxrContext GetHostFxrContext(const std::wstring& runtimeConfigPath);

private:// 内部関数
    hostfxr_handle BootRuntime(const std::wstring& runtimeConfigPath);
    load_assembly_and_get_function_pointer_fn GetLoadFunction(hostfxr_handle handle);

private:
    // エンジンからの必要なファイルまでのパス
    std::wstring compilerDllPath_ = L"SEED/Source/Manager/RuntimeScriptSystem/sln/ScriptCompiler/bin/Debug/net9.0/ScriptCompiler.dll";
    std::wstring compilerConfigPath_ = L"SEED/Source/Manager/RuntimeScriptSystem/sln/ScriptCompiler/bin/Debug/net9.0/ScriptCompiler.runtimeconfig.json";
    std::wstring hostFxrDllPath_ = L"SEED/External/dotNet/hostfxr.dll";

    // .NET Core ホスト関数ポインタ
    hostfxr_initialize_for_runtime_config_fn initFxr = nullptr;
    hostfxr_get_runtime_delegate_fn getDelegate = nullptr;
    hostfxr_close_fn closeFxr = nullptr;


public:// template関数(見にくいので下に)

    // DLL から型付きの関数ポインタを取得する
    template<typename T>
    T GetFunction(HMODULE module, const char* name);
};


// dllから型付きの関数ポインタを取得する関数
template<typename T>
inline T RuntimeScriptSystem::GetFunction(HMODULE module, const char* name){
    FARPROC proc = GetProcAddress(module, name);
    if(!proc){
        throw std::runtime_error(std::string("関数が見つかりません: ") + name);
    }
    return reinterpret_cast<T>(proc);
}