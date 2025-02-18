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

// 起動時に読み込みたいモデルをここで読み込む
void TextureManager::StartUpLoad(){
}

uint32_t TextureManager::LoadTexture(const std::string& filename){
    // すでに読み込み済みのファイルであればreturn
    if(instance_->graphHandle_.find(filename) != instance_->graphHandle_.end()){ 
        return instance_->graphHandle_[filename];
    }

    // 読み込み
    instance_->graphHandle_[filename] = DxManager::GetInstance()->CreateTexture("resources/textures/" + filename);

    return instance_->graphHandle_[filename];
}

