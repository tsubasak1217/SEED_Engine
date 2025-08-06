#include "GPUParticle.hlsli"
#include "../Object3d.hlsli"
#include "../Math/Math.hlsli"
#include "../Math/Quaternion.hlsli"

// �p�[�e�B�N��
RWStructuredBuffer<Particle> gParticles : register(u0, space0);

// �t���[���X�g�֘A
RWStructuredBuffer<uint> gFreeList : register(u1, space0); // �t���[���X�g(�t���[�ȃp�[�e�B�N���̗v�f�ԍ��̃��X�g)
RWStructuredBuffer<int> gFreeListIndex : register(u2, space0); // ���̎g�p����t���[���X�g�̗v�f�ԍ�

// �p�[�e�B�N���̐�
RWStructuredBuffer<int> gParticleCount : register(u3, space0); // ���̎g�p����t���[���X�g�̗v�f�ԍ�

// �萔
ConstantBuffer<Float> gDeltaTime : register(b0); // DeltaTime
ConstantBuffer<Int> gMaxParticleCount : register(b1); // �ő�p�[�e�B�N����

[numthreads(1024, 1, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID) {
    
    int index = DTid.x;
    
    // ����ł���p�[�e�B�N���͏������Ȃ�
    if (gParticles[index].alive != 1) {
        return;
    }
    
    // �͈͊O�̃A�N�Z�X��h��
    if(index < 0) {
        return;
    }
    
    // �����ɉ����Ĕ}��ϐ����v�Z
    float t = gParticles[index].currentTime / gParticles[index].lifeTime;
    
    // transform�̍X�V
    gParticles[index].position += gParticles[index].direction * gParticles[index].speed * gDeltaTime.value;
    gParticles[index].rotation += gParticles[index].rotateSpeed * gDeltaTime.value;
    
    // �F�̍X�V
    gParticles[index].color.a = 1.0f - t;
    
    // ���Ԃ̍X�V
    gParticles[index].currentTime += gDeltaTime.value;
    
    // �p�[�e�B�N���̎������s�����玀��
    gParticles[index].alive = gParticles[index].currentTime < gParticles[index].lifeTime;
    
    if(!gParticles[index].alive) {
    
        // �X�P�[�����[���ɂ��Ĕ�\���ɂ���
        gParticles[index].scale = float3(0.0f, 0.0f, 0.0f); 
    
        // �t���[���X�g�̍X�V
        int freeListIndex;
        InterlockedAdd(gFreeListIndex[0], 1, freeListIndex);
        InterlockedAdd(gParticleCount[0], -1);
        
        if (freeListIndex + 1 < gMaxParticleCount.value) {
            // �t���[���X�g�Ƀp�[�e�B�N���̃C���f�b�N�X��ǉ�
            gFreeList[freeListIndex + 1] = gParticles[index].particleIndex;
        } else {
            // �ꉞ���S��
            InterlockedAdd(gFreeListIndex[0], -1);
        }
    }
}