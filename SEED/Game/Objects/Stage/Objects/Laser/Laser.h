#pragma once


//============================================================================
//	include
//============================================================================
#include <Game/Objects/Stage/Objects/Interface/IStageObject.h>

//============================================================================
//	Laser class
//============================================================================
class Laser : public IStageObject {
public:

    Laser(GameObject2D* owner) : IStageObject(owner) {}
    Laser() = default;
    ~Laser() = default;

    void Initialize(const std::string& filename) override;

    void Update() override;

    void Draw() override;

    void Edit() override {};

    //--------- accessor -----------------------------------------------------

private:
    //========================================================================
    //	private Methods
    //========================================================================

    //--------- variables ----------------------------------------------------



    //--------- functions ----------------------------------------------------
};