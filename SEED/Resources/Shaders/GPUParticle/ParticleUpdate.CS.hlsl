#include "GPUParticle.hlsli"
#include "../Object3d.hlsli"
#include "../Math/Math.hlsli"
#include "../Math/Quaternion.hlsli"

// �p�[�e�B�N��
RWStructuredBuffer<Particle> gParticles : register(u0, space0);

// �t���[���X�g�֘A
RWStructuredBuffer<uint> gFreeList : register(u1, space0); // �t���[���X�g(�t���[�ȃp�[�e�B�N���̗v�f�ԍ��̃��X�g)
RWStructuredBuffer<int> gFreeListIndex : register(u2, space0); // ���̎g�p����t���[���X�g�̗v�f�ԍ�

// �萔
ConstantBuffer<Float> gDeltaTime : register(b0); // DeltaTime

[numthreads(MAX_PARTICLE_COUNT, 1, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID) {
    
    // ����ł���p�[�e�B�N���͏������Ȃ�
    if (gParticles[DTid.x].alive != 1) {
        return;
    }
    
    // �����ɉ����Ĕ}��ϐ����v�Z
    float t = gParticles[DTid.x].currentTime / gParticles[DTid.x].lifeTime;
    
    // transform�̍X�V
    gParticles[DTid.x].position += gParticles[DTid.x].direction * gParticles[DTid.x].speed * gDeltaTime.value;
    
    gParticles[DTid.x].localRotation =
    gParticles[DTid.x].localRotation.Multiply(
        Quaternion::AngleAxis(
            gParticles[DTid.x].rotateSpeed * gDeltaTime.value,
            gParticles[DTid.x].rotateAxis
        )
    );  

    gParticles[DTid.x].rotation = gParticles[DTid.x].localRotation;
    
    // �F�̍X�V
    gParticles[DTid.x].color.a = 1.0f - t;
    
    // ���Ԃ̍X�V
    gParticles[DTid.x].currentTime += gDeltaTime.value;
    
    // �p�[�e�B�N���̎������s�����玀��
    gParticles[DTid.x].alive = gParticles[DTid.x].currentTime < gParticles[DTid.x].lifeTime;
    
    if(!gParticles[DTid.x].alive) {
    
        // �X�P�[�����[���ɂ��Ĕ�\���ɂ���
        gParticles[DTid.x].scale = float3(0.0f, 0.0f, 0.0f); 
    
        // �t���[���X�g�̍X�V
        int freeListIndex;
        InterlockedAdd(gFreeListIndex[0], 1, freeListIndex);
        
        if(freeListIndex + 1 < MAX_PARTICLE_COUNT) {
            // �t���[���X�g�Ƀp�[�e�B�N���̃C���f�b�N�X��ǉ�
            gFreeList[freeListIndex + 1] = gParticles[DTid.x].particleIndex;
        } else {
            // �ꉞ���S��
            InterlockedAdd(gFreeListIndex[0], -1);
        }
    }
}