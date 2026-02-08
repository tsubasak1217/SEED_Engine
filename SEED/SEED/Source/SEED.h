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
#include <SEED/Source/Manager/ParticleManager/CPUParticle/ParticleManager.h>
#include <SEED/Source/Manager/InputManager/InputManager.h>
#include <SEED/Source/Manager/ModelManager/ModelManager.h>
#include <SEED/Source/Manager/TextureManager/TextureManager.h>
#include <SEED/Source/Manager/TextSystem/TextSystem.h>
#include <SEED/Source/Manager/DxManager/ViewManager.h>

// structs
#include <SEED/Lib/Structs/DirectionalLight.h>
#include <SEED/Lib/Shapes/Triangle.h>
#include <SEED/Lib/Shapes/Quad.h>
#include <SEED/Lib/Structs/Sprite.h>
#include <SEED/Source/Basic/Camera/BaseCamera.h>
#include <SEED/Lib/Structs/blendMode.h>
#include <SEED/Lib/Shapes/AABB.h>
#include <SEED/Lib/Shapes/OBB.h>
#include <SEED/Lib/Shapes/Ring.h>
#include <SEED/Lib/Structs/SkyBox.h>
#include <SEED/Lib/Structs/TextBox.h>

// math
#include <SEED/Lib/Functions/Math.h>
#include <SEED/Lib/Functions/MatrixMath.h>
#include <SEED/Lib/Functions/ShapeMath.h>
#include <SEED/Lib/Functions/RandomFunc.h>
#include <SEED/Lib/Functions/FileFunc.h>
#include <SEED/Lib/Functions/StringFunc.h>
#include <SEED/Lib/Functions/SRTFunc.h>
#include <SEED/Lib/Functions/Easing.h>

// dict
#include <SEED/Source/Basic/Dictionary/AudioDictionary.h>
#include <SEED/Source/Basic/Dictionary/LayerDictionary.h>

// external
#include <json.hpp>

#ifdef _DEBUG
#define USE_SUB_WINDOW
#endif // _DEBUG


/// <summary>
/// エンジンの大元のクラス。エンジン側の各マネージャなどの更新や描画を行う
/// </summary>
namespace SEED{
    class Instance{
        friend TextureManager;
        friend class DxManager;

        /////////////////////////////////////////////////////////////////////////////////////
        /*                                     基本の関数                                    */
        /////////////////////////////////////////////////////////////////////////////////////
    private:
        Instance() = default;
        Instance(const Instance& other) = delete;
        Instance& operator=(const Instance&) = delete;

    public:
        ~Instance();
        static void Initialize(int clientWidth, int clientHeight, HINSTANCE hInstance, int nCmdShow);
        static void Finalize();
        static Instance* GetInstance();
        static void Update();
        static void Draw();
        static void BeginFrame();
        static void EndFrame();

    private:
        void DrawGUI();

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
        static void DrawTriangle(const Topology::Triangle& triangle);
        static void AddTriangle3DPrimitive(
            const Vector4& v1, const Vector4& v2, const Vector4& v3,
            const Vector2& texCoordV1, const Vector2& texCoordV2, const Vector2& texCoordV3, const Color& color,
            uint32_t GH, BlendMode blendMode, int32_t lightingType, const Matrix4x4& uvTransform,
            D3D12_CULL_MODE cullMode = D3D12_CULL_MODE::D3D12_CULL_MODE_BACK
        );

        // 2D三角形
        static void DrawTriangle2D(const Topology::Triangle2D& triangle);

        /*=========================矩形の描画関数========================*/

        // 3D矩形
        static void DrawQuad(const Topology::Quad& quad);
        static void AddQuad3DPrimitive(
            const Vector4& v1, const Vector4& v2, const Vector4& v3, const Vector4& v4,
            const Vector2& texCoordV1, const Vector2& texCoordV2, const Vector2& texCoordV3, const Vector2& texCoordV4,
            const Color& color, uint32_t GH, BlendMode blendMode, int32_t lightingType, const Matrix4x4& uvTransform,
            D3D12_CULL_MODE cullMode = D3D12_CULL_MODE::D3D12_CULL_MODE_BACK
        );

        // 2D矩形
        static void DrawQuad2D(const Topology::Quad2D& quad, const Color& masterColor = Color(1.0f));

        // スプライト
        static void DrawSprite(const Sprite& sprite, const std::optional<Color>& masterColor = std::nullopt);

