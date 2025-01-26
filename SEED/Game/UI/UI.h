#pragma once

/// stl
#include <memory>
#include <string>
// local
#include "Sprite.h"
// state
#include "State/IUiState.h"

class UI
{
public:
    UI(const std::string &_name);
    virtual ~UI();

    virtual void Initialize(const std::string &_filePath);
    virtual void Update();
    virtual void Draw();
    virtual void Finalize();

    virtual void BeginFrame();
    virtual void EndFrame();

protected:
    std::string name_;
    std::unique_ptr<Sprite> sprite_;

    std::unique_ptr<IUiState> state_;

public:
    Sprite *GetSprite() { return sprite_.get(); }

    void SetState(std::unique_ptr<IUiState> _state) { state_ = std::move(_state); }
};

using ui = UI;