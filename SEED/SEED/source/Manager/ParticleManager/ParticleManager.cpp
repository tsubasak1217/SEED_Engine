#include "ParticleManager.h"
#include "ImGuiManager.h"
#include "SEED.h"

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
        textureIDs_[fileName] =
            (ImTextureID)ViewManager::GetHandleGPU(DESCRIPTOR_HEAP_TYPE::SRV_CBV_UAV, handle).ptr;
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
    instance_->EditAll();
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
            emitter.Update();
            if(emitter.emitOrder == true){
                Emit(&emitter);
            }
        }
    }

    // エディターのエミッターグループの更新
    for(auto& emitterGroup : instance_->emitterGroups_){
        for(auto& emitter : emitterGroup->emitters){
            emitter.Update();
            if(emitter.emitOrder == true){
                Emit(&emitter);
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
            // パーティクルの進行方向を描画
            SEED::DrawLine(
                area.center,
                area.center + MyMath::Normalize(emitter->baseDirection) * 5.0f,
                { 1.0f,0.0f,1.0f,1.0f }
            );
        }

        // エミッターグループの描画
        for(auto& emitterGroup : instance_->emitterGroups_){
            for(auto& emitter : emitterGroup->emitters){
                AABB area;
                area.center = emitter.GetCenter();
                area.halfSize = emitter.emitRange * 0.5f;
                SEED::DrawAABB(area, { 0.0f,0.0f,1.0f,1.0f });
                // パーティクルの進行方向を描画
                SEED::DrawLine(
                    area.center,
                    area.center + MyMath::Normalize(emitter.baseDirection) * 5.0f,
                    { 1.0f,0.0f,1.0f,1.0f }
                );
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
/// エミッターを追加する
/// </summary>
/// <param name="emitter"></param>
void ParticleManager::AddEmitter(const Emitter& emitter){
    instance_->emitters_.emplace_back(std::make_unique<Emitter>(emitter));
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


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*                                                                                                               */
/*                                          パーティクルを追加する関数                                               */
/*                                                                                                               */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// パーティクルを発生させる
/// </summary>
/// <param name="emitter"></param>
void ParticleManager::Emit(Emitter* emitter){

    if(!emitter->emitOrder){ return; }
    if(!emitter->isActive){ return; }

    // パーティクルを発生させる
    for(int32_t i = 0; i < emitter->numEmitEvery; ++i){

        switch(emitter->particleType){
        case ParticleType::kRadial:
            instance_->particles_.emplace_back(std::make_unique<RadialParticle>(*emitter));
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


void ParticleManager::EditAll(){

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
            EditEmitterGroup(emitterGroup);

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
/*                                          各EmitterGroupを編集する関数                                            */
/*                                                                                                               */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ParticleManager::EditEmitterGroup(EmitterGroup* emitterGroup){

    // エミッターごとに編集
    int32_t emitterNo = 0;
    for(auto itEmitter = emitterGroup->emitters.begin(); itEmitter != emitterGroup->emitters.end();){

        std::string name = "Emitter_" + std::to_string(emitterNo);
        Emitter& emitter = *itEmitter; // イテレーターが指す値にアクセス

        if(ImGui::CollapsingHeader(name.c_str())){
            ImGui::Indent();

            // エミッターの編集
            EditEmitter(&emitter);

            // 削除ボタン
            if(ImGui::Button("DeleteEmitter")){
                ImGui::OpenPopup("Delete?");
            }

            // 削除ボタンを押したらポップアップを出し確認をする
            if(ImGui::BeginPopupModal("Delete?")){
                ImGui::Text("Are you sure you want to delete?");

                // OKボタンを押したら削除
                if(ImGui::Button("OK", ImVec2(120, 0))){
                    itEmitter = emitterGroup->emitters.erase(itEmitter); // 要素を削除し、次の要素を取得
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

            ImGui::Unindent();
        }

        ++itEmitter;// 次の要素に進む
        ++emitterNo;
    }

    ImGui::Text("-------------");

    // エミッターの追加ボタン
    if(ImGui::Button("AddEmitter")){
        emitterGroup->emitters.emplace_back(Emitter());
        emitterGroup->emitters.back().parentGroup = emitterGroup;
    }

    // エミッターグループをjsonに保存
    if(ImGui::Button("OutputToJson")){
        // 入力前に文字列をクリア
        for(auto& character : outputFileName_){ character = '\0'; }
        // 名前があれば入れておく
        if(emitterGroup->name != ""){
            for(int32_t i = 0; i < emitterGroup->name.size(); ++i){
                outputFileName_[i] = emitterGroup->name[i];
            }
        }

        ImGui::OpenPopup("OutputToJson");
    }
    OutputEmitterGroup(*emitterGroup);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*                                                                                                               */
/*                                            各Emitterを編集する関数                                               */
/*                                                                                                               */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ParticleManager::EditEmitter(Emitter* emitter){

    // 全般の情報
    if(ImGui::CollapsingHeader("General")){
        ImGui::Indent();
        EditGeneral(emitter);
        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        ImGui::Unindent();
    }


    // 範囲などの情報
    if(ImGui::CollapsingHeader("Ranges / Parameters")){
        ImGui::Indent();
        EditRangeParameters(emitter);
        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        ImGui::Unindent();
    }


    // マテリアルなどの情報
    if(ImGui::CollapsingHeader("Material")){
        ImGui::Indent();
        EditMaterial(emitter);
        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        ImGui::Unindent();
    }


    // 頻度などの情報
    if(ImGui::CollapsingHeader("Frequency")){
        ImGui::Indent();
        EditFrequency(emitter);
        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        ImGui::Unindent();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*                                                                                                               */
/*                                        エミッター内の各パラメータ編集                                              */
/*                                                                                                               */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*------------------------*/
/*        全般の情報       */
/*------------------------*/
void ParticleManager::EditGeneral(Emitter* emitter){
    ImGui::Checkbox("isActive", &emitter->isActive);
    ImGui::Checkbox("isBillboard", &emitter->isBillboard);
    if(ImGui::Checkbox("isUseRotate", &emitter->isUseRotate)){ emitter->isBillboard = false; }
    ImGui::Checkbox("isUseGravity", &emitter->isUseGravity);
    ImGui::DragFloat3("center", &emitter->center.x, 0.05f);
    ImGui::Combo("emitType", (int*)&emitter->emitType, "kOnce\0kInfinite\0kCustom\0");
    ImGui::Combo("particleType", (int*)&emitter->particleType, "kRadial\0");
    ImGui::Combo(
        "blendMode", (int*)&emitter->blendMode,
        "NONE\0MULTIPLY\0SUBTRACT\0NORMAL\0ADD\0SCREEN\0"
    );
    ImGui::Combo("CullingMode", (int*)&emitter->cullingMode, "NONE\0FRONT\0BACK\0");
}


/*------------------------*/
/*      範囲などの情報      */
/*------------------------*/
void ParticleManager::EditRangeParameters(Emitter* emitter){
    ImGui::Text("-------- Emit --------");
    ImGui::DragFloat3("emitRange", &emitter->emitRange.x, 0.05f);
    ImGui::Text("------- Radius -------");
    ImGui::DragFloat("radiusRange.min", &emitter->radiusRange.min, 0.01f, 0.0f, emitter->radiusRange.max);
    ImGui::DragFloat("radiusRange.max", &emitter->radiusRange.max, 0.01f, emitter->radiusRange.min);
    ImGui::Text("------ Direction ------");
    if(ImGui::DragFloat3("baseDirection", &emitter->baseDirection.x, 0.01f)){
        emitter->baseDirection = MyMath::Normalize(emitter->baseDirection);
    };
    ImGui::DragFloat("directionRange", &emitter->directionRange, 0.01f, 0.0f, 1.0f);
    ImGui::Text("------- Speed -------");
    ImGui::DragFloat("speedRange.min", &emitter->speedRange.min, 0.02f, 0.0f, emitter->speedRange.max);
    ImGui::DragFloat("speedRange.max", &emitter->speedRange.max, 0.02f, emitter->speedRange.min);
    ImGui::Text("------ LifeTime ------");
    ImGui::DragFloat("lifeTimeRange.min", &emitter->lifeTimeRange.min, 0.05f, 0.0f, emitter->lifeTimeRange.max);
    ImGui::DragFloat("lifeTimeRange.max", &emitter->lifeTimeRange.max, 0.05f, emitter->lifeTimeRange.min);

    // 回転が適用されている場合のみ
    if(emitter->isUseRotate && !emitter->isBillboard){
        ImGui::Text("------- Rotate -------");
        ImGui::DragFloat("rotateSpeedRange.min", &emitter->rotateSpeedRange.min, 0.01f, 0.0f, emitter->rotateSpeedRange.max);
        ImGui::DragFloat("rotateSpeedRange.max", &emitter->rotateSpeedRange.max, 0.01f, emitter->rotateSpeedRange.min);
    }

    // 重力が適用されている場合のみ
    if(emitter->isUseGravity){
        ImGui::Text("------- Gravity -------");
        ImGui::DragFloat("gravity", &emitter->gravity, 0.01f);
    }
}


/*------------------------*/
/*     マテリアルなどの情報  */
/*------------------------*/
void ParticleManager::EditMaterial(Emitter* emitter){
    // 色の設定
    ImGui::Text("-------- Colors --------");
    if(ImGui::CollapsingHeader("ColorList")){
        ImGui::Indent();

        // 色のリスト
        for(int32_t i = 0; i < (int)emitter->colors.size(); ++i){
            std::string colorName = "color_" + std::to_string(i);
            ImGui::ColorEdit4(colorName.c_str(), &emitter->colors[i].x);
            // 削除ボタン
            if(ImGui::Button("DeleteColor")){
                if(emitter->colors.size() > 1){
                    emitter->colors.erase(emitter->colors.begin() + i);
                }
            }
        }

        // 追加ボタン
        ImGui::Text("-- Add --");
        if(ImGui::Button("AddColor")){
            emitter->colors.emplace_back(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
        }

        ImGui::Unindent();
    }

    // テクスチャの設定
    ImGui::Text("-------- Textures --------");
    if(ImGui::CollapsingHeader("TextureList")){
        ImGui::Indent();

        // エミッターに追加されたテクスチャのリスト
        if(ImGui::CollapsingHeader("EmitTextures")){
            ImGui::Indent();

            // 画像の一覧から選択したものをエミッターのテクスチャリストに追加
            for(int32_t i = 0; i < emitter->texturePaths.size(); i++){
                if(ImGui::ImageButton(textureIDs_[emitter->texturePaths[i]], ImVec2(50, 50))){
                    // 消す
                    if(emitter->texturePaths.size() > 1){
                        emitter->texturePaths.erase(
                            std::remove(emitter->texturePaths.begin(), emitter->texturePaths.end(), emitter->texturePaths[i]),
                            emitter->texturePaths.end()
                        );
                    }
                }
            }

            ImGui::Unindent();
        }

        // すべてのテクスチャのリスト
        if(ImGui::CollapsingHeader("Liblaly")){
            ImGui::Indent();

            // 画像の一覧から選択したものをエミッターのテクスチャリストに追加
            for(auto& texture : textureIDs_){
                if(ImGui::ImageButton(texture.second, ImVec2(50, 50))){
                    emitter->texturePaths.push_back(texture.first);
                }
            }

            ImGui::Unindent();
        }

        ImGui::Unindent();
    }
}


/*------------------------*/
/*      頻度などの情報      */
/*------------------------*/
void ParticleManager::EditFrequency(Emitter* emitter){
    ImGui::DragFloat("emitInterval", &emitter->interval, 0.01f, 0.0f);
    ImGui::DragInt("numEmitEvery", &emitter->numEmitEvery, 1, 0, 100);
    if(emitter->emitType == EmitType::kCustom){
        ImGui::DragInt("kMaxEmitCount", &emitter->kMaxEmitCount, 1);
    }
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*                                                                                                               */
/*                                                  ファイル入出力                                                 */
/*                                                                                                               */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// jsonファイルに保存
/// </summary>
void ParticleManager::OutputToJson(const EmitterGroup& emitterGroup, const std::string& outputFileName){
    nlohmann::json j;

    // エミッターの情報を保存
    for(auto& emitter : emitterGroup.emitters){
        nlohmann::json emitterJson;

        // 全般の情報
        emitterJson["isActive"] = emitter.isActive;
        emitterJson["isBillboard"] = emitter.isBillboard;
        emitterJson["isUseRotate"] = emitter.isUseRotate;
        emitterJson["isUseGravity"] = emitter.isUseGravity;
        emitterJson["emitType"] = (int)emitter.emitType;
        emitterJson["particleType"] = (int)emitter.particleType;
        emitterJson["blendMode"] = (int)emitter.blendMode;
        emitterJson["CullingMode"] = (int)emitter.cullingMode;
        emitterJson["center"] = { emitter.center.x, emitter.center.y, emitter.center.z };

        // 範囲などの情報
        emitterJson["emitRange"] = { emitter.emitRange.x, emitter.emitRange.y, emitter.emitRange.z };
        emitterJson["radiusRange"] = { emitter.radiusRange.min, emitter.radiusRange.max };
        emitterJson["baseDirection"] = { emitter.baseDirection.x, emitter.baseDirection.y, emitter.baseDirection.z };
        emitterJson["directionRange"] = emitter.directionRange;
        emitterJson["speedRange"] = { emitter.speedRange.min, emitter.speedRange.max };
        emitterJson["rotateSpeedRange"] = { emitter.rotateSpeedRange.min, emitter.rotateSpeedRange.max };
        emitterJson["lifeTimeRange"] = { emitter.lifeTimeRange.min, emitter.lifeTimeRange.max };
        emitterJson["gravity"] = emitter.gravity;

        // 発生頻度などの情報
        emitterJson["interval"] = emitter.interval;
        emitterJson["numEmitEvery"] = emitter.numEmitEvery;
        emitterJson["kMaxEmitCount"] = emitter.kMaxEmitCount;

        // 色の情報
        for(auto& color : emitter.colors){
            emitterJson["colors"].push_back({ color.x, color.y, color.z, color.w });
        }

        // テクスチャの情報
        for(auto& textureHandle : emitter.texturePaths){
            emitterJson["textureHandles"].push_back(textureHandle);
        }

        j["emitters"].push_back(emitterJson);
    }

    // ファイルに保存
    std::ofstream ofs("resources/jsons/particle/" + outputFileName + ".json");
    ofs << j.dump(4);
}


/// <summary>
/// EmitterGroupをJsonに保存するUIなどを表示
/// </summary>
/// <param name="emitterGroup"></param>
void ParticleManager::OutputEmitterGroup(const EmitterGroup& emitterGroup){

    if(ImGui::BeginPopupModal("OutputToJson")){
        ImGui::Text("Please fileName in below");
        ImGui::InputText(".json", outputFileName_, IM_ARRAYSIZE(outputFileName_));

        if(ImGui::Button("OK")){
            // ファイルに保存
            OutputToJson(emitterGroup, outputFileName_);
            // 成功した旨を表示
            MessageBoxA(nullptr, "Json is Saved", "SaveToJson", MB_OK);
            // ポップアップを閉じる
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

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
        Emitter emitter;

        // 全般の情報
        emitter.isActive = emitterJson["isActive"];
        emitter.isBillboard = emitterJson["isBillboard"];
        emitter.isUseRotate = emitterJson["isUseRotate"];
        emitter.isUseGravity = emitterJson["isUseGravity"];
        emitter.emitType = (EmitType)emitterJson["emitType"];
        emitter.particleType = (ParticleType)emitterJson["particleType"];
        emitter.blendMode = (BlendMode)emitterJson["blendMode"];
        emitter.cullingMode = emitterJson["CullingMode"];
        emitter.center = Vector3(
            emitterJson["center"][0], emitterJson["center"][1], emitterJson["center"][2]
        );

        // 範囲などの情報
        emitter.emitRange = Vector3(
            emitterJson["emitRange"][0], emitterJson["emitRange"][1], emitterJson["emitRange"][2]
        );
        emitter.radiusRange = Range1D(
            emitterJson["radiusRange"][0], emitterJson["radiusRange"][1]
        );
        emitter.baseDirection = Vector3(
            emitterJson["baseDirection"][0], emitterJson["baseDirection"][1], emitterJson["baseDirection"][2]
        );
        emitter.directionRange = emitterJson["directionRange"];
        emitter.speedRange = Range1D(
            emitterJson["speedRange"][0], emitterJson["speedRange"][1]
        );
        emitter.rotateSpeedRange = Range1D(
            emitterJson["rotateSpeedRange"][0], emitterJson["rotateSpeedRange"][1]
        );
        emitter.lifeTimeRange = Range1D(
            emitterJson["lifeTimeRange"][0], emitterJson["lifeTimeRange"][1]
        );
        emitter.gravity = emitterJson["gravity"];


        // 発生頻度などの情報
        emitter.interval = emitterJson["interval"];
        emitter.numEmitEvery = emitterJson["numEmitEvery"];
        emitter.kMaxEmitCount = emitterJson["kMaxEmitCount"];

        // 色の情報
        emitter.colors.clear();
        for(auto& color : emitterJson["colors"]){
            emitter.colors.emplace_back(Vector4(color[0], color[1], color[2], color[3]));
        }

        // テクスチャの情報
        emitter.texturePaths.clear();
        for(auto& textureHandle : emitterJson["textureHandles"]){
            emitter.texturePaths.push_back(textureHandle);
        }

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


