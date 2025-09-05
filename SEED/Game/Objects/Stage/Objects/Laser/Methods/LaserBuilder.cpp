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
    StageObjectCommonState commonState, const Vector2& translate, float laserSize) {

    // 追加するリスト
    std::list<GameObject2D*> laserList{};

    // 向きごとにインスタンスを作成する
    for (const auto& direction : directions) {

        // オブジェクトを作成
        GameObject2D* object = new GameObject2D(GameSystem::GetScene());
        // レーザー発生位置を設定
        object->SetWorldTranslate(GetTranslatedByDirection(direction, translate, laserSize));
        object->UpdateMatrix();

        // コンポーネントを初期化
        LaserObjectComponent* component = object->AddComponent<LaserObjectComponent>();
        component->Initialize(LaserObjectType::Normaml, Vector2(0.0f, 0.0f));
        // 必要な値を設定
        component->SetObjectCommonState(commonState);
        component->SetLaserDirection(direction);

        // サイズYは小さい値から開始して伸びさせる
        const float initSizeY = 0.4f;
        component->SetSize(Vector2(laserSize, initSizeY));
        // 伸びる状態を設定
        component->ReExtend();

        // リストの追加
        laserList.push_back(object);
    }
    return laserList;
}

Vector2 LaserBuilder::GetTranslatedByDirection(DIRECTION4 direction,
    const Vector2& translate, float laserSize) {

    Vector2 result = translate;

    // 向きごとにオフセットをかける
    switch (direction) {
    case DIRECTION4::UP: {

        result.y -= laserSize;
        break;
    }
    case DIRECTION4::DOWN: {

        result.y += laserSize;
        break;
    }
    case DIRECTION4::LEFT: {

        result.x -= laserSize;
        break;
    }
    case DIRECTION4::RIGHT: {

        result.x += laserSize;
        break;
    }
    }
    return result;
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
                const Vector2 size = component->GetLaserObject<Laser>()->GetSize();
                aabb->SetSize(size);
                aabb->SetCenter(Vector2(0.0f, -size.y * 1.0f));
                aabb->SetAnchor(Vector2(0.5f, -1.0f));
                aabb->isMovable_ = false;
                aabb->isGhost_ = true;
                aabb->SetObjectType(ObjectType::Laser);
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