#pragma once
#include <assert.h>

struct Vector3;

struct Vector4 final{
    float x;
    float y;
    float z;
    float w;

    Vector4(){};
    Vector4(float x, float y, float z, float w){
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
    }

    ///////////////////////////////////////////////////////////////////
    /*                            Transform                          */
    ///////////////////////////////////////////////////////////////////

    //================== ADD ====================//

    Vector4 operator+(const Vector4& value){
        return { x + value.x,y + value.y,z + value.z,w + value.w };
    }

    void operator+=(const Vector4& value){
        x += value.x;
        y += value.y;
        z += value.z;
        w += value.w;
    }


    //================== SUB ====================//

    Vector4 operator-(const Vector4& value){
        return { x - value.x,y - value.y,z - value.z,w - value.w };
    }

    void operator-=(const Vector4& value){
        x -= value.x;
        y -= value.y;
        z -= value.z;
        w -= value.w;
    }

    //================== MUL ====================//

    Vector4 operator*(const Vector4& value){
        return { x * value.x,y * value.y,z * value.z,w * value.w };
    }

    Vector4 operator*(float value){
        return { x * value,y * value,z * value,w * value };
    }


    void operator*=(const Vector4& value){
        x *= value.x;
        y *= value.y;
        z *= value.z;
        w *= value.w;
    }

    void operator*=(float value){
        x *= value;
        y *= value;
        z *= value;
        w *= value;
    }

    Vector3 ToVec3();
};

