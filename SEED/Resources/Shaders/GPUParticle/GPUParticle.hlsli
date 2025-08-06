#ifndef GPUPARTICLE_INCLUDED
#define GPUPARTICLE_INCLUDED
#include "../Math/Quaternion.hlsli"

// GPUParticleはPlaneのみに機能を絞る。デフォルトでビルボード。
struct Emitter {
    // position
    float3 position;
    float3 emitRange;
    // scale
    float3 minScale;
    float3 maxScale;
    // rotate
    float minRotation;
    float maxRotation;
    float minRotateSpeed;
    float maxRotateSpeed;
    // direction
    float3 baseDirection;
    float angleRange;
    // speed
    float minSpeed;
    float maxSpeed;
    // 色
    float4 color;
    // parameter
    float minLifeTime;
    float maxLifeTime;
    float interval;
    float currentTime;
    int numEmitEvery;
    int textureIndex;
    bool emit;
    bool loop;
    bool alive;
};


struct Particle {
    float3 position;
    float rotation;
    float3 kScale;
    float3 scale;
    float3 direction;
    float speed;
    float rotateSpeed;
    float lifeTime;
    float currentTime;
    float4 color;
    int textureIdx;
    int particleIndex;
    bool alive;
};

struct GPUParticleVSOutput {
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 worldPosition : WORLDPOSITION0;
    uint instanceID : Index0;
    float4 particleColor : PARTICLE_COLOR0;
    int particleTextureIdx : PARTICLE_TEXTURE_IDX0;
};

#endif