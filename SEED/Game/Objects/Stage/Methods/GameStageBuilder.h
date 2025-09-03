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
class StageObjectComponent;

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

    // CSVファイルからオブジェクトを構築するs
    std::list<GameObject2D*> CreateFromCSVFile(const std::string& fileName, float tileSize);

    // 境界線で線対称にホログラムオブジェクトを構築する
    std::list<GameObject2D*> CreateFromBorderLine(std::list<GameObject2D*> objects,
        float axisX, float playerY, int direction, float tileSize);
private:
    //========================================================================
    //	private Methods
    //========================================================================

    //--------- functions ----------------------------------------------------

    // helper
    std::vector<std::vector<int>> GetCSVData(const std::string& fileName);
};