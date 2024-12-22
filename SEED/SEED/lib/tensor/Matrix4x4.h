#pragma once
/// <summary>
/// 4x4行列
/// </summary>
struct Matrix4x4 final{
    float m[4][4];

    Matrix4x4(){
        for(int i = 0; i < 4; i++) {
            for(int j = 0; j < 4; j++) {
                m[i][j] = 0.0f;
            }
        }
    }

    Matrix4x4(const Matrix4x4& init){
        for(int i = 0; i < 4; i++) {
            for(int j = 0; j < 4; j++) {
                m[i][j] = init.m[i][j];
            }
        }
    }

    Matrix4x4(float aa, float ab, float ac, float ad, float ba, float bb, float bc, float bd, float ca, float cb, float cc, float cd, float da, float db, float dc, float dd){
        m[0][0] = aa;
        m[0][1] = ab;
        m[0][2] = ac;
        m[0][3] = ad;
        m[1][0] = ba;
        m[1][1] = bb;
        m[1][2] = bc;
        m[1][3] = bd;
        m[2][0] = ca;
        m[2][1] = cb;
        m[2][2] = cc;
        m[2][3] = cd;
        m[3][0] = da;
        m[3][1] = db;
        m[3][2] = dc;
        m[3][3] = dd;
    }

    Matrix4x4 operator*(const Matrix4x4 other)const{

        Matrix4x4 result;

        for(int i = 0; i < 4; i++) {
            for(int j = 0; j < 4; j++) {

                result.m[i][j] =
                    (m[i][0] * other.m[0][j]) +
                    (m[i][1] * other.m[1][j]) +
                    (m[i][2] * other.m[2][j]) +
                    (m[i][3] * other.m[3][j]);
            }
        }

        return result;
    }


    void operator*=(const Matrix4x4& other){

        for(int i = 0; i < 4; i++) {
            for(int j = 0; j < 4; j++) {

                m[i][j] =
                    (m[i][0] * other.m[0][j]) +
                    (m[i][1] * other.m[1][j]) +
                    (m[i][2] * other.m[2][j]) +
                    (m[i][3] * other.m[3][j]);
            }
        }
    }
};
