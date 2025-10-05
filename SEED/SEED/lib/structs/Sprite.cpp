#include <SEED/Lib/Structs/Sprite.h>
#include <SEED/Source/SEED.h>
#include <SEED/Lib/Functions/MyFunc/DxFunc.h>
#include <SEED/Source/Manager/ImGuiManager/ImGuiManager.h>

Sprite::Sprite(){
    size = { 100.0f,100.0f };
    transform = Transform2D();
    anchorPoint = { 0.0f,0.0f };
    clipLT = { 0.0f,0.0f };
    clipSize = { 0.0f,0.0f };
    color = { 1.0f,1.0f,1.0f,1.0f };
    GH = TextureManager::LoadTexture("DefaultAssets/white.png");
    blendMode = BlendMode::NORMAL;
    uvTransform = Transform2D();
    isStaticDraw = false;
}

Sprite::Sprite(const std::string& filename) : Sprite::Sprite(){
    SetTexture(filename);
    size = SEED::GetImageSize(MyFunc::ConvertString(filename));
}

Sprite::Sprite(const std::string& filename, const Vector2& size)
    : Sprite::Sprite(filename){
    this->size = size;
}

void Sprite::Draw(){
    SEED::DrawSprite(*this);
}

Matrix4x4 Sprite::GetWorldMatrix()const{

    Vector2 anchorOffset;
    anchorOffset = {
        size.x * anchorPoint.x,
        size.y * anchorPoint.y
    };


    Matrix4x4 worldMat;
    if(parentMat){
        worldMat = (transform.ToMatrix() * (*parentMat)).ToMat4x4();
        worldMat *= TranslateMatrix({ offset.x,offset.y,0.0f });
        return worldMat;
    } else{
        worldMat =
            AffineMatrix(
                { transform.scale.x,transform.scale.y,1.0f },
                { 0.0f, 0.0f, transform.rotate },
                { transform.translate.x + offset.x,transform.translate.y + offset.y,0.0f }
            );
    }
    return worldMat;
}

// テクスチャを設定する
void Sprite::SetTexture(const std::string& filename){
    texturePath = filename;
    GH = TextureManager::LoadTexture(filename);
    defaultSize_ = SEED::GetImageSize(MyFunc::ConvertString(filename));
}

// 画像サイズそのままにする
void Sprite::ToDefaultSize(){
    if(!texturePath.empty()){
        size = SEED::GetImageSize(MyFunc::ConvertString(texturePath));
        defaultSize_ = size;
    }
}

// デフォルトサイズを取得する
Vector2 Sprite::GetDefaultSize() const{
    if(defaultSize_ != Vector2(0.0f, 0.0f)){
        return defaultSize_;
    }

    if(!texturePath.empty()){
        return SEED::GetImageSize(MyFunc::ConvertString(texturePath));
    } else{
        return Vector2(0.0f);
    }
}


///////////////////////////////////////////////////////////////////////////
// Json保存・読み込み
///////////////////////////////////////////////////////////////////////////
nlohmann::json Sprite::ToJson() const{
    nlohmann::json data;
    data["size"] = size;
    data["texturePath"] = texturePath;
    data["color"] = color;
    data["blendMode"] = static_cast<int>(blendMode);
    data["transform"] = transform;
    data["offset"] = offset;
    data["anchorPoint"] = anchorPoint;
    data["clipLT"] = clipLT;
    data["clipSize"] = clipSize;
    data["uvTransform"] = uvTransform;
    data["flipX"] = flipX;
    data["flipY"] = flipY;
    data["isStaticDraw"] = isStaticDraw;
    data["drawLocation"] = static_cast<int>(drawLocation);
    data["layer"] = layer;
    data["isApplyViewMat"] = isApplyViewMat;
    return data;
}

