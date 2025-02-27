#pragma once
#include <Game/Objects/Notes/Note_Base.h>

class Note_Tap : public Note_Base{
public:
    Note_Tap();
    ~Note_Tap() override;
    void Update() override;
    void Draw() override;
};