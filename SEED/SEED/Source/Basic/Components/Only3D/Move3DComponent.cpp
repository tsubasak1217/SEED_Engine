#include "Move3DComponent.h"
#include <SEED/Source/Basic/Object/GameObject.h>
#include <SEED/Source/SEED.h>

namespace SEED{
    /////////////////////////////////////////////////////////////////////////
    // コンストラクタ
    /////////////////////////////////////////////////////////////////////////
    Move3DComponent::Move3DComponent(GameObject* pOwner, const std::string& tagName)
        : IComponent(pOwner, tagName){
        if(tagName == ""){
            componentTag_ = "Move3D_ID:" + std::to_string(componentID_);
        }

        // 2Dは受け付けない
        if(owner_.is2D or !owner_.owner3D){
            assert(false);
        }

        // 入力を受け付けるボタン配列の初期化(keyboard)
        frontKeys_.empty() ? frontKeys_ = { DIK_W,DIK_UP } : frontKeys_;
        backKeys_.empty() ? backKeys_ = { DIK_S,DIK_DOWN } : backKeys_;
        leftKeys_.empty() ? leftKeys_ = { DIK_A,DIK_LEFT } : leftKeys_;
        rightKeys_.empty() ? rightKeys_ = { DIK_D,DIK_RIGHT } : rightKeys_;

        // 入力を受け付けるボタン配列の初期化(pad)
        frontPadButtons_.empty() ? frontPadButtons_ = { PAD_BUTTON::UP, PAD_BUTTON::L_STICK } : frontPadButtons_;
        backPadButtons_.empty() ? backPadButtons_ = { PAD_BUTTON::DOWN, PAD_BUTTON::L_STICK } : backPadButtons_;
        leftPadButtons_.empty() ? leftPadButtons_ = { PAD_BUTTON::LEFT, PAD_BUTTON::L_STICK } : leftPadButtons_;
        rightPadButtons_.empty() ? rightPadButtons_ = { PAD_BUTTON::RIGHT, PAD_BUTTON::L_STICK } : rightPadButtons_;
    }

    /////////////////////////////////////////////////////////////////////////
    // 初期化処理
    /////////////////////////////////////////////////////////////////////////
    void Move3DComponent::Initialize(){

    }

