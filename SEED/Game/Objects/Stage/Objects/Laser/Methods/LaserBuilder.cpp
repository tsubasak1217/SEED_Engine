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