#include "GPUParticle.hlsli"
#include "../Object3d.hlsli"
#include "../Random/Random.hlsli"
#include "../Math/Math.hlsli"
#include "../Math/Quaternion.hlsli"

// �I�u�W�F�N�g���X�g
StructuredBuffer<Emitter> gEmitters : register(t0, space0);
RWStructuredBuffer<Particle> gParticles : register(u0, space0); // �����Ƀp�[�e�B�N�����i�[

// �t���[���X�g�֘A
RWStructuredBuffer<int> gFreeListIndex : register(u1, space0); // ���̎g�p����t���[���X�g�̗v�f�ԍ�
StructuredBuffer<int> gFreeList : register(t1, space0);

// �萔
ConstantBuffer<Int> gEmitterCount : register(b0); // �G�~�b�^�[�̐�
ConstantBuffer<Float> gDeltaTime : register(b1); // DeltaTime
ConstantBuffer<Float> gCurrentTime : register(b2); // ���݂̎���(�����p)

///////////////////////////////////////////////////////////////////////
// �G�~�b�^�[�̏������ƂɃp�[�e�B�N���𔭐�������֐�
///////////////////////////////////////////////////////////////////////
void Emit(Emitter emitter, float seed) {

    // ����������̏�����
    rand.Init(seed);

    for (uint i = 0; i < emitter.numEmitEvery; ++i) {

        // �������猸�炵�Ă��������Ńt���[�C���f�b�N�X���擾
        int freeIndex;
        InterlockedAdd(gFreeListIndex[0], -1, freeIndex);

        // �͈͊O�Ŕ����������Ȃ��ꍇ,�ԍ���߂��ďI��
        if (freeIndex < 0 || freeIndex >= MAX_PARTICLE_COUNT) {
            InterlockedAdd(gFreeListIndex[0], 1);
            continue;
        }
        
        // �t���[���X�g����p�[�e�B�N���̃C���f�b�N�X���擾
        uint freeListIndex = gFreeList[freeIndex];
        
        // �p�[�e�B�N���̏�����
        // translate
        float3 minPos = emitter.position - (emitter.emitRange * 0.5);
        float3 maxPos = emitter.position + (emitter.emitRange * 0.5);
        gParticles[freeListIndex].position = rand.Random3D(minPos, maxPos);
        
        // scale
        gParticles[freeListIndex].kScale = rand.Random3D(emitter.minScale, emitter.maxScale);
        gParticles[freeListIndex].scale = gParticles[freeListIndex].kScale;
        
        // ��]�̏�����
        if (emitter.isBillboard) {
            gParticles[freeListIndex].isBillboard = true;
        }
        
        if (emitter.useRotateAxis) {
            gParticles[freeListIndex].rotateAxis = emitter.rotateAxis;
        } else {
            gParticles[freeListIndex].localRotation = Quaternion::ToQuaternion(rand.Random3D(emitter.minRotation, emitter.maxRotation));
            gParticles[freeListIndex].rotateAxis = normalize(rand.Random3D(-1.0f, 1.0f)); // �����_���ȉ�]���𐶐�
        }
        
        gParticles[freeListIndex].rotateSpeed = rand.Random(emitter.minRotateSpeed, emitter.maxRotateSpeed);
        
        // ��{�����ƃ����_���ȕ�����g�ݍ��킹�čŏI�I�ȕ���������
        gParticles[freeListIndex].direction = emitter.baseDirection;
        float angleRange = 3.14f * clamp(emitter.directionRange, 0.0f, 1.0f);
        float theta = rand.Random(-angleRange, angleRange); // ������]
        float phi = rand.Random(-angleRange / 2.0f, angleRange / 2.0f); // ������] (�͈͂𐧌�)
        float3 rotateVec = CreateVector(theta, phi); // ���ʏ�̃x�N�g���𐶐�
        gParticles[freeListIndex].direction = Mul(rotateVec, Quaternion::DirectionToDirection(float3(1, 0, 0), gParticles[freeListIndex].direction)); // ��]��K�p
    
        // ���̑�
        gParticles[freeListIndex].speed = rand.Random(emitter.minSpeed, emitter.maxSpeed);
        gParticles[freeListIndex].lifeTime = rand.Random(emitter.minLifeTime, emitter.maxLifeTime);
        gParticles[freeListIndex].currentTime = 0;
        gParticles[freeListIndex].alive = true;
        gParticles[freeListIndex].color = emitter.color;
        
        // ��������
        gParticles[freeListIndex].particleIndex = freeListIndex;
        gParticles[freeListIndex].textureIdx = emitter.textureIndex;
    }
}


///////////////////////////////////////////////////////////////////////
// ���C���֐�
///////////////////////////////////////////////////////////////////////
[numthreads(MAX_EMITTER_COUNT, 1, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID) {

    // �G�~�b�^�[�̐��𒴂����ꍇ�͉������Ȃ�
    if (DTid.x >= gEmitterCount.value) {
        return;
    }

    // �I����Ă���Ώ������Ȃ�
    if (gEmitters[DTid.x].alive != 1) {
        return;
    }
    
    // seed���v�Z
    float seed = (DTid.x * gCurrentTime.value) * gDeltaTime.value;
    
    // ���Ԃ������甭��
    if (gEmitters[DTid.x].currentTime >= gEmitters[DTid.x].interval) {
        
        // �J��Ԃ����ǂ����ŏ����𕪊�
        Emit(gEmitters[DTid.x], seed);

    }
}