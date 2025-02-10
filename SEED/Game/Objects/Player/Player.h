#pragma once
// parent
#include <Game/Objects/Base/BaseCharacter.h>
///stl
// pointer
#include <memory>
// container
#include <list>
#include <vector>
#include <json.hpp>
// object
#include <SEED/Source/Object/Collision/Collider.h>
#include <SEED/Source/Object/Camera/BaseCamera.h>
#include <SEED/Lib/Structs/Sprite.h>

// state
class IPlayerState;

class Player : public BaseCharacter{
public:
    Player();
    ~Player();
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void BeginFrame() override;
    void EndFrame() override;

private:

public: // Stateから呼び出す関数
    void HandleMove(const Vector3& acceleration) override;

public: // アクセッサ
    void SetPosition(const Vector3& pos){ model_->translate_ = pos; }
    void SetFollowCameraPtr(BaseCamera* pCamera){ pCamera_ = pCamera; }
    const BaseCamera* GetFollowCamera() const{ return pCamera_; }
    BaseCamera* GetFollowCamera(){ return pCamera_; }

    // json
    const nlohmann::json& GetJsonData()override;


public:
    void OnCollision(BaseObject* other,ObjectType objectType) override;

private: // フォローカメラ、ターゲット用
    BaseCamera* pCamera_ = nullptr;
};