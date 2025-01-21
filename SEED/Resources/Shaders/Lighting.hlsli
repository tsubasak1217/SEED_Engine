#pragma once
#include "Object3D.hlsli"
#include "Material.hlsli"

struct DirectionalLight {
    int type;
    float4 color;
    float intensity;
    int isLighting;
    float3 direction;
};

struct PointLight {
    int type;
    float4 color;
    float intensity;
    int isLighting;
    float3 position;
    float radius;
    float decay;
};

struct SpotLight {
    int type;
    float4 color;
    float intensity;
    int isLighting;
    float3 position;
    float3 direction;
    float distance;
    float decay;
    float cosAngle;
    float cosFallofStart;
};

struct LightingType {
    static const int NONE = 0;
    static const int LAMBERT = 1;
    static const int HALF_LAMBERT = 2;
};


void CalcDirectionalLight(
in DirectionalLight light,
in Material material,
inout float3 textureColor,
inout VertexShaderOutput input,
inout float3 toEye, inout float3 diffuse, inout float3 specular
) {
    // ライトが無効であればスキップ
    if (light.isLighting == 0) {
        return;
    }
    
    float3 lightDir = -normalize(light.direction);

    // ハーフランバート
    if (material.lightingType == LightingType::HALF_LAMBERT) {
        float NdotL = dot(input.normal, lightDir);
        float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        diffuse += material.color.rgb * textureColor.rgb *
                       light.color.rgb * light.intensity * cos;
    }
    
    // ランバート
    else if (material.lightingType == LightingType::LAMBERT) {
        float cos = saturate(dot(input.normal, lightDir));
        diffuse += material.color.rgb * textureColor.rgb *
                       light.color.rgb * light.intensity * cos;
    }

    // スペキュラ反射
    float3 halfVector = normalize(lightDir + toEye);
    float NdotH = saturate(dot(input.normal, halfVector));
    float specularPower = pow(NdotH, material.shinines);
    specular += light.color.rgb * specularPower * light.intensity;
}


// pointLightの計算
void CalcPointLight(
in PointLight light,
in Material material,
inout float3 textureColor,
inout VertexShaderOutput input,
inout float3 toEye, inout float3 diffuse, inout float3 specular
) {
    // ライトが無効であればスキップ
    if (light.isLighting == 0) {
        return;
    }

    float3 lightDir = light.position - input.worldPosition;
    float distance = length(lightDir);
    lightDir = normalize(lightDir);
    
    // ライトの影響範囲外であればスキップ
    if(distance > light.radius) {
        return;
    }
    
    // 距離減衰 (最大半径と減衰率を考慮)
    float factor = pow(saturate(-distance / light.radius + 1.0f), light.decay);
    float attenuation = 1.0f / (1.0f + light.decay * (distance / light.radius));

    // ハーフランバート
    if (material.lightingType == LightingType::HALF_LAMBERT) {
        float NdotL = dot(input.normal, lightDir);
        float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        diffuse += material.color.rgb * textureColor.rgb *
                       light.color.rgb * light.intensity * cos * attenuation * factor;
    }
    // ランバート
    else if (material.lightingType == LightingType::LAMBERT) {
        float cos = saturate(dot(input.normal, lightDir));
        diffuse += material.color.rgb * textureColor.rgb *
                       light.color.rgb * light.intensity * cos * attenuation * factor;
    }

    // スペキュラ反射
    float3 halfVector = normalize(lightDir + toEye);
    float NdotH = saturate(dot(input.normal, halfVector));
    float specularPower = pow(NdotH, material.shinines);
    specular += light.color.rgb * specularPower * light.intensity * attenuation * factor;
}

// spotLightの計算
void CalcSpotLight(
    in SpotLight light,
    in Material material,
    inout float3 textureColor,
    inout VertexShaderOutput input,
    inout float3 toEye,
    inout float3 diffuse,
    inout float3 specular
) {
    // ライトが無効であればスキップ
    if (light.isLighting == 0) {
        return;
    }

    // ライトから対象ピクセルへのベクトル
    float3 lightDir = input.worldPosition - light.position;
    float distance = length(lightDir);
    lightDir = normalize(lightDir);

    // ライトの影響範囲外であればスキップ
    if (distance > light.distance) {
        return;
    }

    // スポットライトのコサイン角度
    float cosTheta = dot(lightDir, normalize(light.direction));
    if (cosTheta < abs(light.cosAngle)) {
        return; // スポットの外側ならスキップ
    }

    // 距離減衰 (最大距離と減衰率を考慮)
    float distanceFactor = pow(saturate(-distance / light.distance + 1.0f), light.decay);
    float attenuation = 1.0f / (1.0f + light.decay * (distance / light.distance));

    // スポットライトの減衰 (コーン内での滑らかな減衰)
    float fallofFactor = saturate((cosTheta - light.cosAngle) / (light.cosFallofStart - light.cosAngle));
    
    // 最終的な減衰
    float finalAttenuation = attenuation * distanceFactor * fallofFactor;

    // ハーフランバート
    if (material.lightingType == LightingType::HALF_LAMBERT) {
        float NdotL = dot(input.normal, -lightDir);
        float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        diffuse += material.color.rgb * textureColor.rgb *
                       light.color.rgb * light.intensity * cos * finalAttenuation;
    }
    // ランバート
    else if (material.lightingType == LightingType::LAMBERT) {
        float cos = saturate(dot(input.normal, -lightDir));
        diffuse += material.color.rgb * textureColor.rgb *
                       light.color.rgb * light.intensity * cos * finalAttenuation;
    }

    // スペキュラ反射
    float3 halfVector = normalize(-lightDir + toEye);
    float NdotH = saturate(dot(input.normal, halfVector));
    float specularPower = pow(NdotH, material.shinines);
    specular += light.color.rgb * specularPower * light.intensity * finalAttenuation;
}