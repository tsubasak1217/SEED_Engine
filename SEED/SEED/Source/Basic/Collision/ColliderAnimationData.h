#pragma once
#include <vector>
#include <memory>
#include <string>
#include <json.hpp>
#include <SEED/Lib/Structs/ModelAnimation.h>
#include <SEED/Source/Manager/ImGuiManager/ImGuiManager.h>
#include <SEED/Lib/Tensor/Quaternion.h>
#include <SEED/Lib/Tensor/Matrix4x4.h>

/// <summary>
/// コライダーを動かす情報
/// </summary>
class ColliderAnimationData{
public:
    ColliderAnimationData();
    void SetParentMat(const Matrix4x4* parentMat){ parentMat_ = parentMat; }
    float GetDuration()const{ return duration_; }
    Vector3 GetScale(float time)const;
    Quaternion GetRotation(float time)const;
    Vector3 GetTranslation(float time)const;
    nlohmann::json GetJsonData()const;
    void LoadFromJson(const nlohmann::json& jsonData,const Matrix4x4* parentMat);

public:
    void Edit(const std::string& headerName);
private:
    void InsertElement(float location);
    void DeleteElement(float location);
    void DrawCollider()const;
    void DrawCollider(float time,bool indexDraw = false);

private:
    const Matrix4x4* parentMat_ = nullptr;
    float duration_;// コライダーの寿命
    NodeAnimation nodeAnimation;// Colliderが生きている間の動き
    float insertLocation_ = 1.0f;// 挿入位置
    float deleteLocation_ = 1.0f;// 削除位置
    bool isLoop = false;// ループするかどうか
};