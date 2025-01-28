// local
#include <SEED.h>
#include <DxFunc.h>
#include <MatrixFunc.h>
#include <MyMath.h>
#include <ShapeMath.h>
#include <includes.h>
#include <Environment.h>
#include <SceneManager/SceneManager.h>
#include <CollisionManaer/CollisionManager.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*                                                                                                               */
/*                                              static変数の初期化                                                 */
/*                                                                                                               */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SEED* SEED::instance_ = nullptr;
std::wstring SEED::windowTitle_ = L"SEED";
std::wstring SEED::systemWindowTitle_ = L"SEED::System";
uint32_t SEED::windowBackColor_ = 0x070707ff;//0x47ada3ff;


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
void SEED::Update() {

}

/*------------------------ 初期化処理 ---------------------------*/
void SEED::Draw() {

}

/*------------------------ 初期化処理 ---------------------------*/

void SEED::Initialize(int clientWidth, int clientHeight, HINSTANCE hInstance, int nCmdShow){

    GetInstance();
    instance_->kClientWidth_ = clientWidth;
    instance_->kClientHeight_ = clientHeight;

    // ウインドウの作成
    WindowManager::Initialize(hInstance, nCmdShow);
    WindowManager::Create(windowTitle_, clientWidth, clientHeight);
#ifdef _DEBUG
    WindowManager::Create(systemWindowTitle_, clientWidth, clientHeight);
#endif // _DEBUG

    // 各マネージャの初期化
    ClockManager::Initialize();
    CameraManager::Initialize();
    DxManager::Initialize(instance_);
    ImGuiManager::Initialize();
    TextureManager::Initialize();
    AudioManager::Initialize();
    Input::Initialize();
    ModelManager::Initialize();
    ParticleManager::Initialize();
    SceneManager::Initialize();
    CollisionManager::Initialize();

    // 起動時読み込み
    instance_->StartUpLoad();
}

/*------------------------- 終了処理 ---------------------------*/

void SEED::Finalize(){
    ImGuiManager::Finalize();
    DxManager::Finalize();
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

    // imgui,directXのフレーム開始時処理
    ImGuiManager::PreDraw();
    DxManager::GetInstance()->PreDraw();
}

/*----------------------- フレーム終了処理 ----------------------*/

void SEED::EndFrame(){
    // 描画
    DxManager::GetInstance()->DrawPolygonAll();
    ImGuiManager::PostDraw();
    DxManager::GetInstance()->PostDraw();
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
    instance_->pPolygonManager_->AddTriangle(
        TransformToVec4(triangle.localVertex[0]),
        TransformToVec4(triangle.localVertex[1]),
        TransformToVec4(triangle.localVertex[2]),
        worldMat, triangle.color, triangle.litingType, triangle.uvTransform, true,
        triangle.GH, triangle.blendMode, triangle.cullMode
    );
}


void SEED::DrawTriangle2D(const Triangle2D& triangle){

    instance_->pPolygonManager_->AddTriangle(
        TransformToVec4(triangle.localVertex[0]),
        TransformToVec4(triangle.localVertex[1]),
        TransformToVec4(triangle.localVertex[2]),
        triangle.GetWorldMatrix(), triangle.color, LIGHTINGTYPE_NONE, triangle.uvTransform, false, triangle.GH,
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
        worldMat, quad.color, quad.lightingType, quad.uvTransform, true, quad.GH, quad.blendMode,
        quad.cullMode
    );
}

void SEED::DrawQuad2D(const Quad2D& quad){

    Matrix4x4 worldMat = AffineMatrix(quad.scale.ToVec3(), { 0.0f,0.0f,quad.rotate }, quad.translate.ToVec3());
    instance_->pPolygonManager_->AddQuad(
        quad.localVertex[0].ToVec3(),
        quad.localVertex[1].ToVec3(),
        quad.localVertex[2].ToVec3(),
        quad.localVertex[3].ToVec3(),
        worldMat, quad.color, quad.lightingType, quad.uvTransform, false, quad.GH,
        quad.blendMode, D3D12_CULL_MODE::D3D12_CULL_MODE_BACK,
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
        sprite.flipX,
        sprite.flipY,
        sprite.anchorPoint,
        sprite.clipLT,
        sprite.clipSize,
        sprite.blendMode,
        D3D12_CULL_MODE::D3D12_CULL_MODE_BACK,
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
        IdentityMat4(), color, true, blendMode, false
    );
}

