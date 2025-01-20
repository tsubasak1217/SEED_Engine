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
    // ���C�g�������ł���΃X�L�b�v
    if (light.isLighting == 0) {
        return;
    }
    
    float3 lightDir = -normalize(light.direction);

    // �n�[�t�����o�[�g
    if (material.lightingType == LightingType::HALF_LAMBERT) {
        float NdotL = dot(input.normal, lightDir);
        float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        diffuse += material.color.rgb * textureColor.rgb *
                       light.color.rgb * light.intensity * cos;
    }
    
    // �����o�[�g
    else if (material.lightingType == LightingType::LAMBERT) {
        float cos = saturate(dot(input.normal, lightDir));
        diffuse += material.color.rgb * textureColor.rgb *
                       light.color.rgb * light.intensity * cos;
    }

    // �X�y�L��������
    float3 halfVector = normalize(lightDir + toEye);
    float NdotH = saturate(dot(input.normal, halfVector));
    float specularPower = pow(NdotH, material.shinines);
    specular += light.color.rgb * specularPower * light.intensity;
}


// pointLight�̌v�Z
void CalcPointLight(
in PointLight light,
in Material material,
inout float3 textureColor,
inout VertexShaderOutput input,
inout float3 toEye, inout float3 diffuse, inout float3 specular
) {
    // ���C�g�������ł���΃X�L�b�v
    if (light.isLighting == 0) {
        return;
    }

    float3 lightDir = light.position - input.worldPosition;
    float distance = length(lightDir);
    lightDir = normalize(lightDir);

    // �������� (�W���I�Ȍ������f�����g�p)
    float attenuation = 1.0f / (1.0f + 0.1f * distance + 0.01f * distance * distance);

    // �n�[�t�����o�[�g
    if (material.lightingType == LightingType::HALF_LAMBERT) {
        float NdotL = dot(input.normal, lightDir);
        float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        diffuse += material.color.rgb * textureColor.rgb *
                       light.color.rgb * light.intensity * cos * attenuation;
    }
    // �����o�[�g
    else if (material.lightingType == LightingType::LAMBERT) {
        float cos = saturate(dot(input.normal, lightDir));
        diffuse += material.color.rgb * textureColor.rgb *
                       light.color.rgb * light.intensity * cos * attenuation;
    }

    // �X�y�L��������
    float3 halfVector = normalize(lightDir + toEye);
    float NdotH = saturate(dot(input.normal, halfVector));
    float specularPower = pow(NdotH, material.shinines);
    specular += light.color.rgb * specularPower * light.intensity * attenuation;
}