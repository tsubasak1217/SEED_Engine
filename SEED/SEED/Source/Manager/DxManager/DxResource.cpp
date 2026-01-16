#include "DxResource.h"
#include <SEED/Source/Manager/DxManager/DxManager.h>
#include <SEED/Source/Manager/DxManager/ViewManager.h>

namespace SEED{
    void DxResource::TransitionState(D3D12_RESOURCE_STATES stateAfter){
        // stateが同じなら何もしない
        if(state == stateAfter){
            return;
        }

        // stateを遷移
        Methods::DxFunc::TransitionResourceState(
            DxManager::GetInstance()->commandList.Get(),
            resource.Get(),
            state,
            stateAfter
        );

        // stateを更新
        stateBefore = state;
        state = stateAfter;
    }

    // リソースの状態を元に戻す
    void DxResource::RevertState(){
        // stateを遷移
        TransitionState(stateBefore);
    }

    /// 初期化時に呼び出す関数
    void DxResource::InitState(D3D12_RESOURCE_STATES _state){
        // 初期化時のstateを保存
        stateBefore = _state;
        this->state = _state;
    }

    // SRVの作成
    void DxResource::CreateSRV(const std::string& viewName){
        srvIndex = ViewManager::CreateView(
            VIEW_TYPE::SRV,
            resource.Get(),
            &srvDesc,
            viewName
        );

        srvName = viewName;
        srvHandle = ViewManager::GetHandleGPU(HEAP_TYPE::SRV_CBV_UAV, viewName);
    }

    // UAVの作成
    void DxResource::CreateUAV(const std::string& viewName){
        uavIndex = ViewManager::CreateView(
            VIEW_TYPE::UAV,
            resource.Get(),
            &uavDesc,
            viewName
        );

        uavName = viewName;
        uavHandle = ViewManager::GetHandleGPU(HEAP_TYPE::SRV_CBV_UAV, viewName);
    }
}