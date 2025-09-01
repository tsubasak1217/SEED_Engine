#ifndef GPUPARTICLE_INCLUDED
#define GPUPARTICLE_INCLUDED
#include "../Math/Quaternion.hlsli"

// GPUParticle��Plane�݂̂ɋ@�\���i��B�f�t�H���g�Ńr���{�[�h�B
struct Emitter {
    // position
    float3 position;
    float pad1;
    float3 emitRange;
    float pad2;
    // scale
    float3 minScale;
    float pad3;
    float3 maxScale;
    float pad4;
    // rotate
    float minRotation;
    float maxRotation;
    float minRotateSpeed;
    float maxRotateSpeed;
    // direction
    float3 baseDirection;
    float pad5;
    float angleRange;
    // speed
    float minSpeed;
    float maxSpeed;
    float pad6;
    // �F
    float4 color;
    // parameter
    float minLifeTime;
    float maxLifeTime;
    float interval;
    float currentTime;
    int numEmitEvery;
    int textureIndex;
    bool emit; // bool
    bool loop; // bool
    bool alive; // bool
    float padding[3]; // 16�o�C�g���E�̂��߂̃p�f�B���O
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