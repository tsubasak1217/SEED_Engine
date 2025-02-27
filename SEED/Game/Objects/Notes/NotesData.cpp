#include "NotesData.h"
#include <Game/Objects/Judgement/PlayerInput.h>
#include <Game/Objects/Judgement/Judgement.h>
#include <SEED/Lib/Structs/Range1D.h>

std::vector<std::weak_ptr<Note_Base>> NotesData::GetNearNotes(float time){

    // 整列されたnotesからtimeに近いノーツを取得
    std::vector<std::weak_ptr<Note_Base>> nearNotes;

    // 判定範囲を取得
    float borderTime = Judgement::GetInstance()->GetJudgeTime(Judgement::Evaluation::GOOD);
    Range1D range(time - borderTime, time + borderTime);

    // 範囲内にあるノーツを入れていく
    for(auto& note : notes_){
        if(MyFunc::IsContain(range, note.first)){
            nearNotes.push_back(note.second);
        }
    }

    return nearNotes;
}
