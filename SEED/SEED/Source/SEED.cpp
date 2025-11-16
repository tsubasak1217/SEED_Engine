// local
#include <SEED/Source/SEED.h>
#include <SEED/Lib/Functions/DxFunc.h>
#include <SEED/Lib/Functions/MatrixFunc.h>
#include <SEED/Lib/Functions/MyMath.h>
#include <SEED/Lib/Functions/ShapeMath.h>
#include <SEED/Lib/Includes/includes.h>
#include <Environment/Environment.h>
#include <SEED/Source/Manager/SceneManager/SceneManager.h>
#include <SEED/Source/Manager/CollisionManager/CollisionManager.h>
#include <SEED/Source/Manager/ParticleManager/GPUParticle/GPUParticleSystem.h>
#include <SEED/Source/Manager/VideoManager/VideoManager.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*                                                                                                               */
/*                                              static変数の初期化                                                 */
/*                                                                                                               */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SEED* SEED::instance_ = nullptr;
std::wstring SEED::windowTitle_ = L"SEED::GameWindow";
std::wstring SEED::systemWindowTitle_ = L"SEED::System";
uint32_t SEED::windowBackColor_ = MyMath::IntColor(27,27,27,255);


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*                                                                                                               */
/*                                                初期化、終了処理                                                  */
/*                                                                                                               */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*-----------------コンストラクタ、デストラクタ---------------------*/

SEED::~SEED(){
    delete instance_;
    instance_ = nullptr;
}

/*---------------------- インスタンス取得 ------------------------*/

SEED* SEED::GetInstance(){
    if(instance_ == nullptr){
        instance_ = new SEED();
    }
    return instance_;
}

/*------------------------ 更新処理 ---------------------------*/
void SEED::Update(){

}

/*------------------------ 描画処理 ---------------------------*/
void SEED::Draw(){

    // グリッドの描画
    if(instance_->isGridVisible_){
        DrawGrid();
    }
}


void SEED::SetImGuiEmptyWindows(){
#ifdef _DEBUG

    // 全体
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->WorkPos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImGui::GetMainViewport()->WorkSize, ImGuiCond_Always);
    ImGui::Begin("SEED_Empty", nullptr,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoBringToFrontOnFocus|
        ImGuiWindowFlags_NoDocking |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_HorizontalScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse
    );

    // ドックIDの取得
    ImGuiID dockId = ImGui::GetID("SEED_Empty");
    // windowSize分のドックスペースを作成
    ImGui::DockSpace(dockId, ImGui::GetMainViewport()->WorkSize);

    ImGui::End();
#endif
}


