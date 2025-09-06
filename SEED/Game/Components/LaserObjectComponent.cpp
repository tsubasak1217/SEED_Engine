#include "LaserObjectComponent.h"

//============================================================================
//	include
//============================================================================
#include <Game/Objects/Stage/Objects/Laser/Methods/LaserHelper.h>
#include <Game/Objects/Stage/Objects/Warp/Warp.h>
#include <Game/Components/StageObjectComponent.h>

// lasers
#include <Game/Objects/Stage/Objects/Laser/Laser.h>

//============================================================================
//	LaserObjectComponent classMethods
//============================================================================

LaserObjectComponent::LaserObjectComponent(GameObject2D* pOwner, const std::string& tagName) : IComponent(pOwner, tagName) {

    // タグの名前が指定されていなければIDをタグ名にする
    if (tagName == "") {
        componentTag_ = "LaserObjectComponent_ID:" + std::to_string(componentID_);
    }
}

void LaserObjectComponent::Initialize(LaserObjectType objectType, const Vector2& translate) {

    objectType_ = objectType;

    // インスタンスを作成
    object_ = CreateInstance(objectType);

    // 座標を設定
    object_->SetTranslate(translate);
}

std::unique_ptr<ILaserObject> LaserObjectComponent::CreateInstance(LaserObjectType objectType) const {

    // タイプで作成するインスタンスを作成する
    switch (objectType) {
    case LaserObjectType::Normaml: {

        std::unique_ptr<Laser> laser = std::make_unique<Laser>(owner_.owner2D);
        laser->Initialize();
        return laser;
    }
    }
    return nullptr;
}

bool LaserObjectComponent::CheckEndExtend(GameObject2D* other) {

    const auto type = other->GetObjectType();

    // ワープ処理のチェック
    if (CheckWarp(other)) {
        return true;
    }

    if (LaserHelper::HasObejctType(type, ObjectType::Player) ||
        LaserHelper::HasObejctType(type, ObjectType::Goal) ||
        LaserHelper::HasObejctType(type, ObjectType::EmptyBlock) ||
        LaserHelper::HasObejctType(type, ObjectType::Laser)) {
        return true;
    }
    return false;
}

bool LaserObjectComponent::CheckWarp(GameObject2D* other) {

    // ワープでないなら次の処理に進ませる
    if (!LaserHelper::HasObejctType(other->GetObjectType(), ObjectType::Warp)) {
        return false;
    }

    // すでに衝突している場合は処理しない
    if (object_->GetWarpParam().isHit) {
        return true;
    }
    // レーザーにワープと衝突したことを通知する
    if (StageObjectComponent* component = other->GetComponent<StageObjectComponent>()) {
        if (Warp* warp = component->GetStageObject<Warp>()) {

            // パラメータを設定
            WarpLaserParam param{};
            param.isHit = true;
            param.warpIndex = warp->GetWarpIndex();
            param.warpCommonState = warp->GetCommonState();
            object_->SetHitWarpParam(param);

            // レーザー停止予約、ワープするなら止める
            SetPendingWarpStop(component->GetBlockTranslate(), component->GetMapSize());
            return true;
        }
    }
    return false;
}

void LaserObjectComponent::ConsiderBlocker(const Vector2& center, const Vector2& size, bool apply) {

    // 方向を考慮した前面までの距離を取得
    const float distance = LaserHelper::ComputeFrontDistance(
        object_->GetDirection(), owner_.owner2D->GetWorldTranslate(), center, size);

    // 軸方向半サイズ
    const Vector2 axis = LaserHelper::GetAxisFromDirection(object_->GetDirection());
    const Vector2 half = size * 0.5f;
    const float padding = std::fabs(half.x * axis.x) + std::fabs(half.y * axis.y);
    const float eps = 1e-4f;

    // 後ろ側は除外しつつ最短を更新
    if (distance >= -padding - eps && distance < blocker_.frontDistance) {

        // 衝突相手を設定
        blocker_.isFound = true;
        blocker_.frontDistance = distance;
        blocker_.translate = center;
        blocker_.size = size;

        // 即時適応を行う
        if (apply) {

            Vector2 setSize = object_->GetSize();
            setSize.y = (std::max)(0.0f, distance);
            object_->SetSize(setSize);
            object_->StopExtend();
        }
    }
}

void LaserObjectComponent::SetPendingWarpStop(const Vector2& translate, const Vector2& size) {

    // 予約開始
    pendingWarpStop_.has = true;
    // ワープは中心で止める
    pendingWarpStop_.byCenter = true;
    pendingWarpStop_.translate = translate;
    pendingWarpStop_.size = size;
}

