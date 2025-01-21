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
    
    // ���C�g�̉e���͈͊O�ł���΃X�L�b�v
    if(distance > light.radius) {
        return;
    }
    
    // �������� (�ő唼�a�ƌ��������l��)
    float factor = pow(saturate(-distance / light.radius + 1.0f), light.decay);
    float attenuation = 1.0f / (1.0f + light.decay * (distance / light.radius));

    // �n�[�t�����o�[�g
    if (material.lightingType == LightingType::HALF_LAMBERT) {
        float NdotL = dot(input.normal, lightDir);
        float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        diffuse += material.color.rgb * textureColor.rgb *
                       light.color.rgb * light.intensity * cos * attenuation * factor;
    }
    // �����o�[�g
    else if (material.lightingType == LightingType::LAMBERT) {
        float cos = saturate(dot(input.normal, lightDir));
        diffuse += material.color.rgb * textureColor.rgb *
                       light.color.rgb * light.intensity * cos * attenuation * factor;
    }

    // �X�y�L��������
    float3 halfVector = normalize(lightDir + toEye);
    float NdotH = saturate(dot(input.normal, halfVector));
    float specularPower = pow(NdotH, material.shinines);
    specular += light.color.rgb * specularPower * light.intensity * attenuation * factor;
}

// spotLight�̌v�Z
void CalcSpotLight(
    in SpotLight light,
    in Material material,
    inout float3 textureColor,
    inout VertexShaderOutput input,
    inout float3 toEye,
    inout float3 diffuse,
    inout float3 specular
) {
    // ���C�g�������ł���΃X�L�b�v
    if (light.isLighting == 0) {
        return;
    }

    // ���C�g����Ώۃs�N�Z���ւ̃x�N�g��
    float3 lightDir = input.worldPosition - light.position;
    float distance = length(lightDir);
    lightDir = normalize(lightDir);

    // ���C�g�̉e���͈͊O�ł���΃X�L�b�v
    if (distance > light.distance) {
        return;
    }

    // �X�|�b�g���C�g�̃R�T�C���p�x
    float cosTheta = dot(lightDir, normalize(light.direction));
    if (cosTheta < abs(light.cosAngle)) {
        return; // �X�|�b�g�̊O���Ȃ�X�L�b�v
    }

    // �������� (�ő勗���ƌ��������l��)
    float distanceFactor = pow(saturate(-distance / light.distance + 1.0f), light.decay);
    float attenuation = 1.0f / (1.0f + light.decay * (distance / light.distance));

    // �X�|�b�g���C�g�̌��� (�R�[�����ł̊��炩�Ȍ���)
    float fallofFactor = saturate((cosTheta - light.cosAngle) / (light.cosFallofStart - light.cosAngle));
    
    // �ŏI�I�Ȍ���
    float finalAttenuation = attenuation * distanceFactor * fallofFactor;

    // �n�[�t�����o�[�g
    if (material.lightingType == LightingType::HALF_LAMBERT) {
        float NdotL = dot(input.normal, -lightDir);
        float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        diffuse += material.color.rgb * textureColor.rgb *
                       light.color.rgb * light.intensity * cos * finalAttenuation;
    }
    // �����o�[�g
    else if (material.lightingType == LightingType::LAMBERT) {
        float cos = saturate(dot(input.normal, -lightDir));
        diffuse += material.color.rgb * textureColor.rgb *
                       light.color.rgb * light.intensity * cos * finalAttenuation;
    }

    // �X�y�L��������
    float3 halfVector = normalize(-lightDir + toEye);
    float NdotH = saturate(dot(input.normal, halfVector));
    float specularPower = pow(NdotH, material.shinines);
    specular += light.color.rgb * specularPower * light.intensity * finalAttenuation;
}