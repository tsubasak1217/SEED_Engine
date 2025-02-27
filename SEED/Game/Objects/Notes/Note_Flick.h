#pragma once
#include <Game/Objects/Notes/Note_Base.h>

class Note_Flick : public Note_Base{
public:
    Note_Flick();
    ~Note_Flick() override;
    void Update() override;
    void Draw() override;
};