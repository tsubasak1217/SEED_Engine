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
    void SetColorX(const float _colorX){ sprite_->color.x = _colorX; }
    void SetColorY(const float _colorY){ sprite_->color.y = _colorY; }
    void SetColorZ(const float _colorZ){ sprite_->color.z = _colorZ; }
    void SetColorW(const float _colorW){ sprite_->color.w = _colorW; }

    const Vector2& GetScale()const{ return sprite_->scale; }
    void SetScale(const Vector2& _scale){ sprite_->scale = _scale; }
    void SetScaleX(const float _scaleX){ sprite_->scale.x = _scaleX; }
    void SetScaleY(const float _scaleY){ sprite_->scale.y = _scaleY; }

    const float GetRotation()const{ return sprite_->rotate; }
    void SetRotation(const float _rotation){ sprite_->rotate = _rotation; }

    const Vector2& GetTranslate()const{ return sprite_->translate; }
    void SetTranslate(const Vector2& _translate){ sprite_->translate = _translate; }
    void SetTranslateX(const float _translateX){ sprite_->translate.x = _translateX; }
    void SetTranslateY(const float _translateY){ sprite_->translate.y = _translateY; }

    const Vector2& GetAnchorPoint()const{ return sprite_->anchorPoint; }
    void SetAnchorPoint(const Vector2& _anchorPoint){ sprite_->anchorPoint = _anchorPoint; }
    void SetAnchorPointX(const float _anchorPointX){ sprite_->anchorPoint.x = _anchorPointX; }
    void SetAnchorPointY(const float _anchorPointY){ sprite_->anchorPoint.y = _anchorPointY; }

    const Vector2& GetSize()const{ return sprite_->size; }
    void SetSize(const Vector2& _size){ sprite_->size = _size; }
    void SetSizeX(const float _sizeX){ sprite_->size.x = _sizeX; }
    void SetSizeY(const float _sizeY){ sprite_->size.y = _sizeY; }

    const Vector2& GetClipLT()const{ return sprite_->clipLT; }
    void SetClipLT(const Vector2& _clipLT){ sprite_->clipLT = _clipLT; }
    void SetClipLTX(const float _clipLTX){ sprite_->clipLT.x = _clipLTX; }
    void SetClipLTY(const float _clipLTY){ sprite_->clipLT.y = _clipLTY; }

    const Vector2& GetClipSize()const{ return sprite_->clipSize; }
    void SetClipSize(const Vector2& _clipSize){ sprite_->clipSize = _clipSize; }
    void SetClipSizeX(const float _clipSizeX){ sprite_->clipSize.x = _clipSizeX; }
    void SetClipSizeY(const float _clipSizeY){ sprite_->clipSize.y = _clipSizeY; }

};

using ui = UI;