#pragma once
#include <Game/Objects/Notes/Note_Base.h>

class Note_Hold : public Note_Base{
public:
    Note_Hold();
    ~Note_Hold() override;
    void Update() override;
    void Draw() override;
};