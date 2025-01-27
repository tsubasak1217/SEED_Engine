#pragma once

#include "../FieldObject.h"
#include "../game/Routine/RoutineManager.h"

#include "../lib/patterns/IObserver.h"

#include <memory>
#include <string>

// 前方宣言
class FieldObject_Switch;

class FieldObject_MoveFloor
    :public FieldObject,public IObserver{
public:
    //===================================================================*/
    //                    public function
    //===================================================================*/

    //constructor / destructor
    FieldObject_MoveFloor(RoutineManager& routinemanager);
    FieldObject_MoveFloor(const std::string& modelName, RoutineManager& routineManager);
    ~FieldObject_MoveFloor()override = default;

    //main
    void Initialize() override;
    void Update() override;
    void InitializeRoutine();       //< ルーチンの初期化
    void HandOverColliders()override;
    //ImGui
    void ShowImGui() override;

    void OnNotify(const std::string& event, void* data = nullptr) override;

private:
    //===================================================================*/
    //                   private function
    //===================================================================*/
    void Move();                    //< 移動処理
    
private:
    //===================================================================*/
    //                   private variable
    //===================================================================*/
    std::vector<Vector3> routinePoints_;        //< ルーチンポイント
    std::string routineName_ = "NULL";          //< ルーチン名
    float moveSpeed_ = 1.0f;                    //< 移動速度
    int32_t currentMovePointIndex_ = 0;         //< 現在の移動ポイントのインデックス

    //routineの参照
    RoutineManager& routineManager_;

    // --- switch variable --- //
    bool hasSwitch_ = false;                    //< スイッチを持っているかどうか
    bool isSwitchActive_ = false;               //< スイッチの状態を追跡する新しいフラグ

public:
    //===================================================================*/
    //                   public variable
    //===================================================================*/
    static uint32_t nextFieldObjectID_;         //< 次のID

public:
    //===================================================================*/
    //                   getter / setter
    //===================================================================*/
#pragma region getter / setter
    void SetRoutineName(const std::string& name){ routineName_ = name; }
    void SetMoveSpeed(float speed){ moveSpeed_ = speed; }
    void SetRoutinePoints(const std::vector<Vector3>& points){ routinePoints_ = points; }
    std::vector<std::string> GetRoutineNames()const{
        return routineManager_.GetRoutineNames();
    }
    const std::string& GetRoutineName()const{ return routineName_; }
    const  std::vector<Vector3>* GetRoutinePoints(const std::string& routineName)const{
        return routineManager_.GetRoutinePoints(routineName);
    }
    float GetMoveSpeed()const{ return moveSpeed_; }

    void SetSwitch(FieldObject_Switch* pSwitch);
    void RemoveSwitch(FieldObject_Switch* pSwitch);
    bool GetHasSwitch()const{ return hasSwitch_; }
#pragma endregion
};
