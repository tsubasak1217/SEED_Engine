#pragma once

// environment
#include <Environment.h>

// Managers
#include <DxManager.h>
#include <ImGuiManager.h>
#include <WindowManager.h>
#include <AudioManager.h>
#include <ClockManager.h>
#include <PolygonManager.h>
#include <ParticleManager.h>
#include <InputManager.h>
#include <ModelManager.h>
#include <TextureManager.h>
#include <ViewManager.h>

// structs
#include <DirectionalLight.h>
#include <Triangle.h>
#include <Quad.h>
#include <Sprite.h>
#include <BaseCamera.h>
#include <BlendMode.h>
#include "AABB.h"
#include "OBB.h"

// math
#include <ShapeMath.h>

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
    static void Initialize(int clientWidth, int clientHeight);
    static void Finalize();
    static SEED* GetInstance();
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
    // 2D三角形
    static void DrawTriangle2D(const Triangle2D& triangle);

    /*=========================矩形の描画関数========================*/

    // 3D矩形
    static void DrawQuad(const Quad& quad);
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
    // スプライン曲線の描画
    static void DrawSpline(const std::vector<Vector3>& points, uint32_t subdivision, const Vector4& color = { 1.0f,1.0f,1.0f,1.0f },bool isControlPointVisible = true);

    /////////////////////////////////////////////////////////////////////////////////////
    /*                                その他細かい関数                                   */
    /////////////////////////////////////////////////////////////////////////////////////
private:
    // 起動時の読み込み関数
    void StartUpLoad();

public:
    // 画像の縦横幅を取得する関数
    static Vector2 GetImageSize(const std::wstring& fileName);
    // 画面の解像度を変更する関数(0.0f ~ 1.0f)
    static void ChangeResolutionRate(float resolutionRate);


    /////////////////////////////////////////////////////////////////////////////////////
    /*                                 アクセッサ関数                                    */
    /////////////////////////////////////////////////////////////////////////////////////
public:

    static void SetPolygonManagerPtr(PolygonManager* ptr){ instance_->pPolygonManager_ = ptr; }
    static BaseCamera* GetCamera(){ return DxManager::GetInstance()->GetCamera(); }
    static void SetCamera(const std::string& cameraName){ DxManager::GetInstance()->SetCamera(cameraName); }

    static void SetWindowColor(uint32_t color){ GetInstance()->windowBackColor_ = color; }
    static uint32_t GetWindowColor(){ return GetInstance()->windowBackColor_; }
    static const std::wstring& GetWindowTitle(){ return GetInstance()->windowTitle_; }
    static DirectionalLight* GetDirectionalLight(){ return DxManager::GetInstance()->directionalLight; }


    /////////////////////////////////////////////////////////////////////////////////////
    /*                                     メンバ変数                                    */
    /////////////////////////////////////////////////////////////////////////////////////

private:// インスタンス
    static SEED* instance_;

private:// 監視用変数

public:// ウインドウに関する変数
    int kClientWidth_;
    int kClientHeight_;
    static std::wstring windowTitle_;
    static uint32_t windowBackColor_;

private:// 外部を参照するためのポインタ変数

    PolygonManager* pPolygonManager_;

};