void Sprite::FromJson(const nlohmann::json& data){

    if(data.empty()){
        return;
    }

    size = data.value("size", size);
    texturePath = data.value("texturePath", texturePath);
    color = data.value("color", color);
    blendMode = static_cast<BlendMode>(data.value("blendMode", static_cast<int>(blendMode)));
    transform = data.value("transform", transform);
    offset = data.value("offset", offset);
    anchorPoint = data.value("anchorPoint", anchorPoint);
    clipLT = data.value("clipLT", clipLT);
    clipSize = data.value("clipSize", clipSize);
    uvTransform = data.value("uvTransform", uvTransform);
    flipX = data.value("flipX", flipX);
    flipY = data.value("flipY", flipY);
    isStaticDraw = data.value("isStaticDraw", isStaticDraw);
    drawLocation = static_cast<DrawLocation>(data.value("drawLocation", static_cast<int>(drawLocation)));
    layer = data.value("layer", layer);
    isApplyViewMat = data.value("isApplyViewMat", false);

    // テクスチャが設定されていれば読み込む
    if(!texturePath.empty()){
        GH = TextureManager::LoadTexture(texturePath);
    } else{
        GH = TextureManager::LoadTexture("DefaultAssets/white1x1.png");
    }
}

///////////////////////////////////////////////////////////////////////////
// 編集関数
///////////////////////////////////////////////////////////////////////////
void Sprite::Edit(){
#ifdef _DEBUG

    if(ImGui::CollapsingHeader("テクスチャ・マテリアル・描画設定")){
        ImGui::Indent();
        static std::filesystem::path rootDir = "Resources/Textures";
        static std::filesystem::path currentDir = rootDir;
        std::string selected = ImFunc::FolderView("テクスチャ選択", currentDir, false, { ".png",".jpg" }, rootDir);
        if(!selected.empty()){
            SetTexture(selected);
        }
        ImGui::Text("テクスチャ: %s", texturePath.c_str());
        if(ImGui::Button("画像サイズに戻す")){
            ToDefaultSize();
        }

        ImGui::ColorEdit4("色", &color.x);
        ImFunc::Combo<BlendMode>("ブレンドモード", blendMode, { "NONE","0MUL" ,"SUB","NORMAL","ADD","SCREEN" });

        ImGui::Unindent();
    }

    if(ImGui::CollapsingHeader("トランスフォーム")){
        ImGui::Indent();
        static bool guizmo = false;
        ImGui::Checkbox("Gizmoで操作", &guizmo);
        ImGui::DragFloat2("スケール", &transform.scale.x, 0.01f);
        ImGui::DragFloat("回転", &transform.rotate, 0.05f);
        ImGui::DragFloat2("移動", &transform.translate.x);
        ImGui::DragFloat2("オフセット", &offset.x);

        if(guizmo){
            if(!parentMat){
                ImGuiManager::RegisterGuizmoItem(&transform);
            } else{
                ImGuiManager::RegisterGuizmoItem(&transform, parentMat->ToMat4x4());
            }
        }
        ImGui::Unindent();
    }

    if(ImGui::CollapsingHeader("サイズ・切り抜き範囲・アンカー詳細")){
        ImGui::Indent();
        ImGui::DragFloat2("サイズ", &size.x, 1.0f, 0.0f, 10000.0f);
        ImGui::DragFloat2("アンカーポイント", &anchorPoint.x, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat2("切り取り左上", &clipLT.x, 1.0f, 0.0f, 10000.0f);
        ImGui::DragFloat2("切り取りサイズ", &clipSize.x, 1.0f, 0.0f, 10000.0f);
        ImGui::Unindent();
    }

    if(ImGui::CollapsingHeader("UVトランスフォーム")){
        ImGui::Indent();
        ImGui::DragFloat2("UVスケール詳細", &uvTransform.scale.x, 0.01f, 0.01f, 10.0f);
        ImGui::DragFloat("UV回転詳細", &uvTransform.rotate, 0.05f);
        ImGui::DragFloat2("UV移動詳細", &uvTransform.translate.x, 0.1f, -10.0f, 10.0f);
        ImGui::Unindent();
    }

    if(ImGui::CollapsingHeader("その他詳細設定")){
        ImGui::Indent();
        ImGui::Checkbox("X反転", &flipX);
        ImGui::Checkbox("Y反転", &flipY);
        ImGui::Checkbox("静的描画", &isStaticDraw);
        ImGui::Checkbox("ビュー行列を適用", &isApplyViewMat);
        ImFunc::Combo<DrawLocation>("描画位置", drawLocation, { "背景","前景" }, 1);
        ImGui::DragInt("描画順(layer)", &layer, 1.0f);
        ImGui::Unindent();
    }

#endif // _DEBUG
}