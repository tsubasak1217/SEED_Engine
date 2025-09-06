#include "LaserBuilder.h"

//============================================================================
//	include
//============================================================================
#include <Game/GameSystem.h>
#include <Game/Components/LaserObjectComponent.h>
#include <Game/Objects/Stage/Objects/Laser/Methods/LaserHelper.h>

// lasers
#include <Game/Objects/Stage/Objects/Laser/Laser.h>

//============================================================================
//	LaserBuilder classMethods
//============================================================================

std::list<GameObject2D*> LaserBuilder::CreateLasersFromDirection(const std::vector<DIRECTION4>& directions,
    StageObjectCommonState commonState, const Vector2& translate, const Vector2& laserSize) {

    // 追加するリスト
    std::list<GameObject2D*> laserList{};

    // 向きごとにインスタンスを作成する
    for (const auto& direction : directions) {

        // ホログラムかどうかで向きを決定する
        DIRECTION4 stateDirection = LaserHelper::GetStateDirection(commonState, direction);

        // オブジェクトを作成
        GameObject2D* object = new GameObject2D(GameSystem::GetScene());
        // レーザー発生位置を設定
        const float gap = 0.16f; // 発射台と当たらないように少し離す
        object->SetWorldTranslate(LaserHelper::GetTranslatedByDirection(stateDirection, translate, laserSize.x, gap));
        object->UpdateMatrix();

        // コンポーネントを初期化
        LaserObjectComponent* component = object->AddComponent<LaserObjectComponent>();
        component->Initialize(LaserObjectType::Normaml, Vector2(0.0f, 0.0f));
        // 必要な値を設定
        component->SetObjectCommonState(commonState);
        component->SetLaserDirection(stateDirection);

        // サイズYは小さい値から開始して伸びさせる
        const float initSizeY = 0.4f;
        component->SetSize(Vector2(MyMath::Length(laserSize) / 2.0f, initSizeY));
        // 伸びる状態を設定
        component->ReExtend();

        // リストの追加
        laserList.push_back(object);
    }
    return laserList;
}

GameObject2D* LaserBuilder::CopyLaser(const Vector2& translate, GameObject2D* sourceLaser) {

    // 新しいオブジェクトを作成
    GameObject2D* object = new GameObject2D(GameSystem::GetScene());
    object->SetWorldTranslate(translate);
    object->UpdateMatrix();

    // 元レーザーのコンポーネントを参照
    const LaserObjectComponent* sourceComponent = sourceLaser->GetComponent<LaserObjectComponent>();

    // 新しいレーザーコンポーネントを作成
    LaserObjectComponent* dstComponent = object->AddComponent<LaserObjectComponent>();
    dstComponent->Initialize(LaserObjectType::Normaml, Vector2(0.0f, 0.0f));
    dstComponent->SetObjectCommonState(sourceComponent->GetLaserObject<Laser>()->GetCommonState());
    dstComponent->SetLaserDirection(sourceComponent->GetLaserObject<Laser>()->GetDirection());

    // ワープデータを共有する
    WarpLaserParam param = sourceComponent->GetLaserObject<Laser>()->GetWarpParam();
    dstComponent->GetLaserObject<Laser>()->SetHitWarpParam(param);
    // 系統IDを継承
    dstComponent->GetLaserObject<Laser>()->SetFamilyId(sourceComponent->GetLaserObject<Laser>()->GetFamilyId());

    // サイズYは小さい値から開始して伸びさせる
    const float initSizeY = 0.4f;
    const Vector2 sourceSize = sourceComponent->GetLaserObject<Laser>()->GetSize();
    dstComponent->SetSize(Vector2(sourceSize.x, initSizeY));
    // 伸びる状態を設定
    dstComponent->ReExtend();

    // コライダーを作成
    std::list<GameObject2D*> dstCollider{ object };
    CreateLaserColliders(dstCollider);

    return object;
}

void LaserBuilder::CreateLaserColliders(std::list<GameObject2D*>& lasers) {

    for (GameObject2D* laser : lasers) {
        if (LaserObjectComponent* component = laser->GetComponent<LaserObjectComponent>()) {

            LaserObjectType laserType = component->GetLaserObjectType();

            // Collisionの追加
            Collision2DComponent* collision = laser->AddComponent<Collision2DComponent>();
            Collider_AABB2D* aabb = new Collider_AABB2D();
            aabb->SetParentMatrix(laser->GetWorldMatPtr());

            // タイプ別で作成
            switch (laserType) {
            case LaserObjectType::Normaml: {

                // 初期サイズ、更新される
                aabb->SetSize(component->GetLaserObject<Laser>()->GetSize());
                aabb->SetAnchor(component->GetLaserObject<Laser>()->GetAnchorPoint());
                aabb->isMovable_ = false;
                aabb->isGhost_ = true;
                laser->SetObjectType(ObjectType::Laser);
                break;
            }
            }

            // 必要な値の更新
            laser->UpdateMatrix();
            aabb->UpdateMatrix();
            aabb->SetOwnerObject(laser);
            collision->AddCollider(aabb);
        }
    }
}