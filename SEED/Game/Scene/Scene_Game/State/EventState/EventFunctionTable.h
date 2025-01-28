#pragma once
#include <string>
#include <unordered_map>
#include <Scene_Base.h>
#include <EventState/EventState_Tutorial_CollectStar.h>
#include <EventState/EventState_Tutorial_Goal.h>
#include <EventState/EventState_Tutorial_EatEnemy.h>
#include <EventState/EventState_Tutorial_ThrowEgg.h>

// イベントの発生関数テーブル
struct EventFunctionTable {
    static std::unordered_map<std::string, void(*)(Scene_Base*)> tableMap_;
};