    /////////////////////////////////////////////////////////////////////////
    // フレーム開始時の処理
    /////////////////////////////////////////////////////////////////////////
    void Move3DComponent::BeginFrame(){


        // 入力状態の取得
        direction_ = Vector3();
        direction_.z = float(Input::IsPressKey(frontKeys_) - Input::IsPressKey(backKeys_));
        direction_.x = float(Input::IsPressKey(rightKeys_) - Input::IsPressKey(leftKeys_));
        direction_.y = float(Input::IsPressKey(upKeys_) - Input::IsPressKey(downKeys_));

        // パッド入力の取得
        for(const PAD_BUTTON& button : frontPadButtons_){

            if(button == PAD_BUTTON::L_STICK){
                // Lスティックの入力を取得
                Vector2 stickValue = Input::GetStickValue(Enums::LR::LEFT);
                stickValue.y > 0.0f ? direction_.z = stickValue.y : direction_.z;

            } else if(button == PAD_BUTTON::R_STICK){
                // Rスティックの入力を取得
                Vector2 stickValue = Input::GetStickValue(Enums::LR::RIGHT);
                stickValue.y > 0.0f ? direction_.z = stickValue.y : direction_.z;

            } else if(button == PAD_BUTTON::LT){
                float triggerValue = Input::GetLRTriggerValue(Enums::LR::LEFT);
                direction_.z = triggerValue;

            } else if(button == PAD_BUTTON::RT){
                float triggerValue = Input::GetLRTriggerValue(Enums::LR::RIGHT);
                direction_.z = triggerValue;

            } else if(Input::IsPressPadButton(button)){
                direction_.z = 1.0f;
            }
        }

        for(const PAD_BUTTON& button : backPadButtons_){
            if(button == PAD_BUTTON::L_STICK){
                // Lスティックの入力を取得
                Vector2 stickValue = Input::GetStickValue(Enums::LR::LEFT);
                stickValue.y < 0.0f ? direction_.z = stickValue.y : direction_.z;

            } else if(button == PAD_BUTTON::R_STICK){
                // Rスティックの入力を取得
                Vector2 stickValue = Input::GetStickValue(Enums::LR::RIGHT);
                stickValue.y < 0.0f ? direction_.z = stickValue.y : direction_.z;

            } else if(button == PAD_BUTTON::LT){
                float triggerValue = Input::GetLRTriggerValue(Enums::LR::LEFT);
                direction_.z = -triggerValue;

            } else if(button == PAD_BUTTON::RT){
                float triggerValue = Input::GetLRTriggerValue(Enums::LR::RIGHT);
                direction_.z = -triggerValue;
            } else if(Input::IsPressPadButton(button)){
                direction_.z = -1.0f;
            }
        }

        for(const PAD_BUTTON& button : leftPadButtons_){
            if(button == PAD_BUTTON::L_STICK){
                // Lスティックの入力を取得
                Vector2 stickValue = Input::GetStickValue(Enums::LR::LEFT);
                stickValue.x < 0.0f ? direction_.x = stickValue.x : direction_.x;

            } else if(button == PAD_BUTTON::R_STICK){
                // Rスティックの入力を取得
                Vector2 stickValue = Input::GetStickValue(Enums::LR::RIGHT);
                stickValue.x < 0.0f ? direction_.x = stickValue.x : direction_.x;

            } else if(button == PAD_BUTTON::LT){
                float triggerValue = Input::GetLRTriggerValue(Enums::LR::LEFT);
                direction_.x = -triggerValue;

            } else if(button == PAD_BUTTON::RT){
                float triggerValue = Input::GetLRTriggerValue(Enums::LR::RIGHT);
                direction_.x = -triggerValue;

            } else if(Input::IsPressPadButton(button)){
                direction_.x = -1.0f;
            }
        }

        for(const PAD_BUTTON& button : rightPadButtons_){
            if(button == PAD_BUTTON::L_STICK){
                // Lスティックの入力を取得
                Vector2 stickValue = Input::GetStickValue(Enums::LR::LEFT);
                stickValue.x > 0.0f ? direction_.x = stickValue.x : direction_.x;

            } else if(button == PAD_BUTTON::R_STICK){
                // Rスティックの入力を取得
                Vector2 stickValue = Input::GetStickValue(Enums::LR::RIGHT);
                stickValue.x > 0.0f ? direction_.x = stickValue.x : direction_.x;

            } else if(button == PAD_BUTTON::LT){
                float triggerValue = Input::GetLRTriggerValue(Enums::LR::LEFT);
                direction_.x = triggerValue;

            } else if(button == PAD_BUTTON::RT){
                float triggerValue = Input::GetLRTriggerValue(Enums::LR::RIGHT);
                direction_.x = triggerValue;

            } else if(Input::IsPressPadButton(button)){
                direction_.x = 1.0f;
            }
        }

        for(const PAD_BUTTON& button : upPadButtons_){
            if(button == PAD_BUTTON::LT){
                float triggerValue = Input::GetLRTriggerValue(Enums::LR::LEFT);
                direction_.y = triggerValue;
            } else if(button == PAD_BUTTON::RT){
                float triggerValue = Input::GetLRTriggerValue(Enums::LR::RIGHT);
                direction_.y = triggerValue;
            } else if(Input::IsPressPadButton(button)){
                direction_.y = 1.0f;
            }
        }

        for(const PAD_BUTTON& button : downPadButtons_){
            if(button == PAD_BUTTON::LT){
                float triggerValue = Input::GetLRTriggerValue(Enums::LR::LEFT);
                direction_.y = -triggerValue;
            } else if(button == PAD_BUTTON::RT){
                float triggerValue = Input::GetLRTriggerValue(Enums::LR::RIGHT);
                direction_.y = -triggerValue;
            } else if(Input::IsPressPadButton(button)){
                direction_.y = -1.0f;
            }
        }

        // 移動方向の正規化
        direction_.Normalize();

    }

