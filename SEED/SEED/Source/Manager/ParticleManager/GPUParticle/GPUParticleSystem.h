#pragma once
#include <vector>
#include <memory>
#include <SEED/Source/Manager/DxManager/DxResource.h>
#include <SEED/Source/Manager/DxManager/DxBuffer.h>
#include <SEED/Source/Manager/DxManager/DxReadbackBuffer.h>
#include <SEED/Lib/Functions/DxFunc.h>
#include <SEED/Lib/Structs/VertexData.h>
#include <SEED/Source/Manager/ModelManager/ModelManager.h>
#include "GPUEmitter.h"
#include "GPUParticle.h"


/// <summary>
/// GPUパーティクルのシステム
/// </summary>
class GPUParticleSystem{
private:
    struct GPUCameraInfo{
        Matrix4x4 viewProjectionMatrix;
        Quaternion cameraRotation;
        Vector3 cameraPosition;
    };

private:
    static inline GPUParticleSystem* instance_ = nullptr;
    GPUParticleSystem() = default;
    // コピー禁止
    GPUParticleSystem(const GPUParticleSystem&) = delete;
    void operator=(const GPUParticleSystem&) = delete;
    // ムーブ禁止
    GPUParticleSystem(GPUParticleSystem&&) = delete;
    void operator=(GPUParticleSystem&&) = delete;

public:
    static GPUParticleSystem* GetInstance();
    virtual ~GPUParticleSystem() = default;

public:
    static void Initialize();
    static void Update();
    static void Draw(const std::string& cameraName);
    static void Release();

private:
    void InitPSO();
    void CreateResources();
    void BindInfo();
    void BindPerFrame();
    void Dispatch(const std::string& pipelineName, int32_t dispatchX, int32_t dispatchY);

public:
    void DrawGUI();

private:
    // 最大数
    int32_t maxParticleCount_ = 1000;
    int32_t maxVertexCount_ = 10240000; // 最大頂点数

    // エミッター関連
    GPUEmitter emitter_;
    std::vector<Vector4> colors_; // パーティクルの色
    std::vector<int32_t> textureHandles_;// パーティクルのテクスチャハンドル(モデルの規定のもの)
    std::vector<int32_t> customTextureHandles_; // パーティクルのテクスチャハンドル(任意のもの)
    bool useCustomTexture_ = false; // 任意のテクスチャを使用するかどうか

private:
    // buffer
    DxBuffer<GPUEmitter> emitterBuffer_;
    DxBuffer<GPUParticle> particleBuffer_;
    DxBuffer<uint32_t> particleFreeListBuffer_;
    DxBuffer<int32_t> particleFreeListIndexBuffer_;
    DxBuffer<GPUCameraInfo> cameraInfoBuffer_;
    DxBuffer<Vector4> particleColorBuffer_;
    DxBuffer<int32_t> particleTextureIndexBuffer_;
    // デバッグ用の値確認バッファ
    DxReadbackBuffer<int32_t> particleCountBuffer_;
    // 描画用のバッファ
    DxBuffer<VertexData> vertexBuffer_;
    DxBuffer<uint32_t> indexBuffer_;
    ModelData* currentModelData_ = nullptr;
    D3D12_VERTEX_BUFFER_VIEW vbv_vertex;
    D3D12_INDEX_BUFFER_VIEW ibv;
    // 32ビット定数用
    float deltaTime_;
    float totalTime_;// 現在の時刻
};