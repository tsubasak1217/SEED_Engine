#include "FieldObject_MoveFloor.h"

// lib
#include "ClockManager.h"
#include "../adapter/json/JsonCoordinator.h"

// local
#include "../FieldObject/Activator/FieldObject_Activator.h"

/////////////////////////////////////////////////////////////////////////
// コンストラクタ
/////////////////////////////////////////////////////////////////////////
FieldObject_MoveFloor::FieldObject_MoveFloor(RoutineManager& routineManager) :
    routineManager_(routineManager){
    // 名前の初期化
    className_ = "FieldObject_MoveFloor";
    name_ = "moveFloor";
    // モデルの初期化
    std::string path = "FieldObject/" + name_ + ".obj";
    model_ = std::make_unique<Model>(path);
    model_->isRotateWithQuaternion_ = false;

    // コライダー関連の初期化
    colliderEditor_ = std::make_unique<ColliderEditor>(className_, this);
    InitColliders(ObjectType::MoveFloor);

    // 全般の初期化
    Initialize();
}

FieldObject_MoveFloor::FieldObject_MoveFloor(const std::string& modelName, RoutineManager& routineManager) :
    FieldObject(modelName), routineManager_(routineManager){
    // クラス名の初期化
    className_ = "FieldObject_MoveFloor";
    // コライダー関連の初期化
    colliderEditor_ = std::make_unique<ColliderEditor>(className_, this);
    InitColliders(ObjectType::Field);
    // 全般の初期化
    Initialize();

}

/////////////////////////////////////////////////////////////////////////
// 初期化関数
/////////////////////////////////////////////////////////////////////////
void FieldObject_MoveFloor::Initialize(){
    FieldObject::Initialize();
}

/////////////////////////////////////////////////////////////////////////
// 更新関数
/////////////////////////////////////////////////////////////////////////
void FieldObject_MoveFloor::Update(){
    if (hasActivator_ && isSwitchActive_){
        Move();
    }
    FieldObject::Update();
}

/////////////////////////////////////////////////////////////////////////
// ImGui表示関数
/////////////////////////////////////////////////////////////////////////
void FieldObject_MoveFloor::ShowImGui(){
    ImGui::DragFloat("MoveSpeed", &moveSpeed_, 0.01f);

    // ルーチン選択用のコンボ

    std::vector<std::string> routineNames = routineManager_.GetRoutineNames();

    // 現在の routineName_ をインデックス化
    int currentIndex = 0;
    for (size_t i = 0; i < routineNames.size(); ++i){
        if (routineNames[i] == routineName_){
            currentIndex = static_cast< int >(i);
            break;
        }
    }

    // コンボ表示
    std::vector<const char*> routineNamesCStr;
    routineNamesCStr.reserve(routineNames.size());
    for (auto& rName : routineNames){
        routineNamesCStr.push_back(rName.c_str());
    }

    if (ImGui::Combo("Select Routine", &currentIndex,
        routineNamesCStr.data(),
        static_cast< int >(routineNamesCStr.size()))){
        // 選択変更されたら更新
        routineName_ = routineNames[currentIndex];
        const auto* points = routineManager_.GetRoutinePoints(routineName_);
        if (points){
            SetRoutinePoints(*points); // 選択ルーチンに対応するポイントを設定
        }
    }
}

/////////////////////////////////////////////////////////////////////////
// Observerの関数
/////////////////////////////////////////////////////////////////////////
void FieldObject_MoveFloor::OnNotify(const std::string& event, [[maybe_unused]] void* data){
    // Switch や Lever からの Activated/Deactivated を捕まえて床を動かす
    // イベントが違うなら無視
    if (event != "SwitchActivated" && event != "SwitchDeactivated"
        && event != "LeverActivated" && event != "LeverDeactivated"){
        return;
    }

    // Activated イベントなら動作開始
    if (event == "SwitchActivated" || event == "LeverActivated"){
        isSwitchActive_ = true;
    }
    // Deactivated イベントなら動作停止
    else if (event == "SwitchDeactivated" || event == "LeverDeactivated"){
        isSwitchActive_ = false;
    }
}


/* private =============================================================*/

/////////////////////////////////////////////////////////////////////////
// ルーチンの初期化
/////////////////////////////////////////////////////////////////////////
void FieldObject_MoveFloor::InitializeRoutine(){
    const auto* points = routineManager_.GetRoutinePoints(routineName_);
    if (points){
        SetRoutinePoints(*points); // 選択ルーチンに対応するポイントを設定
    }
}

void FieldObject_MoveFloor::HandOverColliders(){
    // 基本コライダーを渡す
    BaseObject::HandOverColliders();
}

/////////////////////////////////////////////////////////////////////////
// 移動関数
/////////////////////////////////////////////////////////////////////////
void FieldObject_MoveFloor::Move(){
    if (routinePoints_.size() < 1){ return; }
    Vector3 currentPos = GetWorldTranslate();
    Vector3 targetPos = (routinePoints_)[currentMovePointIndex_];
    Vector3 direction = targetPos - currentPos;
    float distanceToTarget = MyMath::Length(direction);

    // 正規化された移動方向
    Vector3 moveDir = MyMath::Normalize(direction);
    // 1フレームあたりの移動量
    float moveStep = moveSpeed_ * ClockManager::DeltaTime();

    if (moveStep >= distanceToTarget){
        // 移動ステップがターゲットまでの距離以上の場合、ターゲットに直接移動
        model_->translate_ = targetPos;
        currentMovePointIndex_ = (currentMovePointIndex_ + 1) % routinePoints_.size();
    } else{
        // 通常通り移動
        model_->translate_ = model_->translate_ + moveDir * moveStep;
    }

    // 移動後に距離を再チェック
    currentPos = GetWorldTranslate();
    targetPos = (routinePoints_)[currentMovePointIndex_];
    distanceToTarget = MyMath::Length(currentPos, targetPos);

    if (distanceToTarget < 0.01f){
        currentMovePointIndex_ = (currentMovePointIndex_ + 1) % routinePoints_.size();
    }

}


/* public =============================================================*/

/////////////////////////////////////////////////////////////////////////
// getter
/////////////////////////////////////////////////////////////////////////
void FieldObject_MoveFloor::SetActivator(FieldObject_Activator* pActivator){
    FieldObject_Activator* activator = pActivator;
    if (activator){
        activator->RegisterObserver(this);
    }
    hasActivator_ = true;
}

void FieldObject_MoveFloor::RemoveActivator(FieldObject_Activator* pActivator){
    FieldObject_Activator* activator = pActivator;
    if (activator){
        activator->UnregisterObserver(this);
    }
    hasActivator_ = false;
}
