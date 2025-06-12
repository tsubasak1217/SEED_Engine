#include "Note_Base.h"
#include <SEED/Source/SEED.h>

Note_Base::Note_Base(){
    noteID_ = nextNoteID_++;
    noteQuad_ = std::make_unique<Quad>(MakeEqualQuad(1.0f, {1.0f,1.0f,1.0f,1.0f}));
}

void Note_Base::Draw(float currentTime, float appearLength){
    currentTime;
    appearLength;
    SEED::DrawQuad(*noteQuad_.get());
}
