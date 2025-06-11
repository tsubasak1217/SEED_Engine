#include "DxResource.h"
#include <SEED/Source/Manager/DxManager/DxManager.h>

void DxResource::TransitionState(D3D12_RESOURCE_STATES stateAfter){
    // stateが同じなら何もしない
    if(state == stateAfter){
        return;
    }

    // stateを遷移
    TransitionResourceState(
        DxManager::GetInstance()->commandList.Get(),
        resource.Get(),
        state,
        stateAfter
    );

    // stateを更新
    stateBefore = state;
    state = stateAfter;
}

/// 初期化時に呼び出す関数
void DxResource::InitState(D3D12_RESOURCE_STATES _state){
    // 初期化時のstateを保存
    stateBefore = _state;
    this->state = _state;
}
