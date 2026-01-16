#pragma once
#include <memory>
#include <SEED/Source/Basic/SceneTransition/ISceneTransition.h>

namespace SEED{
    /// <summary>
    /// シーン遷移描画クラス
    /// </summary>
    class SceneTransitionDrawer{
    public:
        SceneTransitionDrawer() = default;
        ~SceneTransitionDrawer() = default;
        static void Update();
        static void Draw();

        template<typename T>
        static T* AddTransition();

    private:
        static inline SceneTransitionDrawer* instance_ = nullptr;
        std::unique_ptr<ISceneTransition> transition_;  // どんな型でも入れられる
    };


    template<typename T>
    inline T* SceneTransitionDrawer::AddTransition(){

        if(!instance_){
            instance_ = new SceneTransitionDrawer();
        }

        if constexpr(!std::is_base_of_v<ISceneTransition, T>){
            // TがISceneTransitionを継承していない
            return nullptr;
        } else{
            instance_->transition_.reset();               // 既存の解放
            instance_->transition_ = std::make_unique<T>(); // 新規作成
            return static_cast<T*>(instance_->transition_.get());
        }
    }
}