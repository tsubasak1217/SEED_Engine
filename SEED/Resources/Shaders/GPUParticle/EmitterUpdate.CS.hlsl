#include "GPUParticle.hlsli"
#include "../Object3d.hlsli"
#include "../Random/Random.hlsli"
#include "../Math/Math.hlsli"
#include "../Math/Quaternion.hlsli"

// �I�u�W�F�N�g���X�g
ConstantBuffer<Emitter> gEmitter : register(b0, space0);
RWStructuredBuffer<Particle> gParticles : register(u0, space0); // �����Ƀp�[�e�B�N�����i�[

// �t���[���X�g�֘A
RWStructuredBuffer<int> gFreeListIndex : register(u1, space0); // ���̎g�p����t���[���X�g�̗v�f�ԍ�
StructuredBuffer<int> gFreeList : register(t0, space0);

// �p�[�e�B�N���̐�
RWStructuredBuffer<int> gParticleCount : register(u2, space0); // ���̎g�p����t���[���X�g�̗v�f�ԍ�

// �萔
ConstantBuffer<Float> gDeltaTime : register(b1); // DeltaTime(�����p)
ConstantBuffer<Float> gTotalTime : register(b2); // ����(�����p)
ConstantBuffer<Int> gMaxParticleCount : register(b3); // �ő�p�[�e�B�N����

///////////////////////////////////////////////////////////////////////
// �G�~�b�^�[�̏������ƂɃp�[�e�B�N���𔭐�������֐�
///////////////////////////////////////////////////////////////////////
void Emit(float seed) {

    // �������猸�炵�Ă��������Ńt���[�C���f�b�N�X���擾
    int freeIndex;
    InterlockedAdd(gFreeListIndex[0], -1, freeIndex);

    // �͈͊O�Ŕ����������Ȃ��ꍇ,�ԍ���߂��ďI��
    if (freeIndex < 0 || freeIndex >= gMaxParticleCount.value) {
        InterlockedAdd(gFreeListIndex[0], 1);
        return;
    }
    
    // ����������̏�����
    Random rand;
    rand.Init(seed);
    
    // �t���[���X�g����p�[�e�B�N���̃C���f�b�N�X���擾
    uint freeListIndex = gFreeList[freeIndex];
    
    // �p�[�e�B�N���̏�����
    // translate
    float3 minPos = gEmitter.position - (gEmitter.emitRange * 0.5);
    float3 maxPos = gEmitter.position + (gEmitter.emitRange * 0.5);
    gParticles[freeListIndex].position = rand.Get3D(minPos, maxPos);
    
    // scale
    gParticles[freeListIndex].kScale = rand.Get3D(gEmitter.minScale, gEmitter.maxScale);
    gParticles[freeListIndex].scale = gParticles[freeListIndex].kScale;
    
    // ��]�̏�����
    gParticles[freeListIndex].rotation = rand.Get1D(gEmitter.minRotation, gEmitter.maxRotation);
    gParticles[freeListIndex].rotateSpeed = rand.Get1D(gEmitter.minRotateSpeed, gEmitter.maxRotateSpeed);
    
    // ���˕�����ݒ�
    gParticles[freeListIndex].direction = gEmitter.baseDirection;
    float angleRange = PI * clamp(gEmitter.angleRange, 0.0f, 2.0f);
    float theta = rand.Get1D(-angleRange, angleRange); // ������]
    float phi = rand.Get1D(-angleRange, angleRange); // ������] (�͈͂𐧌�)
    float3 rotateVec = CreateVector(theta, phi); // ���ʏ�̃x�N�g���𐶐�
    gParticles[freeListIndex].direction = Mul(rotateVec, Quaternion::DirectionToDirection(float3(1, 0, 0), gParticles[freeListIndex].direction)); // ��]��K�p
    
    // ���̑�
    gParticles[freeListIndex].color = gEmitter.color;
    gParticles[freeListIndex].speed = rand.Get1D(gEmitter.minSpeed, gEmitter.maxSpeed);
    gParticles[freeListIndex].lifeTime = rand.Get1D(gEmitter.minLifeTime, gEmitter.maxLifeTime);
    gParticles[freeListIndex].currentTime = 0;
    gParticles[freeListIndex].alive = true;
    gParticles[freeListIndex].textureIdx = gEmitter.textureIndex;
    gParticles[freeListIndex].particleIndex = freeListIndex;
    
    // �p�[�e�B�N���̐��𑝂₷
    InterlockedAdd(gParticleCount[0], 1);
}


///////////////////////////////////////////////////////////////////////
// ���C���֐�
///////////////////////////////////////////////////////////////////////
[numthreads(1024, 1, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID) {
    
    // �������𒴂����ꍇ�͉������Ȃ�
    if (DTid.x >= gEmitter.numEmitEvery) {
        return;
    }
    
    // �����p�̃V�[�h�𐶐�
    float seed = gTotalTime.value + gDeltaTime.value * DTid.x;
    
    // �o��������
    Emit(seed);
}