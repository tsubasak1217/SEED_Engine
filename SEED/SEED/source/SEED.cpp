// local
#include <SEED.h>
#include <DxFunc.h>
#include <MatrixFunc.h>
#include <MyMath.h>
#include <ShapeMath.h>
#include <includes.h>
#include <Environment.h>
#include <SceneManager.h>
//

/*----------------------------------- static変数の初期化----------------------------------------*/

HWND SEED::hwnd = nullptr;
HINSTANCE SEED::hInstance_ = nullptr;
int SEED::nCmdShow_ = 0;
MSG SEED::msg_ = {};
SEED* SEED::instance_ = nullptr;

/*---------------------------------コンストラクタ、デストラクタ------------------------------------*/

SEED::~SEED(){
    delete imguiManager_;
    imguiManager_ = nullptr;
    delete windowManager_;
    windowManager_ = nullptr;
    delete leakChecker_;
    leakChecker_ = nullptr;
    delete instance_;
    instance_ = nullptr;
}

/*-------------------------------------- 初期化、終了処理 -----------------------------------------*/

void SEED::Initialize(HINSTANCE hInstance, int nCmdShow, const char* windowTitle, int clientWidth, int clientHeight){

    instance_ = GetInstance();

    instance_->hInstance_ = hInstance;
    instance_->nCmdShow_ = nCmdShow;
    instance_->windowTitle_ = windowTitle;
    instance_->kClientWidth_ = clientWidth;
    instance_->kClientHeight_ = clientHeight;
    instance_->windowBackColor_ = 0x47ada3ff;

    CameraManager::Initialize();
    instance_->leakChecker_ = new LeakChecker();
    instance_->windowManager_ = new WindowManager();
    DxManager::GetInstance();
    instance_->imguiManager_ = new ImGuiManager();

    instance_->windowManager_->Initialize(instance_);
    DxManager::GetInstance()->Initialize(instance_);
    instance_->imguiManager_->Initialize();

    ClockManager::Initialize();
    TextureManager::Initialize();
    AudioManager::Initialize();
    InputManager::Initialize();
    ModelManager::Initialize();
    SceneManager::Initialize();

    instance_->StartUpLoad();
}

void SEED::Finalize(){
    instance_->imguiManager_->Finalize();
    instance_->windowManager_->Finalize();
    DxManager::GetInstance()->Finalize();
}

SEED* SEED::GetInstance(){
    static std::once_flag onceFlag;
    std::call_once(
        onceFlag,
        [](){
        if(!instance_){ instance_ = new SEED; }
    });

    return instance_;
}

/*------------------------------------ ゲームエンジン実行部分 --------------------------------------*/

/*-------------------------------------- ゲームループ内の関数 -----------------------------------------*/

void SEED::BeginFrame(){
    // ウインドウにメッセージがある場合、優先して処理する
    if(PeekMessage(&instance_->msg_, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&instance_->msg_);
        DispatchMessage(&instance_->msg_);
    }

    // 時間を保存
    ClockManager::BeginFrame();

    // 全入力情報を格納
    InputManager::GetAllInput();


    // imgui,directXのフレーム開始時処理
    instance_->imguiManager_->Begin();
    DxManager::GetInstance()->PreDraw();

    // カメラの更新
    CameraManager::Update();
}

void SEED::EndFrame(){
    DxManager::GetInstance()->DrawPolygonAll();
    instance_->imguiManager_->End();
    DxManager::GetInstance()->PostDraw();
    // 経過時間を取得
    ClockManager::EndFrame();
}

/////////////////////////////////////////////////////////////////////////////////////

/*                             テクスチャを読み込む関数                                */

/////////////////////////////////////////////////////////////////////////////////////

// 起動時読み込み
void SEED::StartUpLoad(){

    AudioManager::StartUpLoad();
    ModelManager::StartUpLoad();
    TextureManager::StartUpLoad();
}

// 画像の縦横幅を取得する関数
Vector2 SEED::GetImageSize(const std::wstring& fileName){
    
    // パスの結合
    std::wstring filePath = L"resources/textures/" + fileName;

    // 画像をロード
    Gdiplus::Image image(filePath.c_str());

    // 画像の幅と高さを取得
    int width = image.GetWidth();
    int height = image.GetHeight();

    return {float(width),float(height)};
}

