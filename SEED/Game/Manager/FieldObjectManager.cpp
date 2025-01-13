#include "FieldObjectManager.h"
#include "CollisionManaer/CollisionManager.h"
#include "FieldObject/Door/FieldObject_Door.h"

////////////////////////////////////////////////////////////////////////
// 更新関数
////////////////////////////////////////////////////////////////////////
void FieldObjectManager::Update(){
    for (auto& fieldObject : fieldObjects_){

        FieldObject_Door* door = dynamic_cast< FieldObject_Door* >(fieldObject.get());

        // ドアの場合は、ドアの更新処理を呼び出す
        if (door){
            door->Update();
        } else{
            // 通常のオブジェクト
            fieldObject->Update();
        }

    }
}

////////////////////////////////////////////////////////////////////////
// 描画関数
////////////////////////////////////////////////////////////////////////
void FieldObjectManager::Draw(){
    for (auto& fieldObject : fieldObjects_){
        fieldObject->Draw();
    }
}

////////////////////////////////////////////////////////////////////////
// フレーム開始時処理
////////////////////////////////////////////////////////////////////////
void FieldObjectManager::BeginFrame(){
    for(auto& fieldObject : fieldObjects_){
        fieldObject->BeginFrame();
    }
}


////////////////////////////////////////////////////////////////////////
// フレーム終了時処理
////////////////////////////////////////////////////////////////////////
void FieldObjectManager::EndFrame(){
    for(auto& fieldObject : fieldObjects_){
        fieldObject->EndFrame();
    }
}


////////////////////////////////////////////////////////////////////////
// すべてのオブジェクトをクリア
////////////////////////////////////////////////////////////////////////
void FieldObjectManager::ClearAllFieldObjects(){
    // オブジェクトをクリアする前に、オブザーバー登録を解除
    for (auto& obj : fieldObjects_){
        IObserver* observer = dynamic_cast< IObserver* >(obj.get());
        if (observer){
            subject_.UnregisterObserver(observer);
        }
    }
    fieldObjects_.clear();
}

////////////////////////////////////////////////////////////////////////
// 新しいオブジェクトを追加
////////////////////////////////////////////////////////////////////////
void FieldObjectManager::AddFieldObject(std::unique_ptr<FieldObject> obj){
    // オブジェクトが IObserver を実装している場合、EventManager に登録
    IObserver* observer = dynamic_cast< IObserver* >(obj.get());
    if (observer){
        subject_.RegisterObserver(observer);
    }

    fieldObjects_.push_back(std::move(obj));
}

////////////////////////////////////////////////////////////////////////
// CollisionManagerにコライダーを渡す
////////////////////////////////////////////////////////////////////////
void FieldObjectManager::HandOverColliders(){
    for(auto& fieldObject : fieldObjects_){
        fieldObject->HandOverColliders();
    }
}
