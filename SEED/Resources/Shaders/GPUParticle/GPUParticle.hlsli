#ifndef GPUPARTICLE_INCLUDED
#define GPUPARTICLE_INCLUDED
#include "../Math/Quaternion.hlsli"

static const int MAX_PARTICLE_COUNT = 1024;
static const int MAX_EMITTER_COUNT = 0xff;

struct Emitter {
    // position
    float3 position;
    float3 emitRange;
    // scale
    float3 minScale;
    float3 maxScale;
    // rotate
    float3 minRotation;
    float3 maxRotation;
    float3 rotateAxis;
    bool useRotateAxis;
    float minRotateSpeed;
    float maxRotateSpeed;
    bool isBillboard;
    // direction
    float3 baseDirection;
    float directionRange;
    // speed
    float minSpeed;
    float maxSpeed;
    // êF
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
    Quaternion rotation;
    Quaternion localRotation;
    float3 kScale;
    float3 scale;
    float3 direction;
    float speed;
    float3 rotateAxis;
    float rotateSpeed;
    float lifeTime;
    float currentTime;
    float4 color;
    bool isBillboard;
    bool alive;
    int textureIdx;
    int particleIndex;
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