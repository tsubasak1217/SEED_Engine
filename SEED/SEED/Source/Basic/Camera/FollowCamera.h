#pragma once
#include <SEED/Source/Basic/Camera/BaseCamera.h>
#include <SEED/Lib/Functions/Math.h>

#include <SEED/Source/Basic/Object/GameObject.h>
#include <SEED/Source/Manager/InputManager/InputHandler.h>


namespace SEED{
    /// <summary>
    /// 追従カメラ
    /// </summary>
    struct FollowCamera : public BaseCamera{

    public:
        FollowCamera();
        ~FollowCamera() override;
        void Initialize() override;
        void Update() override;

        void Reset(const Vector3& pos);

    private:
        void UpdateAngle();
        void UpdateDistance();

    public:
        void SetTarget(GameObject* target);
        GameObject* GetPreTarget()const{ return preTarget_; }
        void SetDistance(float distance){ distance_ = distance; }
        void SetRotateSpeed(float speed){ rotateSpeed_ = speed; }
        void SetIsInputActive(bool isActive){ isInputActive_ = isActive; }
        void SetTheta(float theta){ theta_ = theta; }
        void AddTheta(float theta){ theta_ += theta; }
        void SetPhi(float phi){ phi_ = phi; }
        void AddPhi(float phi){ phi_ += phi; }
        void SetInterpolationRate(float rate){ interpolationRate_ = rate; }
        void SetisViewingObject(bool isViewingObject){ isViewingObject_ = isViewingObject; }
        bool GetIsViewingObject()const{ return isViewingObject_; }

    private:
        GameObject* target_ = nullptr;
        GameObject* preTarget_ = nullptr;
        Vector3 targetPos_;
        Vector3 aimTargetPos_;
        Vector3 aimPosition_;
        Vector3 aimRotate_;
        Vector3 defaultOffset_;
        float distance_;
        float theta_;
        float phi_;
        float rotateSpeed_;
        bool isInputActive_ = true;

        bool isViewingObject_ = false;

        // 限界角度
        float kMaxPhi_;
        float kMinPhi_;

        // 補間割合（通常モード）
        float interpolationRate_ = 0.15f;

        // 入力ハンドラ（外部からキーコンフィグ設定可能にするため public）
    public:
        InputHandler<Vector2> angleInput_;
        InputHandler<float> distanceInput_;
    };
}