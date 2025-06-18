#include "RuntimeScriptSystem.h"

// .NETでC#関数を.dllから呼び出すまでの大まかな手順メモ
// 
// 1. hostfxr.dllをロードして必要な基本の関数(初期化、読み込み、終了)を取得
// 2. 読み込みたい任意のランタイムのconfigファイルを指定し、その情報を元に.NETランタイムを起動
// 3. 起動したランタイムハンドルを使って、dllの関数を読み込むための関数を作成しポインタを取得
// 4. 取得した関数ポインタから関数を使って、dll内の関数を取得
// 5. 取得した関数を実行
// 6. 実行が終わったら、ランタイムを終了する

//////////////////////////////////////////////////////////////////////////////////////////
// コンストラクタ
//////////////////////////////////////////////////////////////////////////////////////////
RuntimeScriptSystem::RuntimeScriptSystem(){
    // hostfxr.dllをロード
    HMODULE lib = ::LoadLibraryW(MyFunc::ToFullPath(hostFxrDllPath_).c_str());

    // hostfxrを利用するための各関数を取得
    initFxr = GetFunction<hostfxr_initialize_for_runtime_config_fn>(lib, "hostfxr_initialize_for_runtime_config");
    getDelegate = GetFunction<hostfxr_get_runtime_delegate_fn>(lib, "hostfxr_get_runtime_delegate");
    closeFxr = GetFunction<hostfxr_close_fn>(lib, "hostfxr_close");

    // どれか一つでも取得できなかったらエラー
    if(!initFxr || !getDelegate || !closeFxr){
        assert(false);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
// instanceの取得関数
//////////////////////////////////////////////////////////////////////////////////////////
RuntimeScriptSystem* RuntimeScriptSystem::GetInstance(){
    static RuntimeScriptSystem instance;
    return &instance;
}


//////////////////////////////////////////////////////////////////////////////////////////
// ランタイムでスクリプトを再コンパイルする関数
//////////////////////////////////////////////////////////////////////////////////////////
void RuntimeScriptSystem::CompileScripts(){
    // ランタイムを起動し、Contextを取得
    HostFxrContext runtimeContext = GetHostFxrContext(compilerConfigPath_);

    // 実際の関数を取得する
    typedef void (CORECLR_DELEGATE_CALLTYPE* CompileFunc)();// CORECLR_DELEGATE_CALLTYPEで呼び出し規約をC#側のものに合わせる
    CompileFunc compile = nullptr;
    int result = runtimeContext.loadFunc(
        compilerDllPath_.c_str(), // DLL パス
        L"ScriptCompiler",   // C# 側のクラス名。namespace(あれば) + class名。
        L"CompileScripts",
        L"UNMANAGED_CALLERSONLY_METHOD",
        nullptr,
        (void**)&compile
    );

    // 関数の取得に失敗した場合はエラー
    if(result != 0 || compile == nullptr){
        throw std::runtime_error("Compile 関数の取得に失敗");
    }

    // 実際に C# 側のコンパイル処理を呼び出す
    compile();

    // ランタイムを閉じる
    closeFxr(runtimeContext.handle);
}


//////////////////////////////////////////////////////////////////////////////////////////
// HostFxrContextを取得する関数
//////////////////////////////////////////////////////////////////////////////////////////
HostFxrContext RuntimeScriptSystem::GetHostFxrContext(const std::wstring& runtimeConfigPath){
    HostFxrContext context;
    context.handle = BootRuntime(runtimeConfigPath);
    context.loadFunc = GetLoadFunction(context.handle);
    return context;
}


//////////////////////////////////////////////////////////////////////////////////////////
// configの情報から.NETランタイムを起動する関数
//////////////////////////////////////////////////////////////////////////////////////////
hostfxr_handle RuntimeScriptSystem::BootRuntime(const std::wstring& runtimeConfigPath){
    // configをもとにランタイムの起動を行う
    hostfxr_handle runtimeContext = nullptr;
    int result = initFxr(runtimeConfigPath.c_str(), nullptr, &runtimeContext);
    // 起動に失敗した場合はエラー
    if(result != 0 || runtimeContext == nullptr){
        throw std::runtime_error("hostfxr_initialize_for_runtime_config に失敗");
    }
    return runtimeContext;
}

//////////////////////////////////////////////////////////////////////////////////////////
// 関数を読み込むための関数を作成し、取得する関数
//////////////////////////////////////////////////////////////////////////////////////////
load_assembly_and_get_function_pointer_fn RuntimeScriptSystem::GetLoadFunction(hostfxr_handle handle){
    // 関数を呼び出すための関数のデリゲートを取得する
    void* load_assembly_and_get_function_pointer = nullptr;
    int result = getDelegate(handle, hdt_load_assembly_and_get_function_pointer, &load_assembly_and_get_function_pointer);

    // デリゲートの取得に失敗した場合はエラー
    if(result != 0 || load_assembly_and_get_function_pointer == nullptr){
        throw std::runtime_error("delegate の取得に失敗");
    }

    // 関数呼び出し関数の型に変換し、関数ポインタを返す
    return reinterpret_cast<load_assembly_and_get_function_pointer_fn>(load_assembly_and_get_function_pointer);
}