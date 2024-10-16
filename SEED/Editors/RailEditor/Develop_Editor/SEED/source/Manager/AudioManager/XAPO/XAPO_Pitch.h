#pragma once
#include <xapo.h>

class XAPO_Pitch : public IXAPO, public IXAPOParameters{

public:// コンストラクタ　

    XAPO_Pitch() = default;

public:// 基礎の関数

    // 
    HRESULT __stdcall LockForProcess(UINT32 InputLockedParameterCount,
        const XAPO_LOCKFORPROCESS_BUFFER_PARAMETERS* pInputLockedParameters,
        UINT32 OutputLockedParameterCount,
        const XAPO_LOCKFORPROCESS_BUFFER_PARAMETERS* pOutputLockedParameters)override;

    // エフェクトをかける関数
    void __stdcall Process(UINT32 InputProcessParameterCount,
        const XAPO_PROCESS_BUFFER_PARAMETERS* pInputProcessParameters,
        UINT32 OutputProcessParameterCount,
        XAPO_PROCESS_BUFFER_PARAMETERS* pOutputProcessParameters,
        BOOL IsEnabled) override;

    void __stdcall UnlockForProcess()override;
    void __stdcall Reset()override;
    HRESULT __stdcall GetRegistrationProperties(XAPO_REGISTRATION_PROPERTIES** ppRegistrationProperties)override;



public:// アクセッサ
    void __stdcall SetParameters(const void* pParameters, UINT32 ParameterByteSize)override;
    void __stdcall GetParameters(void* pParameters, UINT32 ParameterByteSize)override;

private:
    float pitch_;  // 音量パラメータ
};