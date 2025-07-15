#include <SEED/Source/Manager/TextureManager/TextureManager.h>
#include <SEED/Source/SEED.h>
#include <SEED/Source/Manager/DxManager/DxManager.h>

// static変数初期化
TextureManager* TextureManager::instance_ = nullptr;

TextureManager::~TextureManager(){
    if(instance_){
        delete instance_;
        instance_ = nullptr;
    }
}

const TextureManager* TextureManager::GetInstance(){
    if(!instance_){
        instance_ = new TextureManager();
    }

    return instance_;
}

void TextureManager::Initialize(){
    // インスタンスなければ作成
    GetInstance();

    // 初期化時読み込み
    StartUpLoad();
}

//////////////////////////////////////////////
// 解放処理
//////////////////////////////////////////////
void TextureManager::Release(){
    // 各テクスチャの解放
    instance_->textureResources.clear();
    instance_->intermediateResources.clear();
}

// 起動時に読み込みたいモデルをここで読み込む
void TextureManager::StartUpLoad(){
}

uint32_t TextureManager::LoadTexture(const std::string& filename, const aiTexture* embeddedTexture){
    // すでに読み込み済みのファイルであればreturn
    if(instance_->graphHandle_.find(filename) != instance_->graphHandle_.end()){
        return instance_->graphHandle_[filename];
    }

    // filenameが"Resources/"から始まる場合、basePathを空に
    std::string basePath = "Resources/textures/";
    if(filename.starts_with("Resources/")){
        basePath = "";
    }

    // 埋め込みテクスチャでない場合
    if(!embeddedTexture){
        // 読み込み
        instance_->graphHandle_[filename] = instance_->CreateTexture(basePath + filename);
    } else{
        // 埋め込みテクスチャの場合
        instance_->graphHandle_[filename] = instance_->CreateTexture(basePath + filename, embeddedTexture);
    }

    return instance_->graphHandle_[filename];
}


uint32_t TextureManager::CreateTexture(const std::string& filename, const aiTexture* embeddedTexture){

    // 既にある場合
    if(ViewManager::GetTextureHandle(filename) != -1){ return ViewManager::GetTextureHandle(filename); }

    /*----------------------------- TextureResourceの作成,転送 -----------------------------*/

    // 読み込み
    DirectX::ScratchImage mipImages;
    if(!embeddedTexture){
        mipImages = LoadTextureImage(filename);// 通常のテクスチャ
    } else{
        mipImages = LoadEmbeddedTextureImage(embeddedTexture);// 埋め込みテクスチャ
    }

    // 作成
    const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
    textureResources.push_back(CreateTextureResource(DxManager::GetInstance()->GetDevice(), metadata));
    // 転送
    intermediateResources.push_back(
        UploadTextureData(
            textureResources.back().Get(), mipImages,
            DxManager::GetInstance()->GetDevice(),
            DxManager::GetInstance()->commandList.Get()
        )
    );

    /*-------------------------------- Texture用SRVの作成 ----------------------------------*/

    // metaDataをもとにSRVの設定
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = metadata.format;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

    // キューブマップかどうか
    if(!metadata.IsCubemap()){
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
        srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);
    } else{
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;//キューブマップ
        srvDesc.TextureCube.MipLevels = UINT_MAX;
        srvDesc.TextureCube.MostDetailedMip = 0;
        srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
    }

    // SRVの作成(グラフハンドルを返す)
    return ViewManager::CreateView(VIEW_TYPE::SRV, textureResources.back().Get(), &srvDesc, filename);

}

// テクスチャハンドルを返す
uint32_t TextureManager::GetGraphHandle(const std::string& fileName){
    // ない場合読み込み
    if(instance_->graphHandle_.find(fileName) == instance_->graphHandle_.end()){
        // 拡張子がある場合のみ読み込み(png、jpgのみ)
        if(fileName.find(".png") != std::string::npos || fileName.find(".jpg") != std::string::npos){
            LoadTexture(fileName);
        }
    }
    return instance_->graphHandle_[fileName];
}

D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetHandleGPU(const std::string& fileName){
    // ない場合読み込み
    if(instance_->graphHandle_.find(fileName) == instance_->graphHandle_.end()){
        // 拡張子がある場合のみ読み込み(png、jpgのみ)
        if(fileName.find(".png") != std::string::npos || fileName.find(".jpg") != std::string::npos){
            LoadTexture(fileName);
        }
    }

    if(instance_->graphHandle_.find(fileName) != instance_->graphHandle_.end()){
        return ViewManager::GetHandleGPU(HEAP_TYPE::SRV_CBV_UAV, instance_->graphHandle_[fileName]);
    } else{
        return ViewManager::GetHandleGPU(HEAP_TYPE::SRV_CBV_UAV, fileName);
    }
}

ImTextureID TextureManager::GetImGuiTexture(const std::string& fileName){
    // ない場合読み込み
    if(instance_->graphHandle_.find(fileName) == instance_->graphHandle_.end()){
        // 拡張子がある場合のみ読み込み(png、jpgのみ)
        if(fileName.find(".png") != std::string::npos || fileName.find(".jpg") != std::string::npos){
            LoadTexture(fileName);
        }
    }
    return ImTextureID(GetHandleGPU(fileName).ptr);
}
