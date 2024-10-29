#pragma once

class ParticleManager{

private:
    ParticleManager() = default;
    // コピー禁止
    ParticleManager(const ParticleManager&) = delete;
    void operator=(const ParticleManager&) = delete;

public:
    ~ParticleManager();
    static ParticleManager* GetInstance();

private:

    static ParticleManager* instance_;

};