void SEED::ChangeResolutionRate(float resolutionRate){
    DxManager::GetInstance()->ChangeResolutionRate(resolutionRate);
}

/*--------------------------------------------------------------------------------------------------*/
/*                                            描画関数                                               */
/*--------------------------------------------------------------------------------------------------*/

/*=============================================== 3D ===========================================*/

void SEED::DrawTriangle(const Vector4& v1, const Vector4& v2, const Vector4& v3, const Vector4& color, uint32_t GH,BlendMode blendMode, LIGHTING_TYPE lightingType){
    Triangle tri(TransformToVec3(v1), TransformToVec3(v2), TransformToVec3(v3));
    tri.color = color;
    tri.litingType = lightingType;
    tri.GH = GH;
    tri.blendMode = blendMode;
    SEED::DrawTriangle(tri);
}

void SEED::DrawTriangle(const Vector3& v1, const Vector3& v2, const Vector3& v3, const Vector4& color, uint32_t GH, BlendMode blendMode, LIGHTING_TYPE lightingType){
    Triangle tri(v1, v2, v3);
    tri.color = color;
    tri.litingType = lightingType;
    tri.GH = GH;
    tri.blendMode = blendMode;
    SEED::DrawTriangle(tri);
}

void SEED::DrawTriangle(
    const Vector4& v1, const Vector4& v2, const Vector4& v3,
    const Vector3& scale, const Vector3& rotate, const Vector3& translate,
    const Vector4& color, uint32_t GH, BlendMode blendMode, LIGHTING_TYPE lightingType
){
    Triangle tri(
        TransformToVec3(v1), TransformToVec3(v2), TransformToVec3(v3),
        scale, rotate, translate
    );

    tri.color = color;
    tri.litingType = lightingType;
    tri.GH = GH;
    tri.blendMode = blendMode;

    SEED::DrawTriangle(tri);
}

void SEED::DrawTriangle(
    const Vector3& v1, const Vector3& v2, const Vector3& v3,
    const Vector3& scale, const Vector3& rotate, const Vector3& translate,
    const Vector4& color, uint32_t GH, BlendMode blendMode, LIGHTING_TYPE lightingType
){
    Triangle tri(v1, v2, v3, scale, rotate, translate);
    tri.color = color;
    tri.GH = GH;
    tri.litingType = lightingType;
    tri.blendMode = blendMode;
    SEED::DrawTriangle(tri);
}

void SEED::DrawTriangle(const Triangle& triangle, const Vector4& color, uint32_t GH,BlendMode blendMode){
    Triangle tri = triangle;
    tri.color = color;
    tri.GH = GH;
    tri.blendMode = blendMode;
    SEED::DrawTriangle(tri);
}

void SEED::DrawTriangle(const Triangle& triangle){
    Matrix4x4 worldMat = AffineMatrix(triangle.scale, triangle.rotate, triangle.translate);
    instance_->pPolygonManager_->AddTriangle(
        TransformToVec4(triangle.localVertex[0]),
        TransformToVec4(triangle.localVertex[1]),
        TransformToVec4(triangle.localVertex[2]),
        worldMat, triangle.color, triangle.litingType, triangle.uvTransform, true, 
        triangle.GH,triangle.blendMode
    );
}

/*=============================================== 2D ===========================================*/

void SEED::DrawTriangle2D(
    const Vector2& v1, const Vector2& v2, const Vector2& v3,
    const Vector4& color, uint32_t GH, BlendMode blendMode, 
    RESOLUTION_MODE resolutionMode
){
    Triangle2D tri(v1, v2, v3);
    SEED::DrawTriangle2D(tri, color,GH,blendMode, resolutionMode);
}

void SEED::DrawTriangle2D(
    const Triangle2D& triangle, const Vector4& color, uint32_t GH,
    BlendMode blendMode, RESOLUTION_MODE resolutionMode
){
    Triangle2D tri = triangle;
    tri.color = color;
    tri.GH = GH;
    tri.blendMode = blendMode;
    tri.isStaticDraw = resolutionMode;
    SEED::DrawTriangle2D(tri);
}

