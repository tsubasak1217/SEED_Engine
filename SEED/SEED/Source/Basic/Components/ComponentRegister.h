#pragma once
#include <type_traits>
#include <iterator>
#include <utility>
#include <concepts>
#include <json.hpp>
#include <SEED/Source/Basic/Components/ComponentDictionary.h>
#include <SEED/Source/Manager/ImGuiManager/ImGuiManager.h>
#include <SEED/Source/Basic/Object/GameObject.h>
#include <SEED/Source/Basic/Object/GameObject2D.h>

// コンセプトの作成
template <typename T>
concept ComponentsOwner = std::is_base_of_v<GameObject, T> || std::is_base_of_v<GameObject2D, T>;

///////////////////////////////////////////////////////////////////////////////////////////////
// オブジェクトにコンポーネントを登録するやつ
///////////////////////////////////////////////////////////////////////////////////////////////
namespace ComponentRegister{
    // オブジェクトにコンポーネントを登録する関数
    template <ComponentsOwner T>
    bool RegisterGUI(T* owner);
    // jsonからコンポーネントを読み込む関数
    template <ComponentsOwner T>
    void LoadComponents(T* owner, const nlohmann::json& json);
};

///////////////////////////////////////////////////////////////////////////////////////////////
// オブジェクトにコンポーネントを登録する関数
///////////////////////////////////////////////////////////////////////////////////////////////
template<ComponentsOwner T>
inline bool ComponentRegister::RegisterGUI(T* owner){
#ifdef _DEBUG
    // 2D、3Dに共通のもの
    if(ImGui::CollapsingHeader("共通")){
        ImGui::Indent();

        if(ImGui::Button("TextComponent / テキスト描画")){
            owner->AddComponent<TextComponent>();
            return true;
        }
        if(ImGui::Button("JumpComponent / ジャンプ")){
            owner->AddComponent<JumpComponent>();
            return true;
        }
        if(ImGui::Button("ColorControlComponent / 色制御")){
            owner->AddComponent<ColorControlComponent>();
            return true;
        }

        ImGui::Unindent();
    }

    // 2D専用のもの
    if constexpr(std::is_base_of_v<GameObject2D, T>){
        if(ImGui::CollapsingHeader("2D専用")){
            ImGui::Indent();
            if(ImGui::Button("CollisionComponent / 衝突判定・押し戻し")){
                owner->AddComponent<Collision2DComponent>();
                return true;
            }
            if(ImGui::Button("UIComponent / UI描画")){
                owner->AddComponent<UIComponent>();
                return true;
            }
            if(ImGui::Button("Routine2DComponent / 2Dルーチン")){
                owner->AddComponent<Routine2DComponent>();
                return true;
            }
            ImGui::Unindent();
        }
    }

    // 3D専用のもの
    if constexpr(std::is_base_of_v<GameObject, T>){
        if(ImGui::CollapsingHeader("3D専用")){
            ImGui::Indent();
            if(ImGui::Button("ModelRenderComponent / モデル描画")){
                owner->AddComponent<ModelRenderComponent>();
                return true;
            }
            if(ImGui::Button("CollisionComponent / 衝突判定・押し戻し")){
                owner->AddComponent<Collision3DComponent>();
                return true;
            }
            if(ImGui::Button("SpotLightComponent / スポットライト")){
                owner->AddComponent<SpotLightComponent>();
                return true;
            }
            if(ImGui::Button("GravityComponent / 重力")){
                owner->AddComponent<Gravity3DComponent>();
                return true;
            }
            if(ImGui::Button("MoveComponent / 移動")){
                owner->AddComponent<Move3DComponent>();
                return true;
            }
            if(ImGui::Button("Routine3DComponent / 3Dルーチン")){
                owner->AddComponent<Routine3DComponent>();
                return true;
            }
            ImGui::Unindent();
        }
    }

#endif // _DEBUG
    return false;
}


///////////////////////////////////////////////////////////////////////////////////////////////
// jsonからコンポーネントを読み込む関数
///////////////////////////////////////////////////////////////////////////////////////////////
template<ComponentsOwner T>
inline void ComponentRegister::LoadComponents(T* owner, const nlohmann::json& json){
    std::string componentType = json["componentType"];

    // 共通===================================================================
    {
        if(componentType == "Text"){
            auto* textComponent = owner->AddComponent<TextComponent>();
            textComponent->LoadFromJson(json);

        } else if(componentType == "Jump"){
            auto* jumpComponent = owner->AddComponent<JumpComponent>();
            jumpComponent->LoadFromJson(json);

        } else if(componentType == "ColorControl"){
            auto* colorControlComponent = owner->AddComponent<ColorControlComponent>();
            colorControlComponent->LoadFromJson(json);
        }
    }

    // 2D専用のもの============================================================
    if constexpr(std::is_base_of_v<GameObject2D, T>){
        if(componentType == "Collision2D"){
            auto* collisionComponent = owner->AddComponent<Collision2DComponent>();
            collisionComponent->LoadFromJson(json);

        } else if(componentType == "UI"){
            auto* uiComponent = owner->AddComponent<UIComponent>();
            uiComponent->LoadFromJson(json);

        } else if(componentType == "Routine2D"){
            auto* uiComponent = owner->AddComponent<Routine2DComponent>();
            uiComponent->LoadFromJson(json);
        }
    }


    // 3D専用============================================================
    if constexpr(std::is_base_of_v<GameObject, T>){
        if(componentType == "ModelRender"){
            auto* modelComponent = owner->AddComponent<ModelRenderComponent>();
            modelComponent->LoadFromJson(json);

        } else if(componentType == "Collision3D"){
            auto* collisionComponent = owner->AddComponent<Collision3DComponent>();
            collisionComponent->LoadFromJson(json);

        } else if(componentType == "SpotLight"){
            auto* spotLightComponent = owner->AddComponent<SpotLightComponent>();
            spotLightComponent->LoadFromJson(json);

        } else if(componentType == "Gravity"){
            auto* gravityComponent = owner->AddComponent<Gravity3DComponent>();
            gravityComponent->LoadFromJson(json);

        } else if(componentType == "Move"){
            auto* moveComponent = owner->AddComponent<Move3DComponent>();
            moveComponent->LoadFromJson(json);

        } else if(componentType == "Routine3D"){
            auto* routineComponent = owner->AddComponent<Routine3DComponent>();
            routineComponent->LoadFromJson(json);
        }
    }


}
