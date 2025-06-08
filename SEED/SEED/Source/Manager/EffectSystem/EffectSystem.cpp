#include "EffectSystem.h"
#include <SEED/Source/Manager/ImGuiManager/ImGuiManager.h>
#include <SEED/Source/SEED.h>

// emitters
#include <SEED/Source/Object/Particle/Emitter/Emitter.h> 
#include <SEED/Source/Object/Particle/Emitter/Emitter_Model.h>
// particles
#include <SEED/Source/Object/Particle/BaseParticle.h>
#include <SEED/Source/Object/Particle/Particle_Model.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*                                                                                                               */
/*                                            statich変数の初期化                                                  */
/*                                                                                                               */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EffectSystem* EffectSystem::instance_ = nullptr;


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*                                                                                                               */
/*                                                初期化・終了関数                                                  */
/*                                                                                                               */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// デストラクタ
/// </summary>
EffectSystem::~EffectSystem(){
    delete instance_;
    instance_ = nullptr;
}


/// <summary>
/// インスタンスを取得
/// </summary>
/// <returns></returns>
EffectSystem* EffectSystem::GetInstance(){
    if(!instance_){
        instance_ = new EffectSystem();
    }
    return instance_;
}

/// <summary>
/// 初期化
/// </summary>
void EffectSystem::Initialize(){
    // インスタンスを取得
    GetInstance();
    // テクスチャの読み込み
    instance_->LoadParticleTexture();
}

