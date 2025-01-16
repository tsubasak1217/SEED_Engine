#include "Stage.h"
#include "CollisionManaer/CollisionManager.h"
#include "FieldObject/Door/FieldObject_Door.h"
#include "FieldObject/Start/FieldObject_Start.h"
#include "FieldObject/Switch/FieldObject_Switch.h"


////////////////////////////////////////////////////////////////////////
// 更新関数
////////////////////////////////////////////////////////////////////////
void Stage::Update(){
    for(auto& fieldObject : fieldObjects_){
        // 通常のオブジェクト
        fieldObject->Update();
        fieldObject->EditCollider();
    }
}

////////////////////////////////////////////////////////////////////////
// 描画関数
////////////////////////////////////////////////////////////////////////
void Stage::Draw(){
    for(auto& fieldObject : fieldObjects_){
        fieldObject->Draw();
    }
}

////////////////////////////////////////////////////////////////////////
// フレーム開始時処理
////////////////////////////////////////////////////////////////////////
void Stage::BeginFrame(){
    for(auto& fieldObject : fieldObjects_){
        fieldObject->BeginFrame();
    }
}

////////////////////////////////////////////////////////////////////////
// フレーム終了時処理
////////////////////////////////////////////////////////////////////////
void Stage::EndFrame(){
    for(auto& fieldObject : fieldObjects_){
        fieldObject->EndFrame();
    }
}

////////////////////////////////////////////////////////////////////////
// すべてのオブジェクトをクリア
////////////////////////////////////////////////////////////////////////
void Stage::ClearAllFieldObjects(){
    // オブジェクトをクリアする前に、オブザーバー登録を解除
    for(auto& obj : fieldObjects_){
        IObserver* observer = dynamic_cast<IObserver*>(obj.get());
        if(observer){
            subject_.UnregisterObserver(observer);
        }
    }
    fieldObjects_.clear();
}

////////////////////////////////////////////////////////////////////////
// 新しいオブジェクトを追加
////////////////////////////////////////////////////////////////////////
void Stage::AddFieldObject(std::unique_ptr<FieldObject> obj){
    // オブジェクトが IObserver を実装している場合、EventManager に登録
    IObserver* observer = dynamic_cast<IObserver*>(obj.get());
    if(observer){
        subject_.RegisterObserver(observer);
    }

    fieldObjects_.push_back(std::move(obj));
}

void Stage::RemoveFieldObject(FieldObject* obj){
    // オブジェクトが IObserver を実装している場合、EventManager から登録解除
    IObserver* observer = dynamic_cast< IObserver* >(obj);
    if (observer){
        subject_.UnregisterObserver(observer);
    }
    // オブジェクトを削除
    fieldObjects_.erase(
        std::remove_if(
        fieldObjects_.begin(),
        fieldObjects_.end(),
        [obj] (const std::unique_ptr<FieldObject>& fieldObject){
            return fieldObject.get() == obj;
        }
    ),
        fieldObjects_.end()
    );
}

////////////////////////////////////////////////////////////////////////
// CollisionManagerにコライダーを渡す
////////////////////////////////////////////////////////////////////////
void Stage::HandOverColliders(){
    for(auto& fieldObject : fieldObjects_){
        fieldObject->HandOverColliders();
    }
}

Vector3 Stage::GetStartPosition() const{
    for(const auto& obj : fieldObjects_){
        // FieldObject_Start 型へのキャストを試みる
        if(auto* start = dynamic_cast<FieldObject_Start*>(obj.get())){
            // スタートオブジェクトが見つかったら位置を返す
            return start->GetWorldTranslate();
        }
    }
    // スタートオブジェクトが見つからなかった場合のデフォルト値を返す
    return Vector3{ 0.0f, 0.0f, 0.0f };
}