void SEED::DrawLine2D(const Vector2& v1, const Vector2& v2, const Vector4& color, BlendMode blendMode){
    instance_->pPolygonManager_->AddLine(
        TransformToVec4(v1),
        TransformToVec4(v2),
        IdentityMat4(), color, false, blendMode, false, DrawLocation::Front, 0
    );
}


/*========================================== リング ===========================================*/

void SEED::DrawRing(const Ring& ring){
    instance_->pPolygonManager_->AddRing(ring);
}


///////////////////////////////////////////////////////
// AABB,OBBの描画関数
///////////////////////////////////////////////////////
void SEED::DrawAABB(const AABB& aabb, const Vector4& color){
    OBB obb;
    obb.center = aabb.center;
    obb.halfSize = aabb.halfSize;
    DrawOBB(obb, color);
}

void SEED::DrawOBB(const OBB& obb, const Vector4& color){

    //ローカルな頂点を計算
    Vector3 vertex[8] = {
        Vector3(-obb.halfSize.x,-obb.halfSize.y,-obb.halfSize.z),
        Vector3(obb.halfSize.x,-obb.halfSize.y,-obb.halfSize.z),
        Vector3(obb.halfSize.x,obb.halfSize.y,-obb.halfSize.z),
        Vector3(-obb.halfSize.x,obb.halfSize.y,-obb.halfSize.z),
        Vector3(-obb.halfSize.x,-obb.halfSize.y,obb.halfSize.z),
        Vector3(obb.halfSize.x,-obb.halfSize.y,obb.halfSize.z),
        Vector3(obb.halfSize.x,obb.halfSize.y,obb.halfSize.z),
        Vector3(-obb.halfSize.x,obb.halfSize.y,obb.halfSize.z)
    };

    // 回転がある場合は回転を適用
    if(MyMath::Length(obb.rotate)){
        for(int i = 0; i < 8; i++){
            vertex[i] *= RotateMatrix(obb.rotate);
        }
    }

    // 移動を適用
    for(int i = 0; i < 8; i++){
        vertex[i] += obb.center;
    }

    // 描画
    DrawLine(vertex[0], vertex[1], color);
    DrawLine(vertex[1], vertex[2], color);
    DrawLine(vertex[2], vertex[3], color);
    DrawLine(vertex[3], vertex[0], color);

    DrawLine(vertex[4], vertex[5], color);
    DrawLine(vertex[5], vertex[6], color);
    DrawLine(vertex[6], vertex[7], color);
    DrawLine(vertex[7], vertex[4], color);

    DrawLine(vertex[0], vertex[4], color);
    DrawLine(vertex[1], vertex[5], color);
    DrawLine(vertex[2], vertex[6], color);
    DrawLine(vertex[3], vertex[7], color);
}