    /////////////////////////////////////////////////////////////////////////
    // 更新処理
    /////////////////////////////////////////////////////////////////////////
    void Move3DComponent::Update(){

        // 移動中かどうかの判定
        isMoving_ = (direction_.x != 0.0f || direction_.y != 0.0f || direction_.z != 0.0f);

        // 速度ベクトルの計算
        velocity_ = direction_ * speed_ * ClockManager::DeltaTime();

        // mainCameraのY回転に合わせて移動方向を調整
        auto* camera = SEED::Instance::GetMainCamera();
        Vector3 cameraRotate = camera->GetEulerRotation();
        cameraRotate.x = 0.0f;
        cameraRotate.z = 0.0f;
        velocity_ *= Methods::Matrix::RotateMatrix(cameraRotate);


        // 移動方向に合わせて回転
        if(isRotateByDirection_ && isMoving_){
            Vector3 eulerRotate = Methods::SRT::CalcRotateFromVec(velocity_);
            isUseRotateX ? eulerRotate.x : eulerRotate.x = 0.0f;
            Quaternion rotation = Quaternion::ToQuaternion(eulerRotate);
            owner_.owner3D->SetWorldRotate(rotation);
        }

        // オーナーの位置を更新
        owner_.owner3D->AddWorldTranslate(velocity_);
    }


    /////////////////////////////////////////////////////////////////////////
    // 描画処理
    /////////////////////////////////////////////////////////////////////////
    void Move3DComponent::Draw(){
    }


    /////////////////////////////////////////////////////////////////////////
    // コンストラクタ
    /////////////////////////////////////////////////////////////////////////
    void Move3DComponent::EndFrame(){

    }


    /////////////////////////////////////////////////////////////////////////
    // フレーム終了時の処理
    /////////////////////////////////////////////////////////////////////////
    void Move3DComponent::Finalize(){
    }


    /////////////////////////////////////////////////////////////////////////
    // GUI上での編集処理
    /////////////////////////////////////////////////////////////////////////
    void Move3DComponent::EditGUI(){
    #ifdef _DEBUG
        ImGui::Indent();
        ImGui::DragFloat("移動速度", &speed_, 0.1f);
        ImGui::Checkbox("移動方向に合わせて回転", &isRotateByDirection_);
        if(isRotateByDirection_){
            ImGui::Checkbox("X軸の回転を使用", &isUseRotateX);
        }
        ImGui::Unindent();

    #endif // _DEBUG
    }


    /////////////////////////////////////////////////////////////////////////
    // jsonへの書き出し
    /////////////////////////////////////////////////////////////////////////
    nlohmann::json Move3DComponent::GetJsonData() const{
        nlohmann::json jsonData;
        jsonData["componentType"] = "Move3D";
        jsonData.update(IComponent::GetJsonData());
        jsonData["speed"] = speed_;
        jsonData["isRotateByDirection"] = isRotateByDirection_;
        jsonData["isUseRotateX"] = isUseRotateX;

        for(const auto& key : frontKeys_){
            jsonData["frontKeys"].push_back(key);
        }
        for(const auto& key : backKeys_){
            jsonData["backKeys"].push_back(key);
        }
        for(const auto& key : leftKeys_){
            jsonData["leftKeys"].push_back(key);
        }
        for(const auto& key : rightKeys_){
            jsonData["rightKeys"].push_back(key);
        }
        for(const auto& key : upKeys_){
            jsonData["upKeys"].push_back(key);
        }
        for(const auto& key : downKeys_){
            jsonData["downKeys"].push_back(key);
        }
        for(const auto& button : frontPadButtons_){
            jsonData["frontPadButtons"].push_back(static_cast<int>(button));
        }
        for(const auto& button : backPadButtons_){
            jsonData["backPadButtons"].push_back(static_cast<int>(button));
        }
        for(const auto& button : leftPadButtons_){
            jsonData["leftPadButtons"].push_back(static_cast<int>(button));
        }
        for(const auto& button : rightPadButtons_){
            jsonData["rightPadButtons"].push_back(static_cast<int>(button));
        }
        for(const auto& button : upPadButtons_){
            jsonData["upPadButtons"].push_back(static_cast<int>(button));
        }
        for(const auto& button : downPadButtons_){
            jsonData["downPadButtons"].push_back(static_cast<int>(button));
        }

        return jsonData;
    }


