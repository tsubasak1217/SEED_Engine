#pragma once
#include <nlohmann/json.hpp>
#include <SEED/Lib/Tensor/Vector2.h>
#include <SEED/Lib/Tensor/Vector3.h>
#include <SEED/Lib/Tensor/Matrix4x4.h>
#include <SEED/Lib/Tensor/Quaternion.h>

namespace SEED{

    /// <summary>
    /// 二次元の拡縮・回転・移動情報を保持する構造体
    /// </summary>
    struct Transform2D{
        Vector2 scale = { 1.0f,1.0f };
        float rotate = 0.0f; // radians
        Vector2 translate;
        // 変換関数
        Matrix3x3 ToMatrix()const;
        Matrix4x4 ToMatrix4x4()const;
        void FromMatrix4x4(const Matrix4x4& mat);

        // 比較演算子
        bool operator==(const Transform2D& other) const{
            return scale == other.scale && rotate == other.rotate && translate == other.translate;
        }

        bool operator!=(const Transform2D& other) const{
            return !(*this == other);
        }
    };

    /// <summary>
    /// 三次元の拡縮・回転・移動情報を保持する構造体
    /// </summary>
    struct Transform{
        Vector3 scale = { 1.0f,1.0f,1.0f };
        Quaternion rotate;
        Vector3 translate;

        // 変換関数
        Matrix4x4 ToMatrix();
        void FromMatrix(const Matrix4x4& mat);

        // 比較演算子
        bool operator==(const Transform& other) const{
            return scale == other.scale && rotate == other.rotate && translate == other.translate;
        }
        bool operator!=(const Transform& other) const{
            return !(*this == other);
        }
    };


    /// <summary>
    /// 頂点変換を行う行列をまとめた構造体
    /// </summary>
    struct TransformMatrix{
        Matrix4x4 WVP;
        Matrix4x4 world;
        Matrix4x4 worldInverseTranspose;
    };


    // jsosnコンバート関数
    inline void to_json(nlohmann::json& j, const Transform2D& transformMatrix){
        j = {
            {"scale",transformMatrix.scale},
            {"rotate",transformMatrix.rotate},
            {"translate",transformMatrix.translate},
        };
    }

    inline void to_json(nlohmann::ordered_json& j, const Transform2D& transform){
        j = {
            {"scale",transform.scale},
            {"rotate",transform.rotate},
            {"translate",transform.translate},
        };
    }

    inline void to_json(nlohmann::json& j, const Transform& transform){
        j = {
            {"scale",transform.scale},
            {"rotate",transform.rotate},
            {"translate",transform.translate},
        };
    }

    inline void to_json(nlohmann::ordered_json& j, const Transform& transform){
        j = {
            {"scale",transform.scale},
            {"rotate",transform.rotate},
            {"translate",transform.translate},
        };
    }

    inline void from_json(const nlohmann::ordered_json& j, Transform2D& transform){
        if(j.contains("scale")){ transform.scale = j["scale"]; }
        if(j.contains("rotate")){ transform.rotate = j["rotate"]; }
        if(j.contains("translate")){ transform.translate = j["translate"]; }
    }

    inline void from_json(const nlohmann::ordered_json& j, Transform& transform){
        if(j.contains("scale")){ transform.scale = j["scale"]; }
        if(j.contains("rotate")){ transform.rotate = j["rotate"]; }
        if(j.contains("translate")){ transform.translate = j["translate"]; }
    }

} // namespace SEED