        /*==========================モデルの描画関数==========================*/

        static void DrawModel(Model* model, const std::optional<Color>& masterColor = std::nullopt);

        /*==========================線の描画関数==========================*/

        // 3D線
        static void DrawLine(
            const Vector3& v1, const Vector3& v2,
            const Color& color = { 1.0f,1.0f,1.0f,1.0f }, BlendMode blendMode = BlendMode::NORMAL
        );

        // 2D線
        static void DrawLine2D(
            const Vector2& v1, const Vector2& v2,
            const Color& color = { 1.0f,1.0f,1.0f,1.0f }, BlendMode blendMode = BlendMode::NORMAL,
            bool isApplyViewMat = false, bool isIgnoreDepth = false
        );

        /*========================テキストの描画関数==========================*/

        // 2Dテキスト
        static void DrawText2D(const TextBox2D& textBox);
        // 3Dテキスト
        static void DrawText3D(const TextBox3D& textBox);

        /*==========================リングの描画関数==========================*/

        static void DrawRing(const Topology::Ring& ring);

        /*=========================デバッグ用の描画関数=======================*/

        // Topology::AABB, OBBの描画関数
        static void DrawAABB(const Topology::AABB& aabb, const Color& color = { 1.0f,1.0f,1.0f,1.0f });
        static void DrawAABB2D(const Topology::AABB2D& aabb, const Color& color = { 1.0f,1.0f,1.0f,1.0f }, bool isIgnoreDepth = false);
        static void DrawOBB(const Topology::OBB& obb, const Color& color = { 1.0f,1.0f,1.0f,1.0f });
        static void DrawOBB2D(const Topology::OBB2D& obb, const Color& color = { 1.0f,1.0f,1.0f,1.0f }, bool isIgnoreDepth = false);

        // 六角形の描画
        static void DrawHexagon(
            const Vector2& center, float radius, float theta,
            const Color& color = { 1.0f,1.0f,1.0f,1.0f },
            BlendMode blendMode = BlendMode::NORMAL,
            DrawLocation drawLocation = DrawLocation::Front,
            int32_t layer = 0, bool isApplyViewMat = true,
            bool isStaticDraw = false
        );

        static void DrawHexagonFrame(
            const Vector2& center, float radius, float theta, float frameWidthRate,
            const Color& color = { 1.0f,1.0f,1.0f,1.0f },
            BlendMode blendMode = BlendMode::NORMAL,
            DrawLocation drawLocation = DrawLocation::Front,
            int32_t layer = 0, bool isApplyViewMat = true,
            bool isStaticDraw = false
        );

        // 球の描画関数
        static void DrawSphere(const Vector3& center, const Vector3& radius, int32_t subdivision = 6, const Color& color = { 1.0f,1.0f,1.0f,1.0f });
        static void DrawSphere(const Vector3& center, float radius, int32_t subdivision = 6, const Color& color = { 1.0f,1.0f,1.0f,1.0f });
        // 円柱の描画関数
        static void DrawCylinder(const Vector3& start, const Vector3& end, float startRadius, float endRadius, int32_t subdivision = 6, const Color& color = { 1.0f,1.0f,1.0f,1.0f });
        // カプセルの描画関数
        static void DrawCapsule(const Vector3& start, const Vector3& end, float radius, int32_t subdivision = 6, const Color& color = { 1.0f,1.0f,1.0f,1.0f });
        static void DrawCapsule(const Vector3& start, const Vector3& end, const Vector3& radius, int32_t subdivision = 6, const Color& color = { 1.0f,1.0f,1.0f,1.0f });
        // デバッグ用のグリッド描画関数
        static void DrawGrid(float gridInterval = 10.0f, int32_t gridCount = 200);
        // ベジェ曲線の描画
        static void DrawBezier(const Vector3& p1, const Vector3& p2, const Vector3& p3, uint32_t subdivision, const Color& color = { 1.0f,1.0f,1.0f,1.0f });
        static void DrawBezier(const Vector2& p1, const Vector2& p2, const Vector2& p3, uint32_t subdivision, const Color& color = { 1.0f,1.0f,1.0f,1.0f });
        static void DrawBezier(const std::vector<Vector2>& points, uint32_t subdivision, const Color& color = { 1.0f,1.0f,1.0f,1.0f });
        // スプライン曲線の描画
        static void DrawSpline(const std::vector<Vector3>& points, uint32_t subdivision, const Color& color = { 1.0f,1.0f,1.0f,1.0f }, bool isControlPointVisible = true);
        static void DrawSpline(const std::vector<Vector2>& points, uint32_t subdivision, const Color& color = { 1.0f,1.0f,1.0f,1.0f }, bool isControlPointVisible = true);
        // ライトのデバッグ用描画
        static void DrawLight(const BaseLight* light);
        // SkyBoxの描画関数
        static void SetSkyBox(const std::string& textureName, const Color& color = { 1.0f,1.0f,1.0f,1.0f });
        static void DrawSkyBox(bool isFollowCameraPos = true, const Vector3& position = { 0.0f,0.0f,0.0f }, float scale = 100.0f);

