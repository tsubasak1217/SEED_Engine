#pragma once
// external
//#include <mutex>

// local
#include <DxManager.h>
#include <ImGuiManager.h>
#include <WindowManager.h>
#include <AudioManager.h>
#include <ClockManager.h>
#include <DirectionalLight.h>
#include <Triangle.h>
#include <Quad.h>
#include <Sprite.h>
#include <Camera.h>
#include <PolygonManager.h>
#include <InputManager.h>
#include <ModelManager.h>
#include <TextureManager.h>

class SEED{

    friend TextureManager;

    /////////////////////////////////////////////////////////////////////////////////////
    /*                                     基本の関数                                    */
    /////////////////////////////////////////////////////////////////////////////////////
private:
    SEED(){};// singletonのためにprivateに置く
    SEED(const SEED& other) = delete;
    SEED& operator=(const SEED&) = delete;

public:
    ~SEED();
    static void Initialize(HINSTANCE hInstance, int nCmdShow, const char* windowTitle, int clientWidth, int clientHeight);
    static void Finalize();
    static SEED* GetInstance();
    static void BeginFrame();
    static void EndFrame();


    /////////////////////////////////////////////////////////////////////////////////////
    /*                                 いろーんな関数                                    */
    /////////////////////////////////////////////////////////////////////////////////////
private:

    // テクスチャを読み込む関数(返り値はグラフハンドル)
    static uint32_t LoadTexture(const std::string& filename);
    void StartUpLoad();
    
public:
    
    // 画像の縦横幅を取得する関数
    static Vector2 GetImageSize(const std::wstring& fileName);
    // 画面の解像度を変更する関数(0.0f ~ 1.0f)
    static void ChangeResolutionRate(float resolutionRate);


    /////////////////////////////////////////////////////////////////////////////////////
    /*                          このエンジンが用意する描画関数                              */
    /////////////////////////////////////////////////////////////////////////////////////
public:

    // 解像度の変更を反映するかどうかのモード
    enum RESOLUTION_MODE{
        STATIC_DRAW,// 変更しない
        DYNAMIC_DRAW// 変更する
    };

    /*==========================三角形の描画関数==========================*/

    static void DrawTriangle(
        const Vector3& v1, const Vector3& v2, const Vector3& v3,
        const Vector4& color, uint32_t GH = SEED::LoadTexture("white1x1.png"),
        LIGHTING_TYPE lightingType = LIGHTINGTYPE_HALF_LAMBERT
    );

    static void DrawTriangle(
        const Vector4& v1, const Vector4& v2, const Vector4& v3,
        const Vector4& color, uint32_t GH = SEED::LoadTexture("white1x1.png"),
        LIGHTING_TYPE lightingType = LIGHTINGTYPE_HALF_LAMBERT
    );

    static void DrawTriangle(
        const Vector4& v1, const Vector4& v2, const Vector4& v3,
        const Vector3& scale, const Vector3& rotate, const Vector3& translate,
        const Vector4& color, uint32_t GH = SEED::LoadTexture("white1x1.png"),
        LIGHTING_TYPE lightingType = LIGHTINGTYPE_HALF_LAMBERT
    );

    static void DrawTriangle(
        const Vector3& v1, const Vector3& v2, const Vector3& v3,
        const Vector3& scale, const Vector3& rotate, const Vector3& translate,
        const Vector4& color, uint32_t GH = SEED::LoadTexture("white1x1.png"),
        LIGHTING_TYPE lightingType = LIGHTINGTYPE_HALF_LAMBERT
    );

    static void DrawTriangle(const Triangle& triangle, const Vector4& color, uint32_t GH = SEED::LoadTexture("white1x1.png"));
    static void DrawTriangle(const Triangle& triangle, uint32_t GH = SEED::LoadTexture("white1x1.png"));
 
    // 2D三角形
    static void DrawTriangle2D(
        const Vector2& v1, const Vector2& v2, const Vector2& v3,
        const Vector4& color, uint32_t GH = 0, RESOLUTION_MODE resolutionMode = STATIC_DRAW
    );
    static void DrawTriangle2D(
        const Triangle2D& triangle, const Vector4& color, 
        uint32_t GH = SEED::LoadTexture("white1x1.png"), RESOLUTION_MODE resolutionMode = STATIC_DRAW
    );

    static void DrawTriangle2D(const Triangle2D& triangle);

    /*=========================矩形の描画関数========================*/

    static void DrawQuad(const Quad& quad,const uint32_t GH = SEED::LoadTexture("white1x1.png"));

    static void DrawSprite(
        const Vector2& leftTop, const Vector2& size,
        uint32_t GH = SEED::LoadTexture("white1x1.png"),
        const Vector4& color = {1.0f,1.0f,1.0f,1.0f}, const Matrix4x4& uvTranfosrm = IdentityMat4(),
        RESOLUTION_MODE resolutionMode = STATIC_DRAW
    );

    static void DrawSprite(const Sprite& sprite);

    /*==========================モデルの描画関数==========================*/

    static void DrawModel(Model* model);


    /////////////////////////////////////////////////////////////////////////////////////
    /*                                     メンバ変数                                    */
    /////////////////////////////////////////////////////////////////////////////////////

private:// インスタンス

    static SEED* instance_;

private:// マネージャたち

    LeakChecker* leakChecker_ = nullptr;
    WindowManager* windowManager_ = nullptr;
    DxManager* dxManager_ = nullptr;
    ImGuiManager* imguiManager_ = nullptr;

private:// ウインドウに関する変数

    static HWND hwnd;
    static HINSTANCE hInstance_;
    static int nCmdShow_;
    static MSG msg_;

public:
    int kClientWidth_;
    int kClientHeight_;
    std::string windowTitle_;
    uint32_t windowBackColor_;

private:// 外部を参照するためのポインタ変数

    PolygonManager* pPolygonManager_;


    /////////////////////////////////////////////////////////////////////////////////////
    /*                                 アクセッサ関数                                    */
    /////////////////////////////////////////////////////////////////////////////////////
public:

    static void SetPolygonManagerPtr(PolygonManager* ptr){ instance_->pPolygonManager_ = ptr; }
    static DxManager* GetDxManager(){ return instance_->dxManager_; }
    static Camera* GetCamera(){ return GetInstance()->dxManager_->GetCamera(); }

    static HWND GetHWND(){ return hwnd; }
    static void SetWindowHandle(HWND handle){ hwnd = handle; }
    static int GetCmdShow(){ return nCmdShow_; }
    static HINSTANCE GetHINSTANCE(){ return hInstance_; }
    static UINT ProcessMessage(){ return msg_.message; }
    static void SetWindowColor(uint32_t color){ GetInstance()->windowBackColor_ = color; }
    static uint32_t GetWindowColor(){ return GetInstance()->windowBackColor_; }
    static DirectionalLight* GetDirectionalLight(){ return GetInstance()->dxManager_->directionalLight; }
};