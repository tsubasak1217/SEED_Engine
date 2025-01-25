#pragma once

/// stl
#include <memory>
#include <string>
// local
#include "Sprite.h"
// state
class UiState;

class UI
{
public:
    UI(const std::string &_name);
    ~UI();

    void Initialize(const std::string &_filePath);
    void Update();
    void Draw();
    void Finalize();

    void BeginFrame();
    void EndFrame();

private:
    std::string name_;
    std::unique_ptr<Sprite> sprite_;
    std::unique_ptr<UiState> state_;

public:
    Sprite *GetSprite() { return sprite_.get(); }

    void SetState(std::unique_ptr<UiState> _state) { state_ = std::move(_state); }
};

using ui = UI;