#pragma once

// environment
#include <Environment/Environment.h>
#include <Environment/Physics.h>

// Managers
#include <SEED/Source/Manager/DxManager/DxManager.h>
#include <SEED/Source/Manager/ImGuiManager/ImGuiManager.h>
#include <SEED/Source/Manager/WindowManager/WindowManager.h>
#include <SEED/Source/Manager/AudioManager/AudioManager.h>
#include <SEED/Source/Manager/ClockManager/ClockManager.h>
#include <SEED/Source/Manager/DxManager/PolygonManager.h>
#include <SEED/Source/Manager/ParticleManager/ParticleManager.h>
#include <SEED/Source/Manager/InputManager/InputManager.h>
#include <SEED/Source/Manager/ModelManager/ModelManager.h>
#include <SEED/Source/Manager/TextureManager/TextureManager.h>
#include <SEED/Source/Manager/DxManager/ViewManager.h>

// structs
#include <SEED/Lib/Structs/DirectionalLight.h>
#include <SEED/Lib/Shapes/Triangle.h>
#include <SEED/Lib/Shapes/Quad.h>
#include <SEED/Lib/Structs/Sprite.h>
#include <SEED/Source/Object/Camera/BaseCamera.h>
#include <SEED/Lib/Structs/blendMode.h>
#include <SEED/Lib/Shapes/AABB.h>
#include <SEED/Lib/Shapes/OBB.h>
#include <SEED/Lib/Shapes/Ring.h>

// math
#include <SEED/Lib/Functions/MyFunc/ShapeMath.h>

// external
#include <json.hpp>


class SEED{

    friend TextureManager;

    /////////////////////////////////////////////////////////////////////////////////////
    /*                                     基本の関数                                    */
    /////////////////////////////////////////////////////////////////////////////////////
private:
    SEED() = default;
    SEED(const SEED& other) = delete;
    SEED& operator=(const SEED&) = delete;

public:
    ~SEED();
    static void Initialize(int clientWidth, int clientHeight, HINSTANCE hInstance, int nCmdShow);
    static void Finalize();
    static SEED* GetInstance();
    static void Update();
    static void Draw();
    static void BeginFrame();
    static void EndFrame();

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

    // 3D三角形
    static void DrawTriangle(const Triangle& triangle);
    static void AddTriangle3DPrimitive(
        const Vector4& v1, const Vector4& v2, const Vector4& v3,
        const Vector2& texCoordV1, const Vector2& texCoordV2, const Vector2& texCoordV3, const Vector4& color,
        uint32_t GH, BlendMode blendMode, int32_t lightingType, const Matrix4x4& uvTransform,
        D3D12_CULL_MODE cullMode = D3D12_CULL_MODE::D3D12_CULL_MODE_BACK
    );

    // 2D三角形
    static void DrawTriangle2D(const Triangle2D& triangle);

    /*=========================矩形の描画関数========================*/

    // 3D矩形
    static void DrawQuad(const Quad& quad);
    static void AddQuad3DPrimitive(
        const Vector4& v1, const Vector4& v2, const Vector4& v3, const Vector4& v4,
        const Vector2& texCoordV1, const Vector2& texCoordV2, const Vector2& texCoordV3, const Vector2& texCoordV4,
        const Vector4& color, uint32_t GH, BlendMode blendMode, int32_t lightingType, const Matrix4x4& uvTransform,
        D3D12_CULL_MODE cullMode = D3D12_CULL_MODE::D3D12_CULL_MODE_BACK
    );

    // 2D矩形
    static void DrawQuad2D(const Quad2D& quad);
    // スプライト
    static void DrawSprite(const Sprite& sprite);

    /*==========================モデルの描画関数==========================*/

    static void DrawModel(Model* model);

    /*==========================線の描画関数==========================*/

    // 3D線
    static void DrawLine(
        const Vector3& v1, const Vector3& v2, 
        const Vector4& color = {1.0f,1.0f,1.0f,1.0f}, BlendMode blendMode = BlendMode::NORMAL
    );

    // 2D線
    static void DrawLine2D(
        const Vector2& v1, const Vector2& v2, 
        const Vector4& color = { 1.0f,1.0f,1.0f,1.0f }, BlendMode blendMode = BlendMode::NORMAL
    );

    /*==========================リングの描画関数==========================*/

    static void DrawRing(const Ring& ring);

    /*=========================デバッグ用の描画関数=======================*/

