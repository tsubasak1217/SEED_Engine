#pragma once

//memory
#include <memory>
//container
#include <vector>
//object
class PlayerCorpse;

class PlayerCorpseManager{
public:
    PlayerCorpseManager();
    ~PlayerCorpseManager();
    void Initialize();
    void Update();
    void Draw();

    void BeginFrame();
    void EndFrame();
private:
    std::vector<std::unique_ptr<PlayerCorpse>> playerCorpses_;

public:
    void AddPlayerCorpse(std::unique_ptr<PlayerCorpse>& addCorpse);
};

