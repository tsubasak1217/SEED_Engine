#pragma once
#include <cassert>
#include "Matrix4x4.h"


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

        x = x * obj.m[0][0] + y * obj.m[1][0] + z * obj.m[2][0] + obj.m[3][0];
        y = x * obj.m[0][1] + y * obj.m[1][1] + z * obj.m[2][1] + obj.m[3][1];
        z = x * obj.m[0][2] + y * obj.m[1][2] + z * obj.m[2][2] + obj.m[3][2];
        w = x * obj.m[0][3] + y * obj.m[1][3] + z * obj.m[2][3] + obj.m[3][3];

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