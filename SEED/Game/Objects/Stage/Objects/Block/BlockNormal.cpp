#include "BlockNormal.h"
#include <SEED/Lib/Functions/MyFunc/ShapeMath.h>
#include <SEED/Lib/Functions/MyFunc/Easing.h>
#include <SEED/Source/SEED.h>
#include <Game/Components/StageObjectComponent.h>
#include <Game/Objects/Stage/Objects/Player/Entity/Player.h>

// scene
#include <Game/GameSystem.h>
#include <Game/Scene/Scene_Game/Scene_Game.h>

//============================================================================
//	BlockNormal classMethods
//============================================================================

void BlockNormal::Initialize() {

    static bool isFirstInitialize = true;
    if (isFirstInitialize) {
        imageMap_["NormalBlock"] = TextureManager::LoadTexture("Scene_Game/StageObject/normalBlock.png");
        imageMap_["White"] = TextureManager::LoadTexture("DefaultAssets/white1x1.png");
        imageMap_["FrameRect"] = TextureManager::LoadTexture("Scene_Game/StageObject/frameRect.png");
        isFirstInitialize = false;
    }

    // スプライトの初期化
    sprite_ = Sprite("Scene_Game/StageObject/normalBlock.png");
    sprite_.anchorPoint = Vector2(0.5f);
    sprite_.layer = 14;

    centerQuad_ = MakeEqualQuad2D(sprite_.size.x * 0.5f);
    centerQuad_.lightingType = LIGHTINGTYPE_NONE;
    hologramQuad_ = MakeEqualQuad2D(sprite_.size.x * 0.5f);
    hologramQuad_.lightingType = LIGHTINGTYPE_NONE;
}

void BlockNormal::Update() {

    // プレイヤーに触れられていない場合、タイマーを減少させる
    if (!isTouchedByPlayer_) {
        touchedTimer_ -= ClockManager::DeltaTime();
        touchedTimer_ = std::clamp(touchedTimer_, 0.0f, maxTouchedTime_);
    }

    // オブジェクトの更新を行う
    switch (commonState_) {

    case StageObjectCommonState::None:
        sprite_.color = normalColor_;
        sprite_.layer = 16;
        break;

    case StageObjectCommonState::Hologram:
        sprite_.color = hologramColor_;
        sprite_.layer = 14;
        break;
    }
}

void BlockNormal::Draw() {

    // スプライトの描画
    sprite_.Draw();

    // 中心の四角形を描画
    QuadDraw();
}


//////////////////////////////////////////////////////////////////////////////
// 衝突時の処理
//////////////////////////////////////////////////////////////////////////////
void BlockNormal::OnCollisionEnter(GameObject2D* other) {
    if (other->GetObjectType() == ObjectType::Player) {
        isTouchedByPlayer_ = true;
    }
}

void BlockNormal::OnCollisionStay(GameObject2D* other) {

    if (other->GetObjectType() == ObjectType::Player) {
        touchedTimer_ += ClockManager::DeltaTime();
        touchedTimer_ = std::clamp(touchedTimer_, 0.0f, maxTouchedTime_);

        // NormalBlockを踏んだらその座標を記録する
        Player* player = nullptr;
        if (StageObjectComponent* component = other->GetComponent<StageObjectComponent>()) {
            player = component->GetStageObject<Player>();
        }
        // 着地の瞬間
        const bool landingNow = other->GetIsOnGroundTrigger();
        // 地面上 かつ 入力で移動中
        const bool walkingNow = player->OnGround() && player->GetIsMoving();

        if (landingNow || walkingNow) {
            if (Scene_Game* pScene = dynamic_cast<Scene_Game*>(GameSystem::GetScene())) {

                pScene->GetStage()->RecordPlayerOnBlock(GetTranslate());
            }
        }
    }
}

void BlockNormal::OnCollisionExit(GameObject2D* other) {
    if (other->GetObjectType() == ObjectType::Player) {

        isTouchedByPlayer_ = false;
    }
}

void BlockNormal::SetSize(const Vector2& size) {
    IStageObject::SetSize(size);

    centerQuad_ = MakeEqualQuad2D((size.x * 0.5f) * 0.2f);
    centerQuad_.lightingType = LIGHTINGTYPE_NONE;
    hologramQuad_ = MakeEqualQuad2D(size.x * 0.5f);
    hologramQuad_.lightingType = LIGHTINGTYPE_NONE;
}

// 中の四角形の描画
void BlockNormal::QuadDraw() {

    // 空白ブロックに重なっていたらそもそも描画しない
    if (isSetStopAppearance_ && commonState_ == StageObjectCommonState::Hologram) {
        return;
    }

    if (!appearanceWaitTimer_.IsFinished()) {
        return;
    }

    // 時間の更新
    motionTimer += ClockManager::DeltaTime();

    // Quadの更新/描画
    {
        float t = touchedTimer_ / maxTouchedTime_;
        float easeT = EaseOutBack(t);
        centerQuad_.GH = imageMap_["White"];
        centerQuad_.layer = sprite_.layer + 1;
        centerQuad_.translate = sprite_.transform.translate;
        centerQuad_.color = sprite_.color;
        if (appearanceTimer_.IsFinished()) {

            centerQuad_.scale = Vector2(1.0f) + Vector2(easeT * 0.6f);
        } else {

            centerQuad_.scale = commonScale_;
            centerQuad_.color.w = commonScale_;
        }
        SEED::DrawQuad2D(centerQuad_);
    }

    // ホログラム状態のときのみの描画
    if (commonState_ == StageObjectCommonState::Hologram) {
        hologramQuad_.GH = imageMap_["FrameRect"];
        hologramQuad_.layer = sprite_.layer + 1;
        hologramQuad_.color = sprite_.color;
        hologramQuad_.translate = sprite_.transform.translate;

        static float kTime = 1.0f;
        static const int divCount = 2;
        float sampledTime[divCount];
        for (int i = 0; i < divCount; i++) {
            sampledTime[i] = std::fmodf(motionTimer + (kTime / divCount) * i, kTime) / kTime;
            sampledTime[i] = std::clamp(sampledTime[i], 0.0f, 1.0f);
            float easeT = EaseOutSine(sampledTime[i]);

            hologramQuad_.color.w = sampledTime[i] * 0.3f;
            hologramQuad_.scale = Vector2(0.2f) + Vector2(easeT * 0.6f);
            SEED::DrawQuad2D(hologramQuad_);
        }
    }
}
