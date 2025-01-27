#pragma once
// parent
#include "UI.h"
/// local
// lib
class Input;

class ButtonUI
    : public UI{
public:
    ButtonUI(const std::string& _name);
    ~ButtonUI();

    void Initialize(const std::string& _filePath) override;
    void Update() override;
    void Draw() override;
    void Finalize() override;

    void BeginFrame() override;
    void EndFrame() override;

private:
    bool isHovered_ = false;
    bool isPressed_ = false;
    bool isClicked_ = false;
    bool isReleased_ = false;

    const Input* input_ = nullptr;

public:
    bool IsHovered() const{ return isHovered_; }
    bool IsPressed() const{ return isPressed_; }
    bool IsClicked() const{ return isClicked_; }
    bool IsReleased() const{ return isReleased_; }
};