    /////////////////////////////////////////////////////////////////////////
    // jsonからの読み込み処理
    /////////////////////////////////////////////////////////////////////////
    void Move3DComponent::LoadFromJson(const nlohmann::json& jsonData){

        IComponent::LoadFromJson(jsonData);
        speed_ = jsonData.value("speed", speed_);
        isRotateByDirection_ = jsonData.value("isRotateByDirection", isRotateByDirection_);
        isUseRotateX = jsonData.value("isUseRotateX", isUseRotateX);

        // キーボードのキー配列を読み込み
        frontKeys_.clear();
        if(jsonData.contains("frontKeys")){
            for(const auto& key : jsonData["frontKeys"]){
                frontKeys_.emplace_back(static_cast<uint8_t>(key));
            }
        }
        backKeys_.clear();
        if(jsonData.contains("backKeys")){
            for(const auto& key : jsonData["backKeys"]){
                backKeys_.emplace_back(static_cast<uint8_t>(key));
            }
        }
        leftKeys_.clear();
        if(jsonData.contains("leftKeys")){
            for(const auto& key : jsonData["leftKeys"]){
                leftKeys_.emplace_back(static_cast<uint8_t>(key));
            }
        }
        rightKeys_.clear();
        if(jsonData.contains("rightKeys")){
            for(const auto& key : jsonData["rightKeys"]){
                rightKeys_.emplace_back(static_cast<uint8_t>(key));
            }
        }
        upKeys_.clear();
        if(jsonData.contains("upKeys")){
            for(const auto& key : jsonData["upKeys"]){
                upKeys_.emplace_back(static_cast<uint8_t>(key));
            }
        }
        downKeys_.clear();
        if(jsonData.contains("downKeys")){
            for(const auto& key : jsonData["downKeys"]){
                downKeys_.emplace_back(static_cast<uint8_t>(key));
            }
        }

        // パッドのボタン配列を読み込み
        frontPadButtons_.clear();
        if(jsonData.contains("frontPadButtons")){
            for(const auto& button : jsonData["frontPadButtons"]){
                frontPadButtons_.emplace_back(static_cast<PAD_BUTTON>(button));
            }
        }
        backPadButtons_.clear();
        if(jsonData.contains("backPadButtons")){
            for(const auto& button : jsonData["backPadButtons"]){
                backPadButtons_.emplace_back(static_cast<PAD_BUTTON>(button));
            }
        }
        leftPadButtons_.clear();
        if(jsonData.contains("leftPadButtons")){
            for(const auto& button : jsonData["leftPadButtons"]){
                leftPadButtons_.emplace_back(static_cast<PAD_BUTTON>(button));
            }
        }
        rightPadButtons_.clear();
        if(jsonData.contains("rightPadButtons")){
            for(const auto& button : jsonData["rightPadButtons"]){
                rightPadButtons_.emplace_back(static_cast<PAD_BUTTON>(button));
            }
        }
        upPadButtons_.clear();
        if(jsonData.contains("upPadButtons")){
            for(const auto& button : jsonData["upPadButtons"]){
                upPadButtons_.emplace_back(static_cast<PAD_BUTTON>(button));
            }
        }
        downPadButtons_.clear();
        if(jsonData.contains("downPadButtons")){
            for(const auto& button : jsonData["downPadButtons"]){
                downPadButtons_.emplace_back(static_cast<PAD_BUTTON>(button));
            }
        }
    }
}