        /////////////////////////////////////////////////////////////////////////////////////
        /*                                その他細かい関数                                   */
        /////////////////////////////////////////////////////////////////////////////////////
    private:
        //
        void SetImGuiEmptyWindows();
        // 起動時の読み込み関数
        void StartUpLoad();

    public:
        // 画像の縦横幅を取得する関数
        static Vector2 GetImageSize(const std::wstring& fileName);
        // 画面の解像度を変更する関数(0.0f ~ 1.0f)
        static void ChangeResolutionRate(float resolutionRate);
        static float GetResolutionRate(){ return DxManager::GetInstance()->GetResolutionRate(); }
        // カメラにシェイクを設定する関数
        static void SetCameraShake(float time, float power, const Vector3& shakeLevel = { 1.0f,1.0f,1.0f });

        /////////////////////////////////////////////////////////////////////////////////////
        /*                                 アクセッサ関数                                    */
        /////////////////////////////////////////////////////////////////////////////////////
    public:

        // カメラ関連
        static BaseCamera* GetMainCamera(){ return DxManager::GetInstance()->mainCamera_; }
        static BaseCamera* GetCamera(const std::string& cameraName){ return DxManager::GetInstance()->GetCamera(cameraName); }
        static void SetMainCamera(const std::string& cameraName){ DxManager::GetInstance()->SetMainCamera(cameraName); }
        static std::string GetMainCameraName(){ return DxManager::GetInstance()->GetMainCameraName(); }
        static void RegisterCamera(const std::string& cameraName, BaseCamera* pCamera){ DxManager::GetInstance()->RegisterCamera(cameraName, pCamera); }
        static void RemoveCamera(const std::string& cameraName){ DxManager::GetInstance()->RemoveCamera(cameraName); }
        static void SetIsCameraActive(const std::string& cameraName, bool isActive){ DxManager::GetInstance()->SetIsCameraActive(cameraName, isActive); }
        // ライト
        static void SendLightData(BaseLight* light){ instance_->pPolygonManager_->AddLight(light); }
        // ウィンドウ
        static void SetWindowColor(uint32_t color){ GetInstance()->windowBackColor_ = color; }
        static void SetWindowColor(const Color& color){ GetInstance()->windowBackColor_ = color.GetColorCode(); }
        static uint32_t GetWindowColor(){ return GetInstance()->windowBackColor_; }
        static const std::wstring& GetWindowTitle(){ return GetInstance()->windowTitle_; }
        // アプリケーション終了フラグ
        static bool GetIsEndAplication(){ return instance_->isEndAplication_; }

        /////////////////////////////////////////////////////////////////////////////////////
        /*                                     メンバ変数                                    */
        /////////////////////////////////////////////////////////////////////////////////////

    private:// インスタンス
        static Instance* instance_;
        bool isDebugCamera_ = false;
        float resolutionRate_ = 1.0f;
        bool isGridVisible_ = false;
        bool isEndAplication_ = false;
        Vector4 clearColor_;

    private:// object
        // デフォルト用意のカメラ
        std::unique_ptr<BaseCamera> defaultCamera_;
        std::unique_ptr<DebugCamera> debugCamera_;

    private:
        std::unique_ptr<Sprite> offscreenWrapper_;

    public:// ウインドウに関する変数
        int kClientWidth_;// game用
        int kClientHeight_;
        int kSystemClientWidth_ = 1920;// editor用
        int kSystemClientHeight_ = 1020;
        static std::wstring windowTitle_;
        static std::wstring systemWindowTitle_;
        static uint32_t windowBackColor_;

    private:// 外部を参照するためのポインタ変数

        PolygonManager* pPolygonManager_;

    };

    Instance* GetInstance();
}