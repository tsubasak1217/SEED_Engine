#pragma once
#include <SEED/Lib/Tensor/Vector4.h>
#include <SEED/Lib/Tensor/Vector3.h>
#include <SEED/Lib/Structs/Color.h>

namespace SEED{

    // ライトの種類
    enum LIGHT_TYPE{
        BASE_LIGHT = 0,
        DIRECTIONAL_LIGHT,
        POINT_LIGHT,
        SPOT_LIGHT
    };

    /// <summary>
    /// ライトの基底クラス
    /// </summary>
    class BaseLight{
    public:
        // polygonManagerに渡すための関数
        void SendData();
        int32_t GetLightType()const{ return lightType_; }

    protected:
        int32_t lightType_;

    public:
        Color color_ = { 1.0f,1.0f,1.0f,1.0f };
        float intensity = 1.0f;
        bool isLighting_ = true;;
    };

} // namespace SEED