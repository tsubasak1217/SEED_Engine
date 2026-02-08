#include "JumpComponent.h"
#include <SEED/Source/Basic/Object/GameObject.h>
#include <SEED/Source/SEED.h>

namespace SEED{
    /////////////////////////////////////////////////////////////////////////
    // コンストラクタ
    /////////////////////////////////////////////////////////////////////////
    JumpComponent::JumpComponent(std::variant<GameObject*, GameObject2D*> pOwner, const std::string& tagName)
        : IComponent(pOwner, tagName){
        if(tagName == ""){
            componentTag_ = "Jump_ID:" + std::to_string(componentID_);
        }

        // 入力を受け付けるボタン配列の初期化(keyboard)
        jumpKeys_.empty() ? jumpKeys_ = { DIK_SPACE } : jumpKeys_;

        // 入力を受け付けるボタン配列の初期化(pad)
        jumpPadButtons_.empty() ? jumpPadButtons_ = { PAD_BUTTON::A } : jumpPadButtons_;
    }

    /////////////////////////////////////////////////////////////////////////
    // 初期化処理
    /////////////////////////////////////////////////////////////////////////
    void JumpComponent::Initialize(){

    }

    /////////////////////////////////////////////////////////////////////////
    // フレーム開始時の処理
    /////////////////////////////////////////////////////////////////////////
    void JumpComponent::BeginFrame(){

        if(owner_.is2D == false){
            if(owner_.owner3D->GetIsOnGrounnd()){

                if(!isJumping_){
                    if(Input::IsPressKey(jumpKeys_)){
                        isJumping_ = true;
                        owner_.owner3D->SetIsOnGround(false); // ジャンプ開始時に地面から離れる
                    }

                    for(const PAD_BUTTON& buttton : jumpPadButtons_){
                        if(Input::IsPressPadButton(buttton)){
                            isJumping_ = true;
                            owner_.owner3D->SetIsOnGround(false); // ジャンプ開始時に地面から離れる
                            break;
                        }
                    }
                }
            }
        } else{
            // 2Dジャンプ処理(未実装)
        }
    }

    /////////////////////////////////////////////////////////////////////////
    // 更新処理
    /////////////////////////////////////////////////////////////////////////
    void JumpComponent::Update(){

        if(isJumping_){
            if(owner_.is2D == false){
                if(!owner_.owner3D->GetIsOnGrounnd()){
                    // ジャンプ中の処理
                    Vector3 velocity = Vector3(0.0f, jumpSpeed_, 0.0f) * ClockManager::DeltaTime();
                    owner_.owner3D->AddWorldTranslate(velocity);
                } else{
                    // ジャンプが終了したらフラグをリセット
                    isJumping_ = false;
                }
            } else{
                // 2Dジャンプ処理(未実装)
            }
        }
    }


    /////////////////////////////////////////////////////////////////////////
    // 描画処理
    /////////////////////////////////////////////////////////////////////////
    void JumpComponent::Draw(){
    }


    /////////////////////////////////////////////////////////////////////////
    // コンストラクタ
    /////////////////////////////////////////////////////////////////////////
    void JumpComponent::EndFrame(){

    }


    /////////////////////////////////////////////////////////////////////////
    // フレーム終了時の処理
    /////////////////////////////////////////////////////////////////////////
    void JumpComponent::Finalize(){
    }


    /////////////////////////////////////////////////////////////////////////
    // GUI上での編集処理
    /////////////////////////////////////////////////////////////////////////
    void JumpComponent::EditGUI(){
    #ifdef _DEBUG
        ImGui::Indent();
        ImGui::DragFloat("ジャンプ速度", &jumpSpeed_, 0.1f);
        ImGui::Unindent();

    #endif // _DEBUG
    }


    /////////////////////////////////////////////////////////////////////////
    // jsonへの書き出し
    /////////////////////////////////////////////////////////////////////////
    nlohmann::json JumpComponent::GetJsonData() const{
        nlohmann::json jsonData;
        jsonData["componentType"] = "Jump";
        jsonData.update(IComponent::GetJsonData());

        return jsonData;
    }


    /////////////////////////////////////////////////////////////////////////
    // jsonからの読み込み処理
    /////////////////////////////////////////////////////////////////////////
    void JumpComponent::LoadFromJson(const nlohmann::json& jsonData){

        IComponent::LoadFromJson(jsonData);
        jumpSpeed_ = jsonData.value("jumpSpeed", jumpSpeed_);

    }
}