    // AABB, OBBの描画関数
    static void DrawAABB(const AABB& aabb, const Vector4& color = { 1.0f,1.0f,1.0f,1.0f });
    static void DrawOBB(const OBB& obb, const Vector4& color = { 1.0f,1.0f,1.0f,1.0f });

    // 球の描画関数
    static void DrawSphere(const Vector3& center, const Vector3& radius, int32_t subdivision = 6, const Vector4& color = { 1.0f,1.0f,1.0f,1.0f });
    static void DrawSphere(const Vector3& center, float radius,int32_t subdivision = 6, const Vector4& color = { 1.0f,1.0f,1.0f,1.0f });
    // 円柱の描画関数
    static void DrawCylinder(const Vector3& start, const Vector3& end, float radius, int32_t subdivision = 6, const Vector4& color = { 1.0f,1.0f,1.0f,1.0f });
    // カプセルの描画関数
    static void DrawCapsule(const Vector3& start, const Vector3& end, float radius, int32_t subdivision = 6, const Vector4& color = { 1.0f,1.0f,1.0f,1.0f });
    static void DrawCapsule(const Vector3& start, const Vector3& end, const Vector3& radius, int32_t subdivision = 6, const Vector4& color = { 1.0f,1.0f,1.0f,1.0f });
    // デバッグ用のグリッド描画関数
    static void DrawGrid(float gridInterval = 10.0f, int32_t gridCount = 200);
    // ベジェ曲線の描画
    static void DrawBezier(const Vector3& p1, const Vector3& p2, const Vector3& p3, uint32_t subdivision, const Vector4& color = { 1.0f,1.0f,1.0f,1.0f });
    // スプライン曲線の描画
    static void DrawSpline(const std::vector<Vector3>& points, uint32_t subdivision, const Vector4& color = { 1.0f,1.0f,1.0f,1.0f },bool isControlPointVisible = true);
    // ライトのデバッグ用描画
    static void DrawLight(const BaseLight* light);

    /////////////////////////////////////////////////////////////////////////////////////
    /*                                その他細かい関数                                   */
    /////////////////////////////////////////////////////////////////////////////////////
private:
    // 起動時の読み込み関数
    void StartUpLoad();
    // カーソルのリピートを行う関数
    void RepeatCursor();

public:
    // 画像の縦横幅を取得する関数
    static Vector2 GetImageSize(const std::wstring& fileName);
    // 画面の解像度を変更する関数(0.0f ~ 1.0f)
    static void ChangeResolutionRate(float resolutionRate);
    // カメラにシェイクを設定する関数
    static void SetCameraShake(float time, float power, const Vector3& shakeLevel = {1.0f,1.0f,1.0f});
    // マウスカーソルの表示・非表示を切り替える関数
    static void SetMouseCursorVisible(bool isVisible);
    static void ToggleMouseCursorVisible();

    /////////////////////////////////////////////////////////////////////////////////////
    /*                                 アクセッサ関数                                    */
    /////////////////////////////////////////////////////////////////////////////////////
public:

    static void SetPolygonManagerPtr(PolygonManager* ptr){ instance_->pPolygonManager_ = ptr; }
    static BaseCamera* GetCamera(){ return DxManager::GetInstance()->GetCamera(); }
    static void SetCamera(const std::string& cameraName){ DxManager::GetInstance()->SetCamera(cameraName); }
    static void SendLightData(BaseLight* light){ instance_->pPolygonManager_->AddLight(light); }
    static void SetWindowColor(uint32_t color){ GetInstance()->windowBackColor_ = color; }
    static uint32_t GetWindowColor(){ return GetInstance()->windowBackColor_; }
    static const std::wstring& GetWindowTitle(){ return GetInstance()->windowTitle_; }
    static void SetIsRepeatCursor(bool flag){instance_->isRepeatCursor_ = flag;}
    static void ToggleRepeatCursor(){ instance_->isRepeatCursor_ = !instance_->isRepeatCursor_; }

    /////////////////////////////////////////////////////////////////////////////////////
    /*                                     メンバ変数                                    */
    /////////////////////////////////////////////////////////////////////////////////////

private:// インスタンス
    static SEED* instance_;
    bool isRepeatCursor_ = false;
    bool isCursorVisible_ = true;

private:
    std::unique_ptr<Sprite> offscreenWrapper_;

public:// ウインドウに関する変数
    int kClientWidth_;
    int kClientHeight_;
    static std::wstring windowTitle_;
    static std::wstring systemWindowTitle_;
    static uint32_t windowBackColor_;

private:// 外部を参照するためのポインタ変数

    PolygonManager* pPolygonManager_;

};