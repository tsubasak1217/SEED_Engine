#include "GPUParticle.hlsli"
#include "../Object3d.hlsli"
#include "../Math/Math.hlsli"
#include "../Math/Quaternion.hlsli"

RWStructuredBuffer<Particle> gParticles : register(u0, space0);
RWStructuredBuffer<uint> gFreeList : register(u1, space0); // �t���[���X�g(�t���[�ȃp�[�e�B�N���̗v�f�ԍ��̃��X�g)
RWStructuredBuffer<int> gFreeListIndex : register(u2, space0); // ���̎g�p����t���[���X�g�̗v�f�ԍ�
ConstantBuffer<Int> gMaxParticleCount : register(b0); // �ő�p�[�e�B�N����

[numthreads(1024, 1, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID) {
    
    gParticles[DTid.x] = (Particle)0;
    gFreeList[DTid.x] = DTid.x; // �t���[���X�g�Ƀp�[�e�B�N���̃C���f�b�N�X��ǉ�
    
    if(DTid.x == 0) {
        // �t���[���X�g�̃C���f�b�N�X���X�V
        gFreeListIndex[0] = gMaxParticleCount.value - 1; // �Ō�̗v�f���w��
    }
}