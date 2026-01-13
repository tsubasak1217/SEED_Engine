#include <SEED/Lib/Shapes/Triangle.h>
#include <SEED/Source/SEED.h>


/////////////////////////////////////////////////////////////////////////////////////
/*                                       3D                                        */
/////////////////////////////////////////////////////////////////////////////////////

Triangle::Triangle(){
}

Triangle::Triangle(
    const Vector3& v1, const Vector3& v2, const Vector3& v3, 
    const Vector3& scale, const Vector3& rotate, 
    const Vector3& translate, const Color& color, BlendMode blendMode
){
    localVertex[0] = v1;
    localVertex[1] = v2;
    localVertex[2] = v3;
    this->scale = scale;
    this->rotate = rotate;
    this->translate = translate;
    this->color = color;
    this->blendMode = blendMode;
    uvTransform = IdentityMat4();
    GH = TextureManager::LoadTexture("DefaultAssets/white1x1.png");
}



/////////////////////////////////////////////////////////////////////////////////////
/*                                        2D                                       */
/////////////////////////////////////////////////////////////////////////////////////


Triangle2D::Triangle2D(){
}

Triangle2D::Triangle2D(
    const Vector2& v1, const Vector2& v2, const Vector2& v3,
    const Vector2& scale,
    float rotate,
    const Vector2& translate,
    const Color& color,
    BlendMode blendMode
){
    localVertex[0] = v1;
    localVertex[1] = v2;
    localVertex[2] = v3;
    this->scale = scale;
    this->rotate = rotate;
    this->translate = translate;
    this->color = color;
    this->blendMode = blendMode;
    lightingType = LIGHTINGTYPE_NONE;
    GH = TextureManager::LoadTexture("DefaultAssets/white1x1.png");
    uvTransform = IdentityMat4();
    isStaticDraw = false;
}

Matrix4x4 Triangle2D::GetWorldMatrix()const{

    Matrix4x4 worldMat =
        AffineMatrix({ scale.x,scale.y,1.0f }, { 0.0f, 0.0f, rotate }, { translate.x,translate.y,0.0f });
    return worldMat;
}

