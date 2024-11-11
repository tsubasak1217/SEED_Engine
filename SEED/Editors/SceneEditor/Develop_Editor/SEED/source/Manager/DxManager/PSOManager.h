#pragma once

class DxManager;
struct ID3D12RootSignature;

struct PSOManager{

    PSOManager(DxManager* pDxManager);
    ~PSOManager();
    
    ID3D12RootSignature* SettingCSRootSignature();
    void Create();

private:
    DxManager* pDxManager_;
};