#pragma once 
#include <string>

// コンポーネントのインターフェース
class IComponent{
    friend class GameObject; // GameObjectからのみアクセス可能にする
protected:
    GameObject* owner_ = nullptr; // コンポーネントの所有者
    std::string componentTag_; // コンポーネントのタグ
public:
    IComponent() = default;
    IComponent(GameObject* pOwner,const std::string& tagName);
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

#pragma warning(push)// 引数未使用警告を無視
#pragma warning(disable:4100)
    // 当たり判定が発生したときの処理
    virtual void OnCollisionEnter(GameObject* other){};
    virtual void OnCollisionStay(GameObject* other){};
    virtual void OnCollisionExit(GameObject* other){};
#pragma warning(pop)

public:
    // アクセッサ
    GameObject* GetOwner() const{ return owner_; }
    const std::string& GetTagName() const{ return componentTag_; }
    void SetTagName(const std::string& tagName){ componentTag_ = tagName; }
};