/////////////////////////////////////////////////////////////
// 球の描画
/////////////////////////////////////////////////////////////
void SEED::DrawSphere(const Vector3& center, const Vector3& radius, int32_t subdivision, const Vector4& color){

    // 常識的な範囲に納める
    subdivision = std::clamp(subdivision, 3, 16);

    // 球の頂点を計算
    std::vector<Vector3> vertex((subdivision + 1) * subdivision); // 余分な頂点を確保
    float pi = static_cast<float>(std::numbers::pi);

    for(int i = 0; i <= subdivision; i++) { // 緯度方向: 0～subdivision
        float theta = pi * i / subdivision; // 緯度角
        for(int j = 0; j < subdivision; j++) { // 経度方向: 0～subdivision-1
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
    for(int i = 0; i < subdivision; i++) { // 緯度方向
        for(int j = 0; j < subdivision; j++) { // 経度方向
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


void SEED::DrawSphere(const Vector3& center, float radius, int32_t subdivision, const Vector4& color){
    DrawSphere(center, Vector3(radius,radius,radius), subdivision, color);
}


/////////////////////////////////////////////////////////////
// 円柱の描画
/////////////////////////////////////////////////////////////
void SEED::DrawCylinder(const Vector3& start, const Vector3& end, float radius, int32_t subdivision, const Vector4& color){

    if(subdivision < 3) {
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

    for(int i = 0; i < subdivision; ++i) {
        float angle = i * angleStep;
        float x = cos(angle) * radius;
        float z = sin(angle) * radius;

        // 円周上の点を計算
        bottomCircle[i] = start + tangent * x + bitangent * z;
        topCircle[i] = end + tangent * x + bitangent * z;
    }

    // 線分を描画
    for(int i = 0; i < subdivision; ++i) {
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
void SEED::DrawCapsule(const Vector3& start, const Vector3& end, float radius, int32_t subdivision, const Vector4& color){
    // 視点と終点に球を描画
    DrawSphere(start, radius, subdivision, color);
    DrawSphere(end, radius, subdivision, color);
    // 円柱を描画
    DrawCylinder(start, end, radius, subdivision, color);
}

void SEED::DrawCapsule(const Vector3& start, const Vector3& end, const Vector3& radius, int32_t subdivision, const Vector4& color){
    // 視点と終点に球を描画
    DrawSphere(start, radius, subdivision, color);
    DrawSphere(end, radius, subdivision, color);
    // 円柱を描画
    DrawCylinder(start, end,MyMath::Length(radius), subdivision, color);
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
            { 0.2f,0.2f,0.2f,1.0f }
        );
    }

    for(int yIdx = 0; yIdx < gridCount + 1; yIdx++){
        SEED::DrawLine(
            leftFront + Vector3(0.0f, 0.0f, gridInterval * yIdx),
            leftFront + Vector3(width, 0.0f, gridInterval * yIdx),
            { 0.2f,0.2f,0.2f,1.0f }
        );
    }
}


/////////////////////////////////////////////////////////////
// スプライン曲線の描画
/////////////////////////////////////////////////////////////
void SEED::DrawSpline(const std::vector<Vector3>& points,uint32_t subdivision, const Vector4& color,bool isControlPointVisible){

    // 点が2つ未満の場合は描画しない
    if(points.size() < 2){return;}

    // 必要な変数を用意
    float t = 0;
    uint32_t totalSubdivision = uint32_t(points.size() - 1) * subdivision;
    std::optional<Vector3> previous = std::nullopt;

    // スプライン曲線の描画
    for(uint32_t i = 0; i <= totalSubdivision; i++){

        // 現在の位置を求める
        t = float(i) / totalSubdivision;

        // 現在の区間の点を求める
        Vector3 p = MyMath::CatmullRomPosition(points, t);

        // 線を描画
        if(previous != std::nullopt){
            DrawLine(previous.value(), p, color);
        }

        // 現在の点を保存
        previous = p;
    }

    // 制御点の描画
    if(!isControlPointVisible){ return; }
    Model controlPointModel = Model("Assets/cube.obj");
    controlPointModel.scale_ = { 0.5f,0.5f,0.5f };
    controlPointModel.color_ = { 1.0f,0.0f,0.0f,1.0f };

    for(int i = 0; i < points.size(); i++){
        controlPointModel.translate_ = points[i];
        controlPointModel.UpdateMatrix();
        DrawModel(&controlPointModel);
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
        Vector3 screenPos3D = GetCamera()->ToWorldPosition(screenPos, 1.0f);
        Vector3 direction = MyMath::Normalize(directionalLight->direction_);
        float dot = MyMath::Dot(direction, -GetCamera()->GetNormal());

        // スクリーン上で最長20.0fの線を描画
        Matrix4x4 vpVp = GetCamera()->GetVpVp();
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
        DrawLine(spotLight->position, spotLight->position + spotLight->direction * spotLight->distance, light->color_);
        break;
    }
    default:
        break;
    }
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
    std::wstring filePath = L"resources/textures/" + fileName;

    // 画像をロード
    Gdiplus::Image image(filePath.c_str());

    // 画像の幅と高さを取得
    int width = image.GetWidth();
    int height = image.GetHeight();

    return { float(width),float(height) };
}

/*------------------ 画面の解像度を変更する関数 ------------------*/

void SEED::ChangeResolutionRate(float resolutionRate){
    DxManager::GetInstance()->ChangeResolutionRate(resolutionRate);
}

/*------------------ カメラにシェイクを設定する関数 ------------------*/
void SEED::SetCameraShake(float time, float power, const Vector3& shakeLevel){
    GetCamera()->SetShake(time, power, shakeLevel);
}
