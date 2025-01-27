#pragma once

/// stl
#include <memory>
#include <string>
// local
#include "Sprite.h"
// state
#include "State/IUiState.h"

class UI{
public:
    UI(const std::string& _name);
    virtual ~UI();

    virtual void Initialize(const std::string& _filePath);
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
    const std::string& GetName()const{ return name_; }

    Sprite* GetSprite() const{ return sprite_.get(); }

    void SetState(std::unique_ptr<IUiState> _state){
        state_ = std::move(_state);
        state_->Initialize();
    }

    const Vector4& GetColor()const{ return sprite_->color; }
    void SetColor(const Vector4& _color){ sprite_->color = _color; }

    const Vector2& GetScale()const{ return sprite_->scale; }
    void SetScale(const Vector2& _scale){ sprite_->scale = _scale; }

    const float GetRotation()const{ return sprite_->rotate; }
    void SetRotation(const float _rotation){ sprite_->rotate = _rotation; }

    const Vector2& GetTranslate()const{ return sprite_->translate; }
    void SetTranslate(const Vector2& _translate){ sprite_->translate = _translate; }

    const Vector2& GetAnchorPoint()const{ return sprite_->anchorPoint; }
    void SetAnchorPoint(const Vector2& _anchorPoint){ sprite_->anchorPoint = _anchorPoint; }

    const Vector2& GetSize()const{ return sprite_->size; }
    void SetSize(const Vector2& _size){ sprite_->size = _size; }
};

using ui = UI;