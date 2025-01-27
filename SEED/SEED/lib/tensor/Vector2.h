#pragma once
#include <nlohmann/json.hpp>

struct Matrix3x3;
struct Vector3;

/// <summary>
/// 2次元ベクトル
/// </summary>
struct Vector2 final {
	float x = 0;
	float y = 0;


    Vector2() = default;
    Vector2(float x, float y){
        this->x = x;
        this->y = y;
    }

	// ADD-----------------------------------
	Vector2 operator+(const Vector2& obj) const {
		Vector2 result;
		result.x = x + obj.x;
		result.y = y + obj.y;

		return result;
	}
	void operator+=(const Vector2& obj) {
		x += obj.x;
		y += obj.y;
	}
	Vector2 operator+(float obj) const {
		Vector2 result;
		result.x = x + obj;
		result.y = y + obj;

		return result;
	}
	void operator+=(float obj) {
		x += obj;
		y += obj;
	}

	// SUB---------------------------------
	Vector2 operator- (const Vector2& obj) const {
		Vector2 result;
		result.x = x - obj.x;
		result.y = y - obj.y;

		return result;
	}
	void operator-=(const Vector2& obj) {
		x -= obj.x;
		y -= obj.y;
	}
	Vector2 operator-(float obj) const {
		Vector2 result;
		result.x = x - obj;
		result.y = y - obj;

		return result;
	}
	void operator-=(float obj) {
		x -= obj;
		y -= obj;
	}

	// MULTIPLY-----------------------
	Vector2 operator*(const Vector2& obj) const {
		Vector2 result;
		result.x = x * obj.x;
		result.y = y * obj.y;

		return result;
	}
	void operator*=(const Vector2& obj) {
		x *= obj.x;
		y *= obj.y;
	}
	Vector2 operator*(float obj) const {
		Vector2 result;
		result.x = x * obj;
		result.y = y * obj;

		return result;
	}
	void operator*=(float obj) {
		x *= obj;
		y *= obj;
	}

    Vector2 operator*(const Matrix3x3& mat) const;

    void operator*=(const Matrix3x3& mat);

	// DEVIDE---------------------------------
	Vector2 operator/(float obj) const {
		return  {
		x / obj,
		y / obj
		};
	}
	Vector2 operator/(const Vector2& obj) const {
		return  {
		x / obj.x,
		y / obj.y
		};
	}
	void operator/=(float obj) {
		x = x / obj;
		y = y / obj;
	}
	void operator/=(const Vector2& obj) {
		x = x / obj.x;
		y = y / obj.y;
	}

    // EQUAL---------------------------------
    bool operator==(const Vector2& obj) const{
        return x == obj.x && y == obj.y;
    }


    Vector3 ToVec3() const;
};

// Vector2をJSONに変換する関数
inline void to_json(nlohmann::json& j, const Vector2& vec){
    j = { {"x", vec.x}, {"y", vec.y} };
}

// JSONをVector2に変換する関数
inline void from_json(const nlohmann::json& j, Vector2& vec){
    vec.x = j.at("x").get<float>();
    vec.y = j.at("y").get<float>();
}

inline void to_json(nlohmann::ordered_json& j,const Vector2& vec){
    j = {{"x",vec.x},{"y",vec.y}};
}

inline void from_json(const nlohmann::ordered_json& j,Vector2& v){
    v.x = j.at("x").get<float>();
    v.y = j.at("y").get<float>();
}