void LaserObjectComponent::ApplyPendingWarpStop() {

    // 予約されたレーザーの停止を実行する
    if (pendingWarpStop_.has) {

        // 衝突していた相手を記録しておく
        if (!keepStoppedByWarp_) {

            savedBlocker_ = blocker_;
            hasSavedBlocker_ = blocker_.isFound;
        }

        // 中心で止めるかどうか
        if (pendingWarpStop_.byCenter) {

            // 中心に対する前面距離を手計算
            const Vector2 origin = owner_.owner2D->GetWorldTranslate();
            const Vector2 axis = LaserHelper::GetAxisFromDirection(object_->GetDirection());
            const Vector2 to = pendingWarpStop_.translate - origin;
            const float   dist = to.x * axis.x + to.y * axis.y;
            const float   eps = 1e-4f;
            if (dist >= -eps && dist < blocker_.frontDistance) {

                blocker_.isFound = true;
                blocker_.frontDistance = dist;
                blocker_.translate = pendingWarpStop_.translate;
                // 即時Clampして停止
                Vector2 setSize = object_->GetSize();
                setSize.y = (std::max)(0.0f, dist);
                object_->SetSize(setSize);
                object_->StopExtend();
            }
        } else {

            // 中心からブロックのサイズ分流れた部分で停止させる
            ConsiderBlocker(pendingWarpStop_.translate, pendingWarpStop_.size, true);
        }

        // レーザー停止処理を実行させる
        pendingWarpStop_.byCenter = false;
        pendingWarpStop_.has = false;
        keepStoppedByWarp_ = true;
    }
}

void LaserObjectComponent::ClearPendingWarpStop() {

    pendingWarpStop_.has = false;
    keepStoppedByWarp_ = false;

    // 保存されていた衝突相手を設定する
    if (hasSavedBlocker_) {

        blocker_ = savedBlocker_;
    }
    hasSavedBlocker_ = false;
}

//============================================================================
//	LaserObjectComponent loopMethods
//============================================================================

void LaserObjectComponent::Update() {

    // objectの更新
    object_->SetTranslate(owner_.owner2D->GetWorldTranslate());
    object_->Update();
}

void LaserObjectComponent::Draw() {

    // objectの描画
    object_->Draw();
}

void LaserObjectComponent::OnCollisionEnter(GameObject2D* other) {

    // 他のフィールドオブジェクトと衝突したらレーザーの伸びる処理を止める
    // プレイヤー、ゴール以外
    if (CheckEndExtend(other)) {
        return;
    }

    // 衝突した瞬間にレーザーを止める
    if (StageObjectComponent* component = other->GetComponent<StageObjectComponent>()) {

        ConsiderBlocker(component->GetBlockTranslate(), component->GetMapSize(), true);
    }
}

void LaserObjectComponent::OnCollisionStay(GameObject2D* other) {

    // 他のフィールドオブジェクトと衝突したらレーザーの伸びる処理を止める
    // プレイヤー、ゴール以外
    if (CheckEndExtend(other)) {
        return;
    }

    // 衝突相手を検索して新しく近い相手と衝突していたらサイズを調整する
    if (StageObjectComponent* component = other->GetComponent<StageObjectComponent>()) {

        ConsiderBlocker(component->GetBlockTranslate(), component->GetMapSize(), false);
    }
}

void LaserObjectComponent::OnCollisionExit(GameObject2D* other) {

    // ワープと離れたらfalseにする
    if (LaserHelper::HasObejctType(other->GetObjectType(), ObjectType::Warp)) {

        auto param = object_->GetWarpParam();
        param.isHit = false;
        object_->SetHitWarpParam(param);

        // レーザー停止処理終了
        keepStoppedByWarp_ = false;
    }
}

//============================================================================
//	LaserObjectComponent otherMethods
//============================================================================

void LaserObjectComponent::BeginFrame() {

    // 衝突相手をリセット
    blocker_.isFound = false;
    blocker_.frontDistance = std::numeric_limits<float>::infinity();
}

void LaserObjectComponent::EndFrame() {

    // ワープ中は停止させる
    if (keepStoppedByWarp_) {

        object_->StopExtend();
        return;
    }

    if (blocker_.isFound) {

        // フィールドオブジェクトにのめり込んだかチェック
        const float current = object_->GetSize().y;
        const float stopAtSize = blocker_.frontDistance;
        const float eps = 1e-4f;
        if (current + eps >= stopAtSize) {

            Vector2 size = object_->GetSize();
            size.y = (std::max)(0.0f, stopAtSize);
            object_->SetSize(size);
            object_->StopExtend();
        } else {

            object_->ReExtend();
        }
    } else {

        // レーザーを再起動する
        object_->ReExtend();
    }
}

void LaserObjectComponent::EditGUI() {
#ifdef _DEBUG
    ImGui::Indent();
    ImGui::Unindent();
#endif // _DEBUG
}

nlohmann::json LaserObjectComponent::GetJsonData() const {
    nlohmann::json jsonData;
    jsonData["componentType"] = "Block";
    jsonData.update(IComponent::GetJsonData());
    return jsonData;
}

void LaserObjectComponent::LoadFromJson(const nlohmann::json& jsonData) {
    IComponent::LoadFromJson(jsonData);
}