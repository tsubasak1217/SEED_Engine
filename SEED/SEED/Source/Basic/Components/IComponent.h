#pragma once 
#include <string>
#include <json.hpp>
#include <variant>
#include <cstdint>
#include <SEED/Source/Manager/ImGuiManager/ImGuiManager.h>


namespace SEED{

    class GameObject; // GameObjectからのみアクセス可能にする
    class GameObject2D;

    /// <summary>
        /// コンポーネントの所有者情報を格納する構造体
        /// </summary>
    struct ComponentOwner{
        bool is2D = false; // 2Dか3Dか
        GameObject* owner3D = nullptr; // GameObject3D*
        GameObject2D* owner2D = nullptr; // GameObject2D*
    };


    /// <summary>
    // コンポーネントのインターフェース
    /// </summary>
    class IComponent{
        friend GameObject; // GameObjectからのみアクセス可能にする
        friend GameObject2D;
    protected:
        ComponentOwner owner_; // コンポーネントの所有者(2D or 3D)
        std::string componentTag_; // コンポーネントのタグ
        inline static uint32_t nextComponentID_ = 0; // コンポーネントのID
        uint32_t componentID_ = 0; // コンポーネントのインスタンスID
        bool isActive_ = true; // コンポーネントが有効かどうか
    public:
        IComponent() = default;
        IComponent(std::variant<GameObject*, GameObject2D*> pOwner, const std::string& tagName = "");
        virtual ~IComponent() = default;
        // コンポーネントの初期化
        virtual void Initialize(){};
        // フレームの開始時処理
        virtual void BeginFrame(){};
        // コンポーネントの更新
        virtual void Update() = 0;
        // コンポーネントの描画
        virtual void Draw(){};
        // フレームの終了時処理
        virtual void EndFrame(){};
        // コンポーネントの終了処理
        virtual void Finalize(){};

    public:
    #pragma warning(push)// 引数未使用警告を無視
    #pragma warning(disable:4100)
        // 当たり判定が発生したときの処理
        virtual void OnCollisionEnter(GameObject* other){};
        virtual void OnCollisionEnter(GameObject2D* other){};
        virtual void OnCollisionStay(GameObject* other){};
        virtual void OnCollisionStay(GameObject2D* other){};
        virtual void OnCollisionExit(GameObject* other){};
        virtual void OnCollisionExit(GameObject2D* other){};
    #pragma warning(pop)

        // GUIで編集を行う関数
        virtual void EditGUI(){};

        // json出力
        virtual nlohmann::json GetJsonData() const;
        virtual void LoadFromJson(const nlohmann::json& jsonData);

    public:
        // アクセッサ
        ComponentOwner GetOwner() const{ return owner_; }
        const std::string& GetTagName() const{ return componentTag_; }
        void SetTagName(const std::string& tagName){ componentTag_ = tagName; }
        void SetIsActive(bool isActive){ isActive_ = isActive; }
    };
}