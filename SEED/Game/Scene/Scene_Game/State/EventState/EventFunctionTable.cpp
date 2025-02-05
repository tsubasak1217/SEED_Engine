#include "EventFunctionTable.h"

// static変数の実体
std::unordered_map<std::string, void(*)(Scene_Base*)> EventFunctionTable::tableMap_ = {
    { "Tutorial_CollictStar", [](Scene_Base* pScene){pScene->CauseEvent(new EventState_Tutorial_CollectStar(pScene)); } },
    { "Tutorial_ThrowEgg",    [](Scene_Base* pScene){pScene->CauseEvent(new EventState_Tutorial_ThrowEgg(pScene)); } },
    { "Tutorial_Goal",        [](Scene_Base* pScene){pScene->CauseEvent(new EventState_Tutorial_Goal(pScene)); } },
    { "Tutorial_EatEnemy",    [](Scene_Base* pScene){pScene->CauseEvent(new EventState_Tutorial_EatEnemy(pScene)); } },
    { "Tutorial_Corpse",      [](Scene_Base* pScene){pScene->CauseEvent(new EventState_Tutorial_Corpse(pScene)); } },
    { "Tutorial_Switch",      [](Scene_Base* pScene){pScene->CauseEvent(new EventState_Tutorial_Switch(pScene)); } },
    { "Tutorial_ThrowLength", [](Scene_Base* pScene){pScene->CauseEvent(new EventState_Tutorial_ThrowLength(pScene)); } }
};