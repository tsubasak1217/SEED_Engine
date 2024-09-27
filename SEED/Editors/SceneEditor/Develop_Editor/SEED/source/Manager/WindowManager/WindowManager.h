#pragma once

class SEED;

class WindowManager{
public:
    void Initialize(SEED* pSEED);
    void Finalize();

private:
    void InitializeGDI();
    SEED* pSEED_;
};