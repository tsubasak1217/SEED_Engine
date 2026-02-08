#pragma once
#include <SEED/Source/Basic/Components/IComponent.h>
#include <SEED/Source/Manager/InputManager/InputManager.h>

namespace SEED{
    /// <summary>
    /// 3次元の簡易的な移動機能を追加するコンポーネント
    /// </summary>
    class Move3DComponent : public IComponent{
    public:
        Move3DComponent(GameObject* pOwner, const std::string& tagName = "");
        ~Move3DComponent() = default;
        void Initialize()override;
        void BeginFrame() override;
        void Update() override;
        void Draw() override;
        void EndFrame() override;
        void Finalize() override;
        // GUI編集
        void EditGUI() override;
        // json
        nlohmann::json GetJsonData() const override;
        void LoadFromJson(const nlohmann::json& jsonData) override;

    private:
        // 入力を受け付けるボタン配列(keyboard)
        std::vector<uint8_t> frontKeys_;
        std::vector<uint8_t> backKeys_;
        std::vector<uint8_t> leftKeys_;
        std::vector<uint8_t> rightKeys_;
        std::vector<uint8_t> upKeys_;
        std::vector<uint8_t> downKeys_;

        // 入力を受け付けるボタン配列(pad)
        std::vector<PAD_BUTTON> frontPadButtons_;
        std::vector<PAD_BUTTON> backPadButtons_;
        std::vector<PAD_BUTTON> leftPadButtons_;
        std::vector<PAD_BUTTON> rightPadButtons_;
        std::vector<PAD_BUTTON> upPadButtons_;
        std::vector<PAD_BUTTON> downPadButtons_;

        // パラメーター
        Vector3 direction_;
        float speed_ = 8.0f; // 移動速度
        Vector3 velocity_; // 速度ベクトル
        bool isMoving_ = false; // 移動中かどうか
        bool isRotateByDirection_ = true; // 移動方向に合わせて回転するかどうか
        bool isUseRotateX = false; // X軸の回転を使用するかどうか
    };
}