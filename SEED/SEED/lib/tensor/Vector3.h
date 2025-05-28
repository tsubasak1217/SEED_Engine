#pragma once
#include <cassert>
#include <nlohmann/json.hpp>
#include <SEED/Lib/Tensor/Matrix4x4.h>


struct Vector4;

/// <summary>
/// 3次元ベクトル
/// </summary>
struct Vector3 final {
	float x = 0;
	float y = 0;
	float z = 0;


    Vector3() = default;
    Vector3(float x, float y, float z){
        this->x = x;
        this->y = y;
        this->z = z;
    }


	// ADD----------------------------------
	Vector3 operator+(const Vector3& obj) const {
		Vector3 result;
		result.x = x + obj.x;
		result.y = y + obj.y;
		result.z = z + obj.z;

		return result;
	}
	void operator+=(const Vector3& obj) {
		x += obj.x;
		y += obj.y;
		z += obj.z;
	}
	Vector3 operator+(float obj) const {
		Vector3 result;
		result.x = x + obj;
		result.y = y + obj;
		result.z = z + obj;

		return result;
	}
	void operator+=(float obj) {
		x += obj;
		y += obj;
		z += obj;
	}

	// SUB--------------------------
	Vector3 operator-(const Vector3& obj) const {
		Vector3 result;
		result.x = x - obj.x;
		result.y = y - obj.y;
		result.z = z - obj.z;

		return result;
	}
	void operator-=(const Vector3& obj) {
		x -= obj.x;
		y -= obj.y;
		z -= obj.z;
	}
	Vector3 operator-(float obj) const {
		Vector3 result;
		result.x = x - obj;
		result.y = y - obj;
		result.z = z - obj;

		return result;
	}
	void operator-=(float obj) {
		x -= obj;
		y -= obj;
		z -= obj;
	}
    Vector3 operator-() const{
        return Vector3(-x, -y, -z);
    }

	// MULTIPLY-----------------------------
	Vector3 operator*(const Vector3& obj) const {
		Vector3 result;
		result.x = x * obj.x;
		result.y = y * obj.y;
		result.z = z * obj.z;

		return result;
	}

	void operator*=(const Vector3& obj) {
		x *= obj.x;
		y *= obj.y;
		z *= obj.z;
	}

	Vector3 operator*(float obj) const {
		Vector3 result;
		result.x = x * obj;
		result.y = y * obj;
		result.z = z * obj;

		return result;
	}
	void operator*=(float obj) {
		x *= obj;
		y *= obj;
		z *= obj;
	}

    Vector3 operator*(const Matrix4x4& obj) const{
        Vector3 result;
        float w;

        result.x = x * obj.m[0][0] + y * obj.m[1][0] + z * obj.m[2][0] + obj.m[3][0];
        result.y = x * obj.m[0][1] + y * obj.m[1][1] + z * obj.m[2][1] + obj.m[3][1];
        result.z = x * obj.m[0][2] + y * obj.m[1][2] + z * obj.m[2][2] + obj.m[3][2];
        w = x * obj.m[0][3] + y * obj.m[1][3] + z * obj.m[2][3] + obj.m[3][3];

        assert(w != 0);

        result.x /= w;
        result.y /= w;
        result.z /= w;

        return result;
    }

    void operator*=(const Matrix4x4& obj){
        float w;
        Vector3 tmp(x,y,z);

        x = tmp.x * obj.m[0][0] + tmp.y * obj.m[1][0] + tmp.z * obj.m[2][0] + obj.m[3][0];
        y = tmp.x * obj.m[0][1] + tmp.y * obj.m[1][1] + tmp.z * obj.m[2][1] + obj.m[3][1];
        z = tmp.x * obj.m[0][2] + tmp.y * obj.m[1][2] + tmp.z * obj.m[2][2] + obj.m[3][2];
        w = tmp.x * obj.m[0][3] + tmp.y * obj.m[1][3] + tmp.z * obj.m[2][3] + obj.m[3][3];

        assert(w != 0);

        x /= w;
        y /= w;
        z /= w;

    }


	// DEVIDE---------------------------------
	Vector3 operator/(float obj) const { return {x / obj, y / obj, z / obj}; }
	Vector3 operator/(const Vector3& obj) const { return {x / obj.x, y / obj.y, z / obj.z}; }
	void operator/=(float obj) {
		x = x / obj;
		y = y / obj;
		z = z / obj;
	}
	void operator/=(const Vector3& obj) {
		x = x / obj.x;
		y = y / obj.y;
		z = z / obj.z;
	}

    // EQUAL---------------------------------
    bool operator==(const Vector3& obj) const{ return x == obj.x && y == obj.y && z == obj.z; }

    Vector4 ToVec4();
};

// Vector3をJSONに変換する関数
inline void to_json(nlohmann::json& j, const Vector3& vec){
    j = { {"x", vec.x}, {"y", vec.y}, {"z", vec.z} };
}

// JSON から Vector3 に変換
inline void from_json(const nlohmann::json& j, Vector3& v){
    if(!j.contains("x") || !j.contains("y") || !j.contains("z")){
        v.x = j[0].get<float>();
        v.y = j[1].get<float>();
        v.z = j[2].get<float>();
    } else{
        v.x = j.at("x").get<float>();
        v.y = j.at("y").get<float>();
        v.z = j.at("z").get<float>();
    }
}

inline void to_json(nlohmann::ordered_json& j, const Vector3& vec){
    j = {{"x", vec.x}, {"y", vec.y}, {"z", vec.z}};
}

inline void from_json(const nlohmann::ordered_json& j, Vector3& v){
    if(!j.contains("x") || !j.contains("y") || !j.contains("z")){
        v.x = j[0].get<float>();
        v.y = j[1].get<float>();
        v.z = j[2].get<float>();
    } else{
        v.x = j.at("x").get<float>();
        v.y = j.at("y").get<float>();
        v.z = j.at("z").get<float>();
    }
}

struct Vector3Int{
    int x;
    int y;
    int z;

    // コンストラクタ
    Vector3Int() : x(0), y(0), z(0){}
    Vector3Int(int x, int y, int z) : x(x), y(y), z(z){}

    // 加算演算子
    Vector3Int operator+(const Vector3Int& other) const{
        return Vector3Int(x + other.x, y + other.y, z + other.z);
    }

    Vector3Int& operator+=(const Vector3Int& other){
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    // 減算演算子
    Vector3Int operator-(const Vector3Int& other) const{
        return Vector3Int(x - other.x, y - other.y, z - other.z);
    }

    Vector3Int& operator-=(const Vector3Int& other){
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }

    // スカラー乗算演算子
    Vector3Int operator*(int scalar) const{
        return Vector3Int(x * scalar, y * scalar, z * scalar);
    }

    Vector3Int& operator*=(int scalar){
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }

    // スカラー除算演算子
    Vector3Int operator/(int scalar) const{
        return Vector3Int(x / scalar, y / scalar, z / scalar);
    }

    Vector3Int& operator/=(int scalar){
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    }

    // 等価比較演算子
    bool operator==(const Vector3Int& other) const{
        return x == other.x && y == other.y && z == other.z;
    }

    // 不等価比較演算子
    bool operator!=(const Vector3Int& other) const{
        return !(*this == other);
    }

    // 符号反転演算子
    Vector3Int operator-() const{
        return Vector3Int(-x, -y, -z);
    }
};