void SEED::DrawGUI(){
#ifdef _DEBUG

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
    // ボタンの背景色を設定
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.8f, 0.8f, 1.0f));
    // menuItemの背景色を設定
    ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

    if(ImGui::BeginMainMenuBar()){
        if(ImGui::BeginMenu("ファイル")){
            ImGui::EndMenu();
        }

        if(ImGui::BeginMenu("編集")){
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
    ImGui::PopStyleColor(3);


    // 親子付け用の空ウィンドウを作成
    SetImGuiEmptyWindows();

    ImFunc::CustomBegin("システム",MoveOnly_TitleBar);
    /*===== FPS表示 =====*/
    ImGui::Text("FPS: %f", ImGui::GetIO().Framerate);
    ImGui::Checkbox("グリッド表示", &instance_->isGridVisible_);
    if(ImGui::Checkbox("デバッグカメラ", &isDebugCamera_)){
        if(isDebugCamera_){
            SEED::SetMainCamera("debug");
            SEED::SetIsCameraActive("debug", true);
        } else{
            SEED::SetMainCamera("default");
            SEED::SetIsCameraActive("debug", false);
        }
    }

    if(ImGui::SliderFloat("解像度", &instance_->resolutionRate_, 0.0f, 1.0f)){
        // 解像度率を変更
        ChangeResolutionRate(instance_->resolutionRate_);
    }

    if(ImGui::ColorEdit4("ウインドウの背景色", (float*)&instance_->clearColor_)){
        // ウインドウの背景色を変更
        instance_->windowBackColor_ = MyMath::IntColor(instance_->clearColor_);
    }

    // マウス移動量
    Vector2 mouseVec = Input::GetMouseVector(INPUT_STATE::BEFORE);
    Vector2 mousePos = Input::GetMousePosition();
    if(mouseVec.Length() > 500.0f){
        mouseVec = Vector2(0.0f);
    }

    ImGui::Text("Mouse Vector: (%f, %f)", mouseVec.x, mouseVec.y);
    ImGui::Text("Mouse Position: (%.1f, %.1f)", mousePos.x, mousePos.y);

    ImGui::End();

#endif // _DEBUG
}

/*------------------------ 初期化処理 ---------------------------*/

void SEED::Initialize(int clientWidth, int clientHeight, HINSTANCE hInstance, int nCmdShow){

    GetInstance();
    instance_->kClientWidth_ = clientWidth;
    instance_->kClientHeight_ = clientHeight;

    // ウインドウの作成
    WindowManager::Initialize(hInstance, nCmdShow);
    WindowManager::Create(windowTitle_, clientWidth, clientHeight);
#ifdef USE_SUB_WINDOW
    WindowManager::Create(systemWindowTitle_, instance_->kSystemClientWidth_, instance_->kSystemClientHeight_);
#endif // USE_SUB_WINDOW

    // 各マネージャの初期化
    ClockManager::Initialize();
    CameraManager::Initialize();
    DxManager::Initialize(instance_);
    ImGuiManager::Initialize();
    TextureManager::Initialize();
    TextSystem::Initialize();
    AudioManager::Initialize();
    Input::Initialize();
    ModelManager::Initialize();
    ParticleManager::Initialize();
    SceneManager::Initialize();
    CollisionManager::Initialize();
    GPUParticleSystem::Initialize();


    // カメラの作成と追加
    instance_->defaultCamera_ = std::make_unique<BaseCamera>();
    instance_->debugCamera_ = std::make_unique<DebugCamera>();
    RegisterCamera("default",instance_->defaultCamera_.get());
    RegisterCamera("debug",instance_->debugCamera_.get());
    // デフォルトカメラを設定
    SetMainCamera("default");
    // デフォルトスカイボックスの設定
    SetSkyBox("DefaultAssets/CubeMaps/rostock_laage_airport_4k.dds");

    // 起動時読み込み
    instance_->StartUpLoad();

    // offscreenの画面のアルファ値を1にするためのcolor{0,0,0,1}のスプライトを作成
    instance_->offscreenWrapper_ = std::make_unique<Sprite>("DefaultAssets/white1x1.png");
    instance_->offscreenWrapper_->size = { (float)clientWidth,(float)clientHeight };
    instance_->offscreenWrapper_->color = MyMath::FloatColor(0, 0, 0, 256);
    instance_->offscreenWrapper_->blendMode = BlendMode::ADD;// 深度書き込みをしないため、加算合成で描画
    instance_->offscreenWrapper_->isStaticDraw = false;
    instance_->offscreenWrapper_->isApplyViewMat = false;
    instance_->offscreenWrapper_->layer = 100;
    // Vec4版の色の計算
    instance_->clearColor_ = MyMath::FloatColor(windowBackColor_);
}

/*------------------------- 終了処理 ---------------------------*/

void SEED::Finalize(){
    ImGuiManager::Finalize();
    DxManager::Finalize();
    instance_->isEndAplication_ = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*                                                                                                               */
/*                                             ゲームループ内の関数                                                 */
/*                                                                                                               */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*----------------------- フレーム開始処理 ----------------------*/

void SEED::BeginFrame(){

    // ウインドウの更新
    WindowManager::Update();

    // 時間を保存
    ClockManager::BeginFrame();

    // 全入力情報を格納
    Input::GetAllInput();

    // Audioのフレーム開始処理
    AudioManager::BeginFrame();

    // imgui,directXのフレーム開始時処理
    ImGuiManager::PreDraw();
    DxManager::GetInstance()->PreDraw();

    // imguiの描画
    instance_->DrawGUI();
}

/*----------------------- フレーム終了処理 ----------------------*/

void SEED::EndFrame(){
    // offscreenの画面のアルファ値を1にするため、color{0,0,01}のスプライトを表示
    instance_->offscreenWrapper_->Draw();
    Draw();

    // 描画
    DxManager::GetInstance()->DrawPolygonAll();
    ImGuiManager::PostDraw();
    DxManager::GetInstance()->PostDraw();

    // videoの終了時処理
    VideoManager::GetInstance()->EndFrame();

    // inputのフレーム終了処理
    Input::EndFrame();

    // 経過時間を取得
    ClockManager::EndFrame();
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*                                                                                                               */
/*                                               描画に関わる関数                                                  */
/*                                                                                                               */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*============================================== 三角形 =========================================*/

void SEED::DrawTriangle(const Triangle& triangle){
    Matrix4x4 worldMat = AffineMatrix(triangle.scale, triangle.rotate, triangle.translate);
    int GH = triangle.GH;

    if(triangle.GH == -1){
        GH = TextureManager::LoadTexture("DefaultAssets/white1x1.png");
    }

    instance_->pPolygonManager_->AddTriangle(
        TransformToVec4(triangle.localVertex[0]),
        TransformToVec4(triangle.localVertex[1]),
        TransformToVec4(triangle.localVertex[2]),
        worldMat, triangle.color, triangle.litingType, triangle.uvTransform, true,
        true,GH,triangle.blendMode, triangle.cullMode
    );
}

void SEED::AddTriangle3DPrimitive(
    const Vector4& v1, const Vector4& v2, const Vector4& v3,
    const Vector2& texCoordV1, const Vector2& texCoordV2, const Vector2& texCoordV3,
    const Color& color, uint32_t GH, BlendMode blendMode, int32_t lightingType,
    const Matrix4x4& uvTransform, D3D12_CULL_MODE cullMode
){
    instance_->pPolygonManager_->AddTrianglePrimitive(
        v1, v2, v3,
        texCoordV1, texCoordV2, texCoordV3,
        color, GH, blendMode, lightingType, uvTransform, cullMode
    );
}


void SEED::DrawTriangle2D(const Triangle2D& triangle){

    instance_->pPolygonManager_->AddTriangle(
        TransformToVec4(triangle.localVertex[0]),
        TransformToVec4(triangle.localVertex[1]),
        TransformToVec4(triangle.localVertex[2]),
        triangle.GetWorldMatrix(), triangle.color, LIGHTINGTYPE_NONE, 
        triangle.uvTransform, false, triangle.isApplyViewMat,
        triangle.GH != -1 ? triangle.GH : TextureManager::LoadTexture("DefaultAssets/white1x1.png"),
        triangle.blendMode, D3D12_CULL_MODE::D3D12_CULL_MODE_BACK,
        triangle.isStaticDraw, triangle.drawLocation, triangle.layer
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
        quad.texCoord[0], quad.texCoord[1], quad.texCoord[2], quad.texCoord[3],
        worldMat, quad.color, quad.lightingType, quad.uvTransform, true,true,
        quad.GH != -1 ? quad.GH : TextureManager::LoadTexture("DefaultAssets/white1x1.png"),
        quad.blendMode, quad.isText, quad.cullMode
    );
}

void SEED::AddQuad3DPrimitive(
    const Vector4& v1, const Vector4& v2, const Vector4& v3, const Vector4& v4,
    const Vector2& texCoordV1, const Vector2& texCoordV2, const Vector2& texCoordV3, const Vector2& texCoordV4,
    const Color& color, uint32_t GH, BlendMode blendMode, int32_t lightingType,
    const Matrix4x4& uvTransform, D3D12_CULL_MODE cullMode
){
    instance_->pPolygonManager_->AddQuadPrimitive(
        v1, v2, v3, v4,
        texCoordV1, texCoordV2, texCoordV3, texCoordV4,
        color, GH, blendMode, lightingType, uvTransform, cullMode
    );
}

void SEED::DrawQuad2D(const Quad2D& quad,const Color& masterColor){

    Matrix4x4 worldMat = AffineMatrix(quad.scale.ToVec3(), { 0.0f,0.0f,quad.rotate }, quad.translate.ToVec3());
    instance_->pPolygonManager_->AddQuad(
        quad.localVertex[0].ToVec3(),
        quad.localVertex[1].ToVec3(),
        quad.localVertex[2].ToVec3(),
        quad.localVertex[3].ToVec3(),
        quad.texCoord[0], quad.texCoord[1], quad.texCoord[2], quad.texCoord[3],
        worldMat, masterColor * quad.color, quad.lightingType,
        quad.uvTransform, false, quad.isApplyViewMat,
        quad.GH != -1 ? quad.GH : TextureManager::LoadTexture("DefaultAssets/white1x1.png"),
        quad.blendMode, quad.isText, D3D12_CULL_MODE::D3D12_CULL_MODE_BACK,
        quad.isStaticDraw, quad.drawLocation, quad.layer
    );
}

/*========================================== スプライト ===========================================*/


void SEED::DrawSprite(const Sprite& sprite, const std::optional<Color>& masterColor){

    instance_->pPolygonManager_->AddSprite(
        sprite,
        masterColor,
        false
    );
}


/*========================================== モデル ===========================================*/

void SEED::DrawModel(Model* model, const std::optional<Color>& masterColor){
    instance_->pPolygonManager_->AddModel(model,masterColor);
}


/*========================================== 線 ===========================================*/

void SEED::DrawLine(const Vector3& v1, const Vector3& v2, const Color& color, BlendMode blendMode){
    instance_->pPolygonManager_->AddLine(
        TransformToVec4(v1),
        TransformToVec4(v2),
        IdentityMat4(), color, true,true, blendMode, false
    );
}

void SEED::DrawLine2D(
    const Vector2& v1, const Vector2& v2, const Color& color, BlendMode blendMode, bool isApplyViewMat
){
    instance_->pPolygonManager_->AddLine(
        TransformToVec4(v1),
        TransformToVec4(v2),
        IdentityMat4(), color, false, isApplyViewMat,blendMode, false, DrawLocation::Front, 100
    );
}

/*========================================== テキスト ===========================================*/

// 内部で文字ごとのDrawQuadに変換する
void SEED::DrawText2D(const TextBox2D& textBox){
    textBox.Draw();
}

void SEED::DrawText3D(const TextBox3D& textBox){
    textBox.Draw();
}


/*========================================== リング ===========================================*/

void SEED::DrawRing(const Ring& ring){
    ring;
}


///////////////////////////////////////////////////////
// AABB,OBBの描画関数
///////////////////////////////////////////////////////
void SEED::DrawAABB(const AABB& aabb, const Color& color){
    OBB obb;
    obb.center = aabb.center;
    obb.halfSize = aabb.halfSize;
    DrawOBB(obb, color);
}

void SEED::DrawAABB2D(const AABB2D& aabb, const Color& color){
    Vector2 vertex[4] = {
        Vector2(aabb.center.x - aabb.halfSize.x, aabb.center.y - aabb.halfSize.y),
        Vector2(aabb.center.x + aabb.halfSize.x, aabb.center.y - aabb.halfSize.y),
        Vector2(aabb.center.x + aabb.halfSize.x, aabb.center.y + aabb.halfSize.y),
        Vector2(aabb.center.x - aabb.halfSize.x, aabb.center.y + aabb.halfSize.y)
    };
    DrawLine2D(vertex[0], vertex[1], color);
    DrawLine2D(vertex[1], vertex[2], color);
    DrawLine2D(vertex[2], vertex[3], color);
    DrawLine2D(vertex[3], vertex[0], color);
}

void SEED::DrawOBB(const OBB& obb, const Color& color){

    // 頂点を取得
    static std::array<Vector3, 8> vertex;
    vertex = obb.GetVertices();

    // 手前面
    DrawLine(vertex[0], vertex[1], color);
    DrawLine(vertex[1], vertex[3], color);
    DrawLine(vertex[3], vertex[2], color);
    DrawLine(vertex[2], vertex[0], color);
    // 奥面
    DrawLine(vertex[4], vertex[5], color);
    DrawLine(vertex[5], vertex[7], color);
    DrawLine(vertex[7], vertex[6], color);
    DrawLine(vertex[6], vertex[4], color);
    // 接続線
    DrawLine(vertex[0], vertex[4], color);
    DrawLine(vertex[1], vertex[5], color);
    DrawLine(vertex[2], vertex[6], color);
    DrawLine(vertex[3], vertex[7], color);
}

/////////////////////////////////////////////////////////////
// 2次元OBBの描画
/////////////////////////////////////////////////////////////
void SEED::DrawOBB2D(const OBB2D& obb, const Color& color){
    // 頂点を計算
    static std::array<Vector2, 4> vertex;
    vertex = obb.GetVertices();

    // 描画
    DrawLine2D(vertex[0], vertex[1], color);
    DrawLine2D(vertex[1], vertex[3], color);
    DrawLine2D(vertex[3], vertex[2], color);
    DrawLine2D(vertex[2], vertex[0], color);
}

/////////////////////////////////////////////////////////////
// 六角形の描画
/////////////////////////////////////////////////////////////
void SEED::DrawHexagon(
    const Vector2& center, float radius, float theta,
    const Color& color, BlendMode blendMode,
    DrawLocation drawLocation, int32_t layer, bool isApplyViewMat
){
    Triangle2D tri;
    tri.GH = TextureManager::LoadTexture("DefaultAssets/white1x1.png");
    tri.color = color;
    tri.drawLocation = drawLocation;
    tri.layer = layer;
    tri.blendMode = blendMode;
    tri.rotate = 0.0f;
    tri.isStaticDraw = false;
    tri.isApplyViewMat = isApplyViewMat;

    static std::array<Vector2,6> vertices;
    for(int i = 0; i < 6; ++i){
        float angle = i * ((float)std::numbers::pi / 3.0f) + theta; // 60度ごと
        Vector2 point = { radius * std::cos(angle), radius * std::sin(angle) };
        vertices[i] = center + point;
    }

    // 描画
    for(int i = 0; i < 6; ++i){
        tri.localVertex[0] = center;
        tri.localVertex[1] = vertices[i];
        tri.localVertex[2] = vertices[(i + 1) % 6]; // 次の頂点（最後は最初に戻る）
        DrawTriangle2D(tri);
    }
}

void SEED::DrawHexagonFrame(
    const Vector2& center, float radius, float theta, float frameWidthRate, 
    const Color& color, BlendMode blendMode,
    DrawLocation drawLocation, int32_t layer, bool isApplyViewMat
){
    Quad2D quad;
    quad.GH = TextureManager::LoadTexture("DefaultAssets/white1x1.png");
    quad.color = color;
    quad.drawLocation = drawLocation;
    quad.layer = layer;
    quad.blendMode = blendMode;
    quad.isApplyViewMat = isApplyViewMat;

    static std::array<Vector2,6> vertices[2];
    for(int i = 0; i < 6; ++i){
        float angle = i * ((float)std::numbers::pi / 3.0f) + theta; // 60度ごと
        Vector2 outSidePos = { radius * std::cos(angle), radius * std::sin(angle) };
        Vector2 inSidePos = { (radius * (1.0f - frameWidthRate)) * std::cos(angle), (radius * (1.0f - frameWidthRate)) * std::sin(angle) };
        vertices[0][i] = center + outSidePos;
        vertices[1][i] = center + inSidePos;
    }

    // 描画
    for(int i = 0; i < 6; ++i){
        quad.localVertex[0] = vertices[0][i];
        quad.localVertex[1] = vertices[0][(i + 1) % 6];
        quad.localVertex[2] = vertices[1][i];
        quad.localVertex[3] = vertices[1][(i + 1) % 6];
        DrawQuad2D(quad);
    }
}



/////////////////////////////////////////////////////////////
// 球の描画
/////////////////////////////////////////////////////////////
void SEED::DrawSphere(const Vector3& center, const Vector3& radius, int32_t subdivision, const Color& color){

    // 常識的な範囲に納める
    subdivision = std::clamp(subdivision, 3, 16);

    // 球の頂点を計算
    std::vector<Vector3> vertex((subdivision + 1) * subdivision); // 余分な頂点を確保
    float pi = static_cast<float>(std::numbers::pi);

    for(int i = 0; i <= subdivision; i++){ // 緯度方向: 0～subdivision
        float theta = pi * i / subdivision; // 緯度角
        for(int j = 0; j < subdivision; j++){ // 経度方向: 0～subdivision-1
            float phi = 2.0f * pi * j / subdivision; // 経度角

            // 頂点の計算
            vertex[i * subdivision + j] = center + Vector3(
                radius.x * std::sin(theta) * std::cos(phi),
                radius.y * std::cos(theta),
                radius.z * std::sin(theta) * std::sin(phi)
            );
        }
    }

    // 線の描画(ラインで)
    for(int i = 0; i < subdivision; i++){ // 緯度方向
        for(int j = 0; j < subdivision; j++){ // 経度方向
            int current = i * subdivision + j;
            int next = current + 1; // 経度方向の次の頂点
            int nextRow = (i + 1) * subdivision + j; // 緯度方向の次の頂点

            // 経度方向の線
            DrawLine(vertex[current], vertex[current % subdivision == subdivision - 1 ? i * subdivision : next], color);

            // 緯度方向の線
            DrawLine(vertex[current], vertex[nextRow], color);
        }
    }
}


void SEED::DrawSphere(const Vector3& center, float radius, int32_t subdivision, const Color& color){
    DrawSphere(center, Vector3(radius, radius, radius), subdivision, color);
}


/////////////////////////////////////////////////////////////
// 円柱の描画
/////////////////////////////////////////////////////////////
void SEED::DrawCylinder(
    const Vector3& start, const Vector3& end, 
    float startRadius, float endRadius, 
    int32_t subdivision, const Color& color
){

    if(subdivision < 3){
        subdivision = 3; // 最低3分割は必要
    }

    // 円柱の軸ベクトルと高さ
    Vector3 axis = MyMath::Normalize(end - start);

    // 基準となる法線ベクトル（円生成の基準）
    Vector3 up = (fabs(axis.y) < 0.99f) ? Vector3(0, 1, 0) : Vector3(1, 0, 0);
    Vector3 tangent = MyMath::Normalize(MyMath::Cross(up, axis));      // 円周の方向
    Vector3 bitangent = MyMath::Cross(axis, tangent);          // 円の垂直方向

    // 円周上の頂点計算
    std::vector<Vector3> bottomCircle(subdivision);
    std::vector<Vector3> topCircle(subdivision);
    float angleStep = 2.0f * 3.14159265359f / subdivision;

    for(int i = 0; i < subdivision; ++i){
        float angle = i * angleStep;
        float x = cos(angle) * startRadius;
        float z = sin(angle) * startRadius;

        // 円周上の点を計算
        bottomCircle[i] = start + tangent * x + bitangent * z;
    }

    for(int i = 0; i < subdivision; ++i){
        float angle = i * angleStep;
        float x = cos(angle) * endRadius;
        float z = sin(angle) * endRadius;

        // 円周上の点を計算
        topCircle[i] = end + tangent * x + bitangent * z;
    }

    // 線分を描画
    for(int i = 0; i < subdivision; ++i){
        int next = (i + 1) % subdivision;

        // 底面の線分
        DrawLine(bottomCircle[i], bottomCircle[next], color);

        // 上面の線分
        DrawLine(topCircle[i], topCircle[next], color);

        // 底面と上面をつなぐ線分
        DrawLine(bottomCircle[i], topCircle[i], color);
    }
}


/////////////////////////////////////////////////////////////
// カプセルの描画
/////////////////////////////////////////////////////////////
void SEED::DrawCapsule(const Vector3& start, const Vector3& end, float radius, int32_t subdivision, const Color& color){
    // 視点と終点に球を描画
    DrawSphere(start, radius, subdivision, color);
    DrawSphere(end, radius, subdivision, color);
    // 円柱を描画
    DrawCylinder(start, end, radius, radius, subdivision, color);
}

void SEED::DrawCapsule(const Vector3& start, const Vector3& end, const Vector3& radius, int32_t subdivision, const Color& color){
    // 視点と終点に球を描画
    DrawSphere(start, radius, subdivision, color);
    DrawSphere(end, radius, subdivision, color);
    // 円柱を描画
    DrawCylinder(start, end, MyMath::Length(radius), MyMath::Length(radius), subdivision, color);
}


/////////////////////////////////////////////////////////////
// グリッドの描画
/////////////////////////////////////////////////////////////
void SEED::DrawGrid(float gridInterval, int32_t gridCount){

    float width = gridInterval * gridCount;
    Vector3 leftFront = { -width * 0.5f,0.0f,-width * 0.5f };

    for(int xIdx = 0; xIdx < gridCount + 1; xIdx++){
        SEED::DrawLine(
            leftFront + Vector3(gridInterval * xIdx, 0.0f, 0.0f),
            leftFront + Vector3(gridInterval * xIdx, 0.0f, width),
            { 1.0f,1.0f,1.0f,1.0f }
        );
    }

    for(int yIdx = 0; yIdx < gridCount + 1; yIdx++){
        SEED::DrawLine(
            leftFront + Vector3(0.0f, 0.0f, gridInterval * yIdx),
            leftFront + Vector3(width, 0.0f, gridInterval * yIdx),
            { 1.0f,1.0f,1.0f,1.0f }
        );
    }
}

void SEED::DrawBezier(const Vector3& p1, const Vector3& p2, const Vector3& p3, uint32_t subdivision, const Color& color){
    // ベジェ曲線の描画
    Vector3 previous = p1;
    for(uint32_t i = 1; i <= subdivision; i++){

        // 現在の位置を求める
        float t = float(i) / subdivision;
        Vector3 current = MyMath::Bezier(p1, p2, p3, t);

        // 線を描画
        DrawLine(previous, current, color);

        // 現在の点を保存
        previous = current;
    }
}

// 2D版
void SEED::DrawBezier(const Vector2& p1, const Vector2& p2, const Vector2& p3, uint32_t subdivision, const Color& color){
    // ベジェ曲線の描画
    Vector2 previous = p1;
    for(uint32_t i = 1; i <= subdivision; i++){
        // 現在の位置を求める
        float t = float(i) / subdivision;
        Vector2 current = MyMath::Bezier(p1, p2, p3, t);
        // 線を描画
        DrawLine2D(previous, current, color);
        // 現在の点を保存
        previous = current;
    }
}

// 複数点版
void SEED::DrawBezier(const std::vector<Vector2>& points, uint32_t subdivision, const Color& color){

    // 点が2つ未満の場合は描画しない
    if(points.size() < 2){ return; }

    // 必要な変数を用意
    float t = 0;
    uint32_t totalSubdivision = uint32_t(points.size() - 1) * subdivision;
    std::optional<Vector2> previous = std::nullopt;

    // ベジェ曲線の描画
    for(uint32_t i = 0; i <= totalSubdivision; i++){
        // 現在の位置を求める
        t = float(i) / totalSubdivision;
        // 現在の区間の点を求める
        Vector2 p = MyMath::Bezier(points, t);
        // 線を描画
        if(previous != std::nullopt){
            DrawLine2D(previous.value(), p, color);
        }
        // 現在の点を保存
        previous = p;
    }
}


/////////////////////////////////////////////////////////////
// スプライン曲線の描画
/////////////////////////////////////////////////////////////
void SEED::DrawSpline(const std::vector<Vector3>& points, uint32_t subdivision, const Color& color, bool isControlPointVisible){

    // 点が2つ未満の場合は描画しない
    if(points.size() < 2){ return; }

    // 必要な変数を用意
    float t = 0;
    uint32_t totalSubdivision = uint32_t(points.size() - 1) * subdivision;
    std::optional<Vector3> previous = std::nullopt;

    // スプライン曲線の描画
    for(uint32_t i = 0; i <= totalSubdivision; i++){

        // 現在の位置を求める
        t = float(i) / totalSubdivision;

        // 現在の区間の点を求める
        Vector3 p = MyMath::MultiCatmullRom(points, t);

        // 線を描画
        if(previous != std::nullopt){
            DrawLine(previous.value(), p, color);
        }

        // 現在の点を保存
        previous = p;
    }

    // 制御点の描画
    if(!isControlPointVisible){ return; }
    Model controlPointModel = Model("DefaultAssets/cube/cube.obj");
    controlPointModel.transform_.scale = { 0.5f,0.5f,0.5f };
    controlPointModel.masterColor_ = { 1.0f,0.0f,0.0f,1.0f };

    for(int i = 0; i < points.size(); i++){
        controlPointModel.transform_.translate = points[i];
        controlPointModel.UpdateMatrix();
        DrawModel(&controlPointModel);
    }
}

// スプライン曲線の描画（2D版）
void SEED::DrawSpline(const std::vector<Vector2>& points, uint32_t subdivision, const Color& color, bool isControlPointVisible){

    // 点が2つ未満の場合は描画しない
    if(points.size() < 2){ return; }

    // 必要な変数を用意
    float t = 0;
    uint32_t totalSubdivision = uint32_t(points.size() - 1) * subdivision;
    std::optional<Vector2> previous = std::nullopt;

    // スプライン曲線の描画
    for(uint32_t i = 0; i <= totalSubdivision; i++){

        // 現在の位置を求める
        t = float(i) / totalSubdivision;

        // 現在の区間の点を求める
        Vector2 p = MyMath::MultiCatmullRom(points, t);

        // 線を描画
        if(previous != std::nullopt){
            DrawLine2D(previous.value(), p, color);
        }

        // 現在の点を保存
        previous = p;
    }

    // 制御点の描画
    if(!isControlPointVisible){ return; }
    Quad2D controlPointQuad = MakeEqualQuad2D(5.0f, { 1.0f,0.0f,1.0f,1.0f });

    for(int i = 0; i < points.size(); i++){
        controlPointQuad.translate = points[i];
        DrawQuad2D(controlPointQuad);
    }
}


/////////////////////////////////////////////////////////////
// ライトのデバッグ用描画
/////////////////////////////////////////////////////////////
void SEED::DrawLight(const BaseLight* light){

    switch(light->GetLightType()){

    case LIGHT_TYPE::DIRECTIONAL_LIGHT:
    {
        DirectionalLight* directionalLight = (DirectionalLight*)light;

        // 必要な情報を用意
        Vector2 screenPos = { 50.0f,50.0f };
        Vector3 screenPos3D = GetMainCamera()->ToWorldPosition(screenPos, 1.0f);
        Vector3 direction = MyMath::Normalize(directionalLight->direction_);
        float dot = MyMath::Dot(direction, -GetMainCamera()->GetNormal());

        // スクリーン上で最長20.0fの線を描画
        Matrix4x4 vpVp = GetMainCamera()->GetVpVp();
        Vector3 transformedPos[2] = {
            screenPos3D * vpVp,
            (screenPos3D + direction) * vpVp
        };

        Vector2 screenDirection =
            MyMath::Normalize(
                Vector2(transformedPos[1].x, transformedPos[1].y) - Vector2(transformedPos[0].x, transformedPos[0].y)
            );

        // ライトの方向を描画
        Vector2 end = screenPos + (screenDirection * 40.0f) * (1.0f - fabsf(dot));
        DrawLine2D(screenPos, end, light->color_);

        break;
    }
    case LIGHT_TYPE::POINT_LIGHT:
    {
        PointLight* pointLight = (PointLight*)light;
        DrawSphere(pointLight->position, 1.0f, 6, light->color_);
        break;
    }
    case LIGHT_TYPE::SPOT_LIGHT:
    {
        SpotLight* spotLight = (SpotLight*)light;
        DrawSphere(spotLight->position, 1.0f, 6, light->color_);
        Vector3 lightVec = spotLight->direction * spotLight->distance;
        DrawLine(spotLight->position, spotLight->position + lightVec, light->color_);
        break;
    }
    default:
        break;
    }
}



/////////////////////////////////////////////////////////////
// cubeMapの描画
/////////////////////////////////////////////////////////////

void SEED::SetSkyBox(const std::string& textureName, const Color& color){
    SkyBox::textureGH_ = TextureManager::LoadTexture(textureName);
    SkyBox::textureName_ = textureName;
    SkyBox::textureGH_;
    SkyBox::color_ = color;
}

void SEED::DrawSkyBox(bool isFollowCameraPos, const Vector3& position, float scale){
    SkyBox::scale_ = scale;
    SkyBox::translate_ = position;
    SkyBox::isFollowToCamera_ = isFollowCameraPos;
    instance_->pPolygonManager_->AddSkyBoxDrawCommand();
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*                                                                                                               */
/*                                                その他細かい関数                                                  */
/*                                                                                                               */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*----------------------- 起動時読み込み -----------------------*/

void SEED::StartUpLoad(){

    AudioManager::StartUpLoad();
    ModelManager::StartUpLoad();
    TextureManager::StartUpLoad();
}

/*------------------ 画像の縦横幅を取得する関数 ------------------*/

Vector2 SEED::GetImageSize(const std::wstring& fileName){

    // パスの結合
    std::wstring filePath;
    
    if(fileName.starts_with(L"Resources")){
        filePath = fileName;
    } else{
       filePath = L"resources/textures/" + fileName;
    }

    // 画像をロード
    Gdiplus::Image image(filePath.c_str());

    // 画像の幅と高さを取得
    int width = image.GetWidth();
    int height = image.GetHeight();

    return { float(width),float(height) };
}

/*------------------ 画面の解像度を変更する関数 ------------------*/

void SEED::ChangeResolutionRate(float resolutionRate){
    static float preResolutionRate = 1.0f;
    if(resolutionRate != preResolutionRate){
        DxManager::GetInstance()->ChangeResolutionRate(resolutionRate);
        preResolutionRate = resolutionRate;
    }
}

/*------------------ カメラにシェイクを設定する関数 ------------------*/
void SEED::SetCameraShake(float time, float power, const Vector3& shakeLevel){
    GetMainCamera()->SetShake(time, power, shakeLevel);
}