#include <SEED/Source/Manager/ParticleManager/ParticleManager.h>
#include <SEED/Source/Manager/ImGuiManager/ImGuiManager.h>
#include <SEED/Source/SEED.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*                                                                                                               */
/*                                            statich変数の初期化                                                  */
/*                                                                                                               */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ParticleManager* ParticleManager::instance_ = nullptr;


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*                                                                                                               */
/*                                                初期化・終了関数                                                  */
/*                                                                                                               */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// デストラクタ
/// </summary>
ParticleManager::~ParticleManager(){
    delete instance_;
    instance_ = nullptr;
}


/// <summary>
/// インスタンスを取得
/// </summary>
/// <returns></returns>
ParticleManager* ParticleManager::GetInstance(){
    if(!instance_){
        instance_ = new ParticleManager();
    }
    return instance_;
}

/// <summary>
/// 初期化
/// </summary>
void ParticleManager::Initialize(){
    // インスタンスを取得
    GetInstance();
    // テクスチャの読み込み
    instance_->LoadParticleTexture();
}

// 起動時のテクスチャの読み込み
void ParticleManager::LoadParticleTexture(){

    // resources/textures/ParticleTextures/ 以下の階層にあるテクスチャを自動で読む
    std::vector<std::string> fileNames;

    // 指定されたディレクトリ内のすべてのファイルを探索
    for(const auto& entry : std::filesystem::directory_iterator("resources/textures/ParticleTextures/")) {
        if(entry.is_regular_file()) { // 通常のファイルのみ取得（ディレクトリを除外）
            // もしファイル名が".png"で終わっていたら
            if(entry.path().extension() == ".png"){
                // ファイル名を追加
                fileNames.push_back("ParticleTextures/" + entry.path().filename().string()); // ファイル名のみ追加
            }
        }
    }

    // テクスチャの読み込み
    for(const auto& fileName : fileNames){

        // テクスチャの読み込み
        int handle = TextureManager::LoadTexture(fileName);

        // GPUハンドルを取得
        Emitter_Base::textureDict[fileName] =
            (ImTextureID)ViewManager::GetHandleGPU(HEAP_TYPE::SRV_CBV_UAV, handle).ptr;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*                                                                                                               */
/*                                                     更新処理                                                   */
/*                                                                                                               */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ParticleManager::Update(){


#ifdef _DEBUG
    ImGui::Begin("ParticleEditor");
    ImGui::Checkbox("isFieldVisible", &instance_->isFieldVisible_);
    instance_->Edit();
    ImGui::End();
#endif // _DEBUG

    ////////////////////////////////////////////
    // エミッターの更新
    ////////////////////////////////////////////
    for(auto& emitter : instance_->emitters_){
        emitter->Update();

        // 発生命令が出ていればパーティクルを発生させる
        if(emitter->emitOrder == true){
            Emit(emitter.get());
        }
    }

    // エフェクトのエミッター更新
    for(auto& effect : instance_->effects_){
        for(auto& emitter : effect->emitters){
            emitter->Update();
            if(emitter->emitOrder == true){
                Emit(emitter);
            }
        }
    }

    // エディターのエミッターグループの更新
    for(auto& emitterGroup : instance_->emitterGroups_){
        for(auto& emitter : emitterGroup->emitters){
            emitter->Update();
            if(emitter->emitOrder == true){
                Emit(emitter);
            }
        }
    }

    ////////////////////////////////////////////
    // パーティクルの更新
    ////////////////////////////////////////////

    for(auto& particle : instance_->particles_){
        particle->Update();
    }

    ////////////////////////////////////////////
    // パーティクルとフィールドの衝突判定
    ////////////////////////////////////////////
    if(instance_->isFieldActive_){
        instance_->CollisionParticle2Field();
    }

    ////////////////////////////////////////////
    // 死んでいる要素の削除
    ////////////////////////////////////////////
    instance_->particles_.remove_if([](auto& particle){
        return !particle->GetIsAlive();
    });

    instance_->emitters_.remove_if([](auto& emitter){
        return !emitter->isAlive;
    });

    instance_->effects_.remove_if([](auto& effect){
        return !effect->GetIsAlive();
    });

    instance_->emitterGroups_.remove_if([](auto& emitterGroup){
        return !emitterGroup->GetIsAlive();
    });
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*                                                                                                               */
/*                                                   描画関数                                                      */
/*                                                                                                               */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ParticleManager::Draw(){

    // パーティクルの描画
    for(auto& particle : instance_->particles_){
        particle->Draw();
    }

#ifdef _DEBUG
    if(instance_->isFieldVisible_){
        // 加速フィールドの描画
        for(auto& accelerationField : instance_->accelerationFields_){
            AABB area;
            area.center = (accelerationField->range.min + accelerationField->range.max) * 0.5f;
            area.halfSize = (accelerationField->range.max - accelerationField->range.min) * 0.5f;
            SEED::DrawAABB(area, { 1.0f,0.0f,0.0f,1.0f });
        }

        // エミッターの描画
        for(auto& emitter : instance_->emitters_){
            AABB area;
            area.center = emitter->GetCenter();
            area.halfSize = emitter->emitRange * 0.5f;
            SEED::DrawAABB(area, { 0.0f,0.0f,1.0f,1.0f });
        }

        // エミッターグループの描画
        for(auto& emitterGroup : instance_->emitterGroups_){
            for(auto& emitter : emitterGroup->emitters){
                AABB area;
                area.center = emitter->GetCenter();
                area.halfSize = emitter->emitRange * 0.5f;
                SEED::DrawAABB(area, { 0.0f,0.0f,1.0f,1.0f });
            }
        }
    }
#endif // _DEBUG
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*                                                                                                               */
/*                                              領域を追加する関数                                                  */
/*                                                                                                               */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// 加速フィールドを作成する
/// </summary>
/// <param name="range">フィールドの範囲</param>
/// <param name="force">加速度</param>
void ParticleManager::CreateAccelerationField(const Range3D& range, const Vector3& force){
    instance_->accelerationFields_.emplace_back(
        std::make_unique<AccelerationField>(force, range)
    );
}


/// <summary>
/// エフェクトを出現させる
/// </summary>
/// <param name="fileName"></param>
/// <param name="parentMat"></param>
void ParticleManager::AddEffect(const std::string& fileName, const Vector3& position, const Matrix4x4* parentMat){

    // ファイル名が登録されていない場合は読み込む
    if(instance_->effectData_.find(fileName) == instance_->effectData_.end()){
        instance_->effectData_[fileName] = instance_->LoadFromJson(fileName);
    }

    // エフェクトデータを取得
    auto& effect = instance_->effects_.emplace_back(std::make_unique<EmitterGroup>(instance_->effectData_[fileName]));
    effect->offset = position;
    effect->parentMat = parentMat;
    effect->TeachParent();
}

void ParticleManager::AddEffect(const std::string& fileName, const Matrix4x4* parentMat){
    AddEffect(fileName, { 0.0f,0.0f,0.0f }, parentMat);
}

void ParticleManager::AddEffect(const std::string& fileName, const Vector3& position){
    AddEffect(fileName, position, nullptr);
}

/// <summary>
/// エフェクトを削除する
/// </summary>
void ParticleManager::DeleteAll(){
    instance_->particles_.clear();
    instance_->emitters_.clear();
    instance_->emitterGroups_.clear();
    instance_->effects_.clear();
    instance_->effectData_.clear();
    instance_->accelerationFields_.clear();
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*                                                                                                               */
/*                                          パーティクルを追加する関数                                               */
/*                                                                                                               */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// パーティクルを発生させる
/// </summary>
/// <param name="emitter"></param>
void ParticleManager::Emit(Emitter_Base* emitter){

    if(!emitter->emitOrder){ return; }
    if(!emitter->isActive && !emitter->isEdittting){ return; }

    // パーティクルを発生させる
    for(int32_t i = 0; i < emitter->numEmitEvery; ++i){

        switch(emitter->particleType){
        case ParticleType::kRadial:
            instance_->particles_.emplace_back(std::make_unique<RadialParticle>(emitter));
            break;
        }
    }

    // 発生命令をリセット
    emitter->emitOrder = false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*                                                                                                               */
/*                                          大元のエフェクト編集関数                                                 */
/*                                                                                                               */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void ParticleManager::Edit(){

    int32_t emitterGroupNo = 0;
    for(auto itEmitterGroups = instance_->emitterGroups_.begin(); itEmitterGroups != instance_->emitterGroups_.end();){
        // エミッターグループを取得
        EmitterGroup* emitterGroup = itEmitterGroups->get();

        // タブの名前の決定
        std::string headerName = "EmitterGroup (" + std::to_string(emitterGroupNo) + ")";
        if(emitterGroup->name != ""){
            emitterGroup->name = emitterGroup->name + "(" + std::to_string(emitterGroupNo) + ")";
        }

        // 各エミッターグループタブの開閉
        if(ImGui::CollapsingHeader(headerName.c_str())){
            ImGui::Indent();

            // オフセットの設定
            ImGui::DragFloat3("offset", &emitterGroup->offset.x, 0.05f);

            // エミッターグループの編集
            emitterGroup->Edit();

            // 削除ボタン
            if(ImGui::Button("Delete")){
                ImGui::OpenPopup("Delete?");
            }

            // 削除ボタンを押したらポップアップを出し確認をする
            if(ImGui::BeginPopupModal("Delete?")){
                ImGui::Text("Are you sure you want to delete?");

                // OKボタンを押したら削除
                if(ImGui::Button("OK", ImVec2(120, 0))){
                    itEmitterGroups = instance_->emitterGroups_.erase(itEmitterGroups); // 要素を削除し、次の要素を取得
                    ImGui::CloseCurrentPopup();
                    ImGui::EndPopup();
                    continue; // 削除したので次の要素に進む
                }

                // キャンセルボタンを押したらポップアップを閉じる
                if(ImGui::Button("Cancel", ImVec2(120, 0))){
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }

            ImGui::Separator();
            ImGui::Unindent();
        }

        ++itEmitterGroups;
        ++emitterGroupNo;
    }

    ImGui::Separator();

    // エミッターの追加ボタン
    ImGui::Text("-- AddEmitterGroup --");
    if(ImGui::Button("AddEmitterGroup")){
        instance_->emitterGroups_.emplace_back(std::make_unique<EmitterGroup>());
    }

    // ホットリロード
    ImGui::Text("-- Load --");
    if(ImGui::CollapsingHeader("Load")){
        ImGui::Indent();
        Load();
        ImGui::Unindent();
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*                                                                                                               */
/*                                                  ファイル入出力                                                 */
/*                                                                                                               */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/// <summary>
/// Jsonファイルから読み込み
/// </summary>
void ParticleManager::LoadFromJson(EmitterGroup* emitterGroup, const std::string& fileName){
    *emitterGroup = LoadFromJson(fileName);
}

EmitterGroup ParticleManager::LoadFromJson(const std::string& fileName){

    EmitterGroup emitterGroup;
    std::ifstream ifs("resources/jsons/particle/" + fileName);
    if(ifs.fail()){
        assert(false);
    }

    nlohmann::json j;
    ifs >> j;

    // エミッターの情報を読み込み
    for(auto& emitterJson : j["emitters"]){
        Emitter_Base* emitter = nullptr;

        // エミッターの種類によってインスタンスを生成
        if(emitterJson["emitterType"] == "Emitter_Plane3D"){
            emitter = new Emitter_Plane3D();
        }

        // エミッターの情報を読み込み
        emitter->LoadFromJson(emitterJson);
        emitterGroup.emitters.emplace_back(emitter);
    }

    emitterGroup.name = fileName;
    return emitterGroup;
}

/// <summary>
/// Particleフォルダ内のファイルを選択してロード
/// </summary>
void ParticleManager::Load(){
    // ファイル一覧を取得
    std::vector<std::string> fileNames;
    for(const auto& entry : std::filesystem::directory_iterator("resources/jsons/particle/")) {
        if(entry.is_regular_file()) { // 通常のファイルのみ取得（ディレクトリを除外）
            // もしファイル名が".json"で終わっていたら
            if(entry.path().extension() == ".json"){
                // ファイル名を追加
                fileNames.push_back(entry.path().filename().string());
            }
        }
    }

    // ファイルを選択して読み込み
    ImGui::Text("Please select file");
    for(auto& fileName : fileNames){
        if(ImGui::Button(fileName.c_str())){

            // ファイルを読み込み、追加
            EmitterGroup emitterGroup = LoadFromJson(fileName);
            instance_->emitterGroups_.emplace_back(std::make_unique<EmitterGroup>(emitterGroup));
            instance_->emitterGroups_.back()->TeachParent();

            for(auto& emitter : instance_->emitterGroups_.back()->emitters){
                emitter->isEdittting = true;
            }
        }
    }
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*                                                                                                               */
/*                                パーティクルと加速フィールドの当たり判定関数                                          */
/*                                                                                                               */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// パーティクルと加速フィールドの衝突判定
/// </summary>
void ParticleManager::CollisionParticle2Field(){
    for(auto& particle : instance_->particles_){
        for(auto& field : instance_->accelerationFields_){
            if(field->CheckCollision(particle->GetPos())){
                particle->SetAcceleration(field->acceleration);
            }
        }
    }
}


