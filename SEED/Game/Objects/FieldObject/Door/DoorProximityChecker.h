#pragma once

// forward declaration
class Player;
class EventManager;
class FieldObjectManager;

class DoorProximityChecker{
public:
    DoorProximityChecker(
        EventManager& eventManager,
        FieldObjectManager& fieldObjectManager,
        Player& player
    );

    void Update();

    // ドアに近いと判定する距離
    void SetCheckRadius(float radius){ checkRadius_ = radius; }
    float GetCheckRadius() const{ return checkRadius_; }

private:
    // プレイヤーとの距離が checkRadius_ 以内ならイベントを発行
    void CheckAndNotify();

private:
    EventManager& eventManager_;
    FieldObjectManager& fieldObjectManager_;
    Player& player_;

    float checkRadius_ = 20.0f; // 例: 5m以内ならドアを開く
};
