#pragma once
#include <string>
#include <unordered_map>
#include <Scene_Base.h>

// イベントの発生関数テーブル
struct EventFunctionTable {
    EventFunctionTable();
    static std::unordered_map<std::string, void(*)(Scene_Base*)> tableMap_;
};