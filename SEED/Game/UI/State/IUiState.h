#pragma once

// host
class UI;

class IUiState
{
public:
    IUiState(UI *_ui) : ui_(_ui) {}
    virtual ~IUiState() {}

    virtual void Initialize() = 0;
    virtual void Update() = 0;
    virtual void Draw() = 0;

    virtual void BeginFrame() = 0;
    virtual void EndFrame() = 0;

protected:
    UI *ui_;
};