void SEED::DrawTriangle2D(const Triangle2D& triangle){

    instance_->pPolygonManager_->AddTriangle(
        TransformToVec4(triangle.localVertex[0]),
        TransformToVec4(triangle.localVertex[1]),
        TransformToVec4(triangle.localVertex[2]),
        triangle.GetWorldMatrix(), triangle.color, LIGHTINGTYPE_NONE, triangle.uvTransform, false, triangle.GH,
        triangle.blendMode, triangle.isStaticDraw, triangle.drawLocation, triangle.layer
    );
}

/*=============================================== 矩形 ===========================================*/


void SEED::DrawQuad(const Quad& quad){
    Matrix4x4 worldMat = AffineMatrix(quad.scale, quad.rotate, quad.translate);
    instance_->pPolygonManager_->AddQuad(
        quad.localVertex[0],
        quad.localVertex[1],
        quad.localVertex[2],
        quad.localVertex[3],
        worldMat, quad.color, quad.lightingType, quad.uvTransform, true, quad.GH,quad.blendMode
    );
}

void SEED::DrawQuad2D(const Quad2D& quad){

    Matrix4x4 worldMat = AffineMatrix(quad.scale.ToVec3(), { 0.0f,0.0f,quad.rotate }, quad.translate.ToVec3());
    instance_->pPolygonManager_->AddQuad(
        quad.localVertex[0].ToVec3(),
        quad.localVertex[1].ToVec3(),
        quad.localVertex[2].ToVec3(),
        quad.localVertex[3].ToVec3(),
        worldMat, quad.color, quad.lightingType, quad.uvTransform, false, quad.GH, quad.blendMode,
        quad.isStaticDraw, quad.drawLocation, quad.layer
    );
}

/*========================================== スプライト ===========================================*/


void SEED::DrawSprite(const Sprite& sprite){

    instance_->pPolygonManager_->AddSprite(
        sprite.size,
        sprite.GetWorldMatrix(),
        sprite.GH,
        sprite.color,
        sprite.uvTransform,
        sprite.anchorPoint,
        sprite.clipLT,
        sprite.clipSize,
        sprite.blendMode,
        sprite.isStaticDraw,
        sprite.drawLocation,
        sprite.layer,
        false
    );
}


/*========================================== モデル ===========================================*/

void SEED::DrawModel(Model* model){
    instance_->pPolygonManager_->AddModel(model);
}


/*========================================== 線 ===========================================*/

void SEED::DrawLine(const Vector3& v1, const Vector3& v2, const Vector4& color, BlendMode blendMode){
    instance_->pPolygonManager_->AddLine(
        TransformToVec4(v1),
        TransformToVec4(v2),
        IdentityMat4(), color, true, blendMode,false
    );
}

void SEED::DrawLine2D(const Vector2& v1, const Vector2& v2, const Vector4& color, BlendMode blendMode){
    instance_->pPolygonManager_->AddLine(
        TransformToVec4(v1),
        TransformToVec4(v2),
        IdentityMat4(), color, false, blendMode,false,DrawLocation::Front,0
    );
}

void SEED::DrawGrid(float gridInterval, int32_t gridCount){

    float width = gridInterval * gridCount;
    Vector3 leftFront = { -width * 0.5f,0.0f,-width * 0.5f };

    for(int xIdx = 0; xIdx < gridCount + 1; xIdx++){
        SEED::DrawLine(
            leftFront + Vector3(gridInterval * xIdx, 0.0f, 0.0f),
            leftFront + Vector3(gridInterval * xIdx, 0.0f, width),
            { 1.0f,0.0f,0.0f,1.0f }
        );
    }

    for(int yIdx = 0; yIdx < gridCount + 1; yIdx++){
        SEED::DrawLine(
            leftFront + Vector3(0.0f, 0.0f, gridInterval * yIdx),
            leftFront + Vector3(width, 0.0f, gridInterval * yIdx),
            { 0.0f,0.0f,1.0f,1.0f }
        );
    }
}
