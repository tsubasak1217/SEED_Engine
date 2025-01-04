#pragma once
#include <vector>
#include <memory>
#include <string>
#include "ModelAnimation.h"
#include "ImGuiManager.h"
#include "Quaternion.h"
#include "Matrix4x4.h"

class ColliderAnimationData{
    friend class ColliderManager;// ColliderManagerにはアクセス権を与える

public:
    ColliderAnimationData() = default;
    float GetDuration()const{ return duration_; }
    Vector3 GetScale(float time)const;
    Quaternion GetRotation(float time)const;
    Vector3 GetTranslation(float time)const;

private:
    void EditByImGui(const std::string& headerName);
    void InsertElement(float location);
    void DeleteElement(float location);
    void DrawCollider()const;

private:
    float duration_;// コライダーの寿命
    std::unique_ptr<NodeAnimation> nodeAnimation;// Colliderが生きている間の動き
    float insertLocation_ = 1.0f;// 挿入位置
    float deleteLocation_ = 1.0f;// 削除位置
};