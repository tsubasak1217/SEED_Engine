#pragma once

//============================================================================
//	include
//============================================================================

// c++
#include <string>
#include <list>
#include <vector>
#include <sstream>
#include <fstream>
#include <cassert>
// front
class GameObject2D;

//============================================================================
//	GameStageBuilder class
//============================================================================
class GameStageBuilder {
public:
	//========================================================================
	//	public Methods
	//========================================================================

    GameStageBuilder() = default;
	~GameStageBuilder() = default;

    std::list<GameObject2D*> Create(const std::string& fileName, float stageObjectMapTileSize);
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

    // fileNameの前のパス
    const std::string kCSVPath_ = "Resources/Stage/";

	//--------- functions ----------------------------------------------------

    // helper
    std::vector<std::vector<int>> GetCSVData(const std::string& fileName);
};