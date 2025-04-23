#include "Note_Base.h"

Note_Base::Note_Base(){
    noteID_ = nextNoteID_++;
}

void Note_Base::Draw(){
    noteSprite_->Draw();
}
