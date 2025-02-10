#pragma once
#include <SEED/Lib/Functions/MyFunc/DxFunc.h>

class DxManager;
class SEED;

enum class DEAPTH_TEXTURE_FORMAT {
    NORMAL = 0,
    FOCUS
};

enum class DX_RESOURCE_TYPE {
    TEXTURE2D = 0,
    TEXTURE3D,
    RENDER_TARGET,
    DEPTH_STENCIL,
    BUFFER
};

class EffectManager{

public:
    EffectManager() = default;
    EffectManager(DxManager* pDxManager);
    ~EffectManager();
    void Initialize();
    void Finalize();

public:

    // depthStencil用のreadbackBufferを作成して初期化する関数    
    void TransfarToCS();

    //
    void CopyDepthStencil(
        ID3D12Resource* dstResource,
        ID3D12Resource* sourceResource,
        D3D12_RESOURCE_STATES currentState_source,
        D3D12_RESOURCE_STATES currentState_dst
    );

private:
    // dxManager参照のためのポインタ
    DxManager* pDxManager_;
    SEED* pSEED_;
};