// 起動時のテクスチャの読み込み
void EffectSystem::LoadParticleTexture(){

    // resources/textures/ParticleTextures/ 以下の階層にあるテクスチャを自動で読む
    std::vector<std::string> fileNames;

    // 指定されたディレクトリ内のすべてのファイルを探索
    for(const auto& entry : std::filesystem::directory_iterator("resources/textures/ParticleTextures/")){
        if(entry.is_regular_file()){ // 通常のファイルのみ取得（ディレクトリを除外）
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

void EffectSystem::Update(){


#ifdef _DEBUG
    ImFunc::CustomBegin("エフェクトエディター",MoveOnly_TitleBar);
    ImGui::Checkbox("範囲の描画", &instance_->isFieldVisible_);
    instance_->Edit();
    ImGui::End();
#endif // _DEBUG

    ////////////////////////////////////////////
    // エミッターの更新
    ////////////////////////////////////////////

    // エフェクトのエミッター更新
    for(auto& effect : instance_->onceEffects_){
        for(auto& emitter : effect->emitters){
            emitter->Update();
            if(emitter->emitOrder == true){
                Emit(emitter.get());
            }
        }
    }

    // endlessEffectsの更新
    for(auto& effect : instance_->endlessEffects_){
        for(auto& emitter : effect.second->emitters){
            emitter->Update();
            if(emitter->emitOrder == true){
                Emit(emitter.get());
            }
        }
    }

    // エディターのエミッターグループの更新
    for(auto& emitterGroup : instance_->emitterGroups_){
        for(auto& emitter : emitterGroup->emitters){
            emitter->Update();
            if(emitter->emitOrder == true){
                Emit(emitter.get());
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

    instance_->onceEffects_.remove_if([](auto& effect){
        return !effect->GetIsAlive();
    });

    instance_->endlessEffects_.remove_if([](auto& effect){
        return !effect.second->GetIsAlive();
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

void EffectSystem::Draw(){

    // パーティクルの描画
    for(auto& particle : instance_->particles_){
        particle->Draw();
    }

#ifdef _DEBUG
    if(instance_->isFieldVisible_){
        // 加速フィールドの描画
        for(auto& accelerationField : instance_->accelerationFields_){
            accelerationField->Draw();
        }

        // エミッターグループの描画
        for(auto& emitterGroup : instance_->emitterGroups_){
            for(auto& emitter : emitterGroup->emitters){
                emitter->DrawEditData();
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
void EffectSystem::CreateAccelerationField(const Range3D& range, const Vector3& force){
    instance_->accelerationFields_.emplace_back(
        std::make_unique<AccelerationField>(force, range)
    );
}


/// <summary>
/// エフェクトを出現させる
/// </summary>
/// <param name="fileName"></param>
/// <param name="parentMat"></param>
void EffectSystem::AddEffectOnce(const std::string& fileName, const Vector3& position, const Matrix4x4* parentMat){

    // エフェクトデータを取得
    auto& effect = instance_->onceEffects_.emplace_back(std::make_unique<EmitterGroup>());
    instance_->LoadFromJson(effect.get(), fileName);
    effect->offset = position;
    effect->parentMat = parentMat;
    effect->TeachParent();
}

void EffectSystem::AddEffectOnce(const std::string& fileName, const Matrix4x4* parentMat){
    AddEffectOnce(fileName, { 0.0f,0.0f,0.0f }, parentMat);
}

void EffectSystem::AddEffectOnce(const std::string& fileName, const Vector3& position){
    AddEffectOnce(fileName, position, nullptr);
}

uint32_t EffectSystem::AddEffectEndless(const std::string& fileName, const Vector3& position, const Matrix4x4* parentMat){

    // endlessEffectsから使われていないハンドルを探す
    uint32_t handle = 0;
    while(true){
        uint32_t prev = handle;
        for(auto& effect : instance_->endlessEffects_){
            if(handle == effect.first){
                handle++;
                break; // 使われているハンドルが見つかったので、次のハンドルを探す
            }
        }

        // 値が同じ場合、使われていないハンドル
        if(prev == handle){ 
            break;
        }
    }

    // エフェクトデータを取得
    auto& effect = instance_->endlessEffects_.emplace_back(std::make_pair(handle, std::make_unique<EmitterGroup>()));
    instance_->LoadFromJson(effect.second.get(), fileName);
    effect.second->offset = position;
    effect.second->parentMat = parentMat;
    effect.second->TeachParent();

    // ハンドルを返す
    return handle;
}

/// <summary>
/// エフェクトを削除する
/// </summary>
void EffectSystem::DeleteAll(){
    instance_->particles_.clear();
    instance_->emitterGroups_.clear();
    instance_->onceEffects_.clear();
    instance_->effectData_.clear();
    instance_->accelerationFields_.clear();
}

void EffectSystem::DeleteEffect(uint32_t handle){
    // ハンドルに対応するエフェクトを削除
    auto it = std::find_if(instance_->endlessEffects_.begin(), instance_->endlessEffects_.end(),
        [handle](const std::pair<uint32_t, std::unique_ptr<EmitterGroup>>& effect){
        return effect.first == handle;
    });

    if(it != instance_->endlessEffects_.end()){
        instance_->endlessEffects_.erase(it);
    }
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
void EffectSystem::Emit(Emitter_Base* emitter){

    if(!emitter->emitOrder){ return; }
    if(!emitter->isActive && !emitter->isEdittting){ return; }

    // パーティクルを発生させる
    for(int32_t i = 0; i < emitter->numEmitEvery; ++i){

        switch(emitter->particleType){
        case ParticleType::kRadial:
            instance_->particles_.emplace_back(std::make_unique<Particle_Model>(emitter));
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


void EffectSystem::Edit(){

    int32_t emitterGroupNo = 0;
    static EmitterGroup* currentEmitterGroup = nullptr;
    static auto selectedItEmitterGroup = instance_->emitterGroups_.begin();
    std::string label = "";


    ImGui::BeginChild("クループの管理", ImVec2(250, 350), true);
    {
        for(auto itEmitterGroup = instance_->emitterGroups_.begin(); itEmitterGroup != instance_->emitterGroups_.end();){
            
            // エミッターグループを取得
            EmitterGroup* emitterGroup = itEmitterGroup->get();
            emitterGroup->Reactivation(); // エミッターグループの再活性化処理

            // タブの名前の決定
            emitterGroup->name = "グループ(" + std::to_string(emitterGroupNo) + ")";

            // 各エミッターグループタブの開閉
            if(ImGui::Button(emitterGroup->name.c_str())){
                // 現在のエミッターグループを設定
                currentEmitterGroup = emitterGroup;
                selectedItEmitterGroup = itEmitterGroup; // 選択されたエミッターグループを保存
                ImGui::SetWindowFocus(emitterGroup->name.c_str()); // タブを選択状態にする
            }

            ++itEmitterGroup;
            ++emitterGroupNo;
        }


        ImGui::Separator();

        // エミッターの追加ボタン
        ImGui::Text("-- エミッターグループの追加 --");
        if(ImGui::Button("グループの追加")){
            instance_->emitterGroups_.emplace_back(std::make_unique<EmitterGroup>());
            instance_->emitterGroups_.back()->isEditMode_ = true; // 編集モードにする
        }

        // ホットリロード
        ImGui::Text("-- ファイルから読み込む --");
        if(ImGui::CollapsingHeader("ファイル一覧")){
            ImGui::Indent();
            Load();
            ImGui::Unindent();
        }

    } ImGui::EndChild();
    ImGui::SameLine();

    // 選択されているグループの 編集を行う
    if(currentEmitterGroup){


        // エミッターの選択・追加/グループの削除
        ImGui::BeginChild(currentEmitterGroup->name.c_str(), ImVec2(0, 350), true);
        {
            // 選択されているエミッターグループの情報を表示
            ImGui::Text(currentEmitterGroup->name.c_str());
            ImGui::Separator();

            // オフセットの設定
            ImGui::Text("グループ単位の座標設定");
            label = "offset" + currentEmitterGroup->idTag_;
            ImGui::DragFloat3(label.c_str(), &currentEmitterGroup->offset.x, 0.05f);
            ImGui::Separator();

            // エミッターグループの編集
            currentEmitterGroup->Edit();

            // 削除ボタン
            ImGui::Separator();
            ImGui::Text("削除");
            label = "グループの削除" + currentEmitterGroup->idTag_;
            if(ImGui::Button(label.c_str())){
                ImGui::OpenPopup("削除しますか?");
            }

            // 削除ボタンを押したらポップアップを出し確認をする
            if(ImGui::BeginPopupModal("削除しますか?")){
                ImGui::Text("本当に削除しますか？");

                // OKボタンを押したら削除
                if(ImGui::Button("OK", ImVec2(120, 0))){
                    instance_->emitterGroups_.erase(selectedItEmitterGroup); // 要素を削除し、次の要素を取得
                    currentEmitterGroup->selectedEmitter_ = nullptr; // 選択されているエミッターをリセット
                    currentEmitterGroup = nullptr; // 現在のエミッターグループをリセット
                    ImGui::CloseCurrentPopup();
                }

                // キャンセルボタンを押したらポップアップを閉じる
                if(ImGui::Button("Cancel", ImVec2(120, 0))){
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }

        }ImGui::EndChild();
        //ImGui::SameLine();
    }


    // 選択しているエミッターの編集
    if(currentEmitterGroup && currentEmitterGroup->selectedEmitter_){

        ImGui::Text("---------- エミッターの編集 ----------");

        // エミッターの情報編集・削除
        ImGui::BeginChild("エミッター", ImVec2(0, 0), true);
        {
            // 選択されているエミッターの名前を表示
            ImGui::Text(currentEmitterGroup->selectedEmitterName_.c_str());

            // エミッターの一般的な情報を編集
            Emitter_Base* emitter = currentEmitterGroup->selectedEmitter_;
            emitter->Edit();

            // 削除ボタン
            ImGui::Separator();
            label = "エミッターの削除" + emitter->idTag_;
            if(ImGui::Button(label.c_str())){
                ImGui::OpenPopup("削除しますか?");
            }

            // 削除ボタンを押したらポップアップを出し確認をする
            if(ImGui::BeginPopupModal("削除しますか?")){
                ImGui::Text("本当に削除しますか？");

                // OKボタンを押したら削除
                if(ImGui::Button("OK", ImVec2(120, 0))){
                    currentEmitterGroup->emitters.erase(currentEmitterGroup->selectedItEmitter_);
                    currentEmitterGroup->selectedEmitter_ = nullptr; // 選択されているエミッターをリセット
                    ImGui::CloseCurrentPopup();
                }

                // キャンセルボタンを押したらポップアップを閉じる
                if(ImGui::Button("Cancel", ImVec2(120, 0))){
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }

        }ImGui::EndChild();
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*                                                                                                               */
/*                                                  ファイル入出力                                                 */
/*                                                                                                               */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


EmitterGroup EffectSystem::LoadFromJson(const std::string& fileName){

    EmitterGroup emitterGroup;
    std::ifstream ifs("resources/jsons/particle/" + fileName);
    if(ifs.fail()){
        assert(false);
    }

    nlohmann::json j;
    ifs >> j;

    // エミッターの情報を読み込み
    for(auto& emitterJson : j["emitters"]){
        std::unique_ptr<Emitter_Base> emitter;

        // エミッターの種類によってインスタンスを生成
        if(emitterJson["emitterType"] == "Emitter_Model3D"){
            emitter = std::make_unique<Emitter_Model>();
        } else{
            // 他の型への対応があるならここに追加
            continue; // 未対応タイプはスキップ
        }

        emitter->LoadFromJson(emitterJson);
        emitterGroup.emitters.emplace_back(std::move(emitter));
    }

    emitterGroup.name = fileName;
    return emitterGroup;
}

void EffectSystem::LoadFromJson(EmitterGroup* emitterGroup, const std::string& fileName){

    std::ifstream ifs("resources/jsons/particle/" + fileName);
    if(ifs.fail()){
        assert(false);
    }

    nlohmann::json j;
    ifs >> j;

    // エミッターの情報を読み込み
    for(auto& emitterJson : j["emitters"]){
        std::unique_ptr<Emitter_Base> emitter;

        // エミッターの種類によってインスタンスを生成
        if(emitterJson["emitterType"] == "Emitter_Model3D"){
            emitter = std::make_unique<Emitter_Model>();
        } else{
            // 他の型への対応があるならここに追加
            continue; // 未対応タイプはスキップ
        }

        emitter->LoadFromJson(emitterJson);
        emitterGroup->emitters.emplace_back(std::move(emitter));
    }

    emitterGroup->name = fileName;
}

/// <summary>
/// Particleフォルダ内のファイルを選択してロード
/// </summary>
void EffectSystem::Load(){
    // ファイル一覧を取得
    std::vector<std::string> fileNames;
    for(const auto& entry : std::filesystem::directory_iterator("resources/jsons/particle/")){
        if(entry.is_regular_file()){ // 通常のファイルのみ取得（ディレクトリを除外）
            // もしファイル名が".json"で終わっていたら
            if(entry.path().extension() == ".json"){
                // ファイル名を追加
                fileNames.push_back(entry.path().filename().string());
            }
        }
    }

    // ファイルを選択して読み込み
    ImGui::Text("読み込むファイルを選択");
    for(auto& fileName : fileNames){
        if(ImGui::Button(fileName.c_str())){

            // ファイルを読み込み、追加
            auto& effect = instance_->emitterGroups_.emplace_back(std::make_unique<EmitterGroup>());
            instance_->LoadFromJson(effect.get(), fileName);
            effect->TeachParent();
            effect->isEditMode_ = true; // 編集モードにする

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
void EffectSystem::CollisionParticle2Field(){
    for(auto& particle : instance_->particles_){
        for(auto& field : instance_->accelerationFields_){
            if(field->CheckCollision(particle->GetPos())){
                particle->SetAcceleration(field->acceleration);
            }
        }
    }
}


