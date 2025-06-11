#include <SEED/Lib/Shapes/Quad.h>
#include <SEED/Source/SEED.h>

Quad::Quad(
    const Vector3& v1, const Vector3& v2, const Vector3& v3, const Vector3& v4,
    const Vector3& scale, const Vector3& rotate, const Vector3& translate,
    const Vector4& color, BlendMode blendMode
){
    localVertex[0] = v1;
    localVertex[1] = v2;
    localVertex[2] = v3;
    localVertex[3] = v4;

    this->scale = scale;
    this->rotate = rotate;
    this->translate = translate;

    this->color = color;
    this->blendMode = blendMode;
    lightingType = LIGHTINGTYPE_HALF_LAMBERT;
    GH = TextureManager::LoadTexture("DefaultAssets/white1x1.png");
    uvTransform = IdentityMat4();
}

Quad2D::Quad2D(
    const Vector2& v1, const Vector2& v2, const Vector2& v3, const Vector2& v4, 
    const Vector2& scale,float rotate, const Vector2& translate, 
    const Vector4& color, BlendMode blendMode
){
    localVertex[0] = v1;
    localVertex[1] = v2;
    localVertex[2] = v3;
    localVertex[3] = v4;

    this->scale = scale;
    this->rotate = rotate;
    this->translate = translate;

    this->color = color;
    this->blendMode = blendMode;
    lightingType = LIGHTINGTYPE_NONE;
    GH = TextureManager::LoadTexture("DefaultAssets/white1x1.png");
    uvTransform = IdentityMat4();
}
