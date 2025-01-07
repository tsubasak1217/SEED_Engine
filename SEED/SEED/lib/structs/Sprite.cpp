#include "Sprite.h"
#include "SEED.h"
#include "DxFunc.h"

Sprite::Sprite(){
    leftTop = { 0.0f,0.0f };
    size = { 100.0f,100.0f };
    scale = { 1.0f,1.0f };
    rotate = 0.0f;
    translate = { 0.0f,0.0f };
    anchorPoint = { 0.0f,0.0f };
    clipLT = { 0.0f,0.0f };
    clipSize = { 0.0f,0.0f };
    color = { 1.0f,1.0f,1.0f,1.0f };
    GH = TextureManager::LoadTexture("Assets/white1x1.png");
    blendMode = BlendMode::NORMAL;
    uvTransform = IdentityMat4();
    isStaticDraw = true;
}

Sprite::Sprite(const std::string& filename) : Sprite::Sprite(){
    GH = TextureManager::LoadTexture(filename);
    size = SEED::GetImageSize(ConvertString(filename));
}

Sprite::Sprite(const std::string& filename, const Vector2& leftTop, const Vector2& size)
    : Sprite::Sprite(filename){
    this->leftTop = leftTop;
    this->size = size;
}

void Sprite::Draw(){
    SEED::DrawSprite(*this);
}

Matrix4x4 Sprite::GetWorldMatrix() const{

    Vector3 offset;

    if(MyMath::Length(clipSize) == 0.0f){
        offset = {
            leftTop.x + size.x * anchorPoint.x,
            leftTop.y + size.y * anchorPoint.y,
            0.0f
        };
    } else{
        offset = {
            leftTop.x + clipSize.x * anchorPoint.x,
            leftTop.y + clipSize.y * anchorPoint.y,
            0.0f
        };
    }


    Matrix4x4 worldMat =
        AffineMatrix({ scale.x,scale.y,1.0f }, { 0.0f, 0.0f, rotate }, { translate.x + offset.x,translate.y + offset.y,0.0f });
    return worldMat;
}