#pragma once

#include <memory>

#include <string>

class Model;

struct Vector3;
struct Quaternion;

class GameObject{
public:
    GameObject();
    GameObject(const std::string& _name);
    virtual ~GameObject();

    virtual void Initialize() = 0;
    virtual void Update() = 0;
    void Draw();

protected:
    std::unique_ptr<Model> model_;
private:
    std::string name_ = "";
public:
    const std::string& GetName()const{ return name_; }
    void SetName(const std::string& _name){ name_ = _name.c_str(); }

    ////////////////////////////////////////////////////
    //  model_ Transform の アクセッサー
    ////////////////////////////////////////////////////
    /// <summary>
    /// Model's Scale
    /// </summary>
    /// <returns></returns>
    const Vector3& GetScale()const;
    /// <summary>
    /// Model's Rotate
    /// </summary>
    /// <returns></returns>
    const Vector3& GetRotate()const;
    /// <summary>
    /// Model's RotateQuat
    /// </summary>
    /// <returns></returns>
    const Quaternion& GetQuaternion()const;
    /// <summary>
    /// Model's Transform
    /// </summary>
    /// <returns></returns>
    const Vector3& GetTranslate()const;
    /// <summary>
    /// Model's WorldPos
    /// </summary>
    /// <returns></returns>
    Vector3 GetWorldPos()const;

    /// <summary>
    /// モデルのスケールを設定
    /// </summary>
    /// <param name="scale">新しいスケール</param>
    void SetScale(const Vector3& scale);
    void SetScaleX(float x);
    void SetScaleY(float y);
    void SetScaleZ(float z);

    /// <summary>
    /// モデルの回転を設定
    /// </summary>
    /// <param name="rotate">新しい回転</param>
    void SetRotate(const Vector3& rotate);
    void SetRotateX(float x);
    void SetRotateY(float y);
    void SetRotateZ(float z);

    /// <summary>
    /// モデルのクォータニオンを設定
    /// </summary>
    /// <param name="quaternion">新しいクォータニオン</param>
    void SetQuaternion(const Quaternion& quaternion);
    void SetQuaternionX(float x);
    void SetQuaternionY(float y);
    void SetQuaternionZ(float z);
    void SetQuaternionW(float w);

    /// <summary>
    /// モデルの位置を設定
    /// </summary>
    /// <param name="translate">新しい位置</param>
    void SetTranslate(const Vector3& translate);
    void SetTranslateX(float x);
    void SetTranslateY(float y);
    void SetTranslateZ(float z);
};