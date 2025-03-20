#include "ClosedState.h"
#include "../FieldObject_Door.h"

#include "CameraManager.h"
#include "FollowCamera.h"

void ClosedState::Enter([[maybe_unused]] FieldObject_Door* door){
    // ドアが完全に開いた際の処理（例：サウンド再生など）
    viewTime_ = kViewTime_;
}

void ClosedState::Update([[maybe_unused]] FieldObject_Door* door, [[maybe_unused]] float deltaTime){

    if(door->GetCameraViewActive()){
        viewTime_ -= ClockManager::DeltaTime();
        if(viewTime_ <= 0){
            FollowCamera* followCamera = dynamic_cast<FollowCamera*>(CameraManager::GetActiveCamera());
            if(followCamera && followCamera->GetPreTarget()){
                // カメラのターゲットを元に戻す
                followCamera->SetTarget(followCamera->GetPreTarget());
                followCamera->SetisViewingObject(false);

                // 禁忌
                followCamera->SetPhi(followCamera->prePhi_);
            }
            door->SetCameraViewActive(false);
        }

    }
}

void ClosedState::Exit([[maybe_unused]] FieldObject_Door* door){}
