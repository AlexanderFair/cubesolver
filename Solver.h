#pragma once
#include <unordered_set>
#include "Cube.h"
#include "Move.h"

class Solver
{
public:
	static std::vector<int> solve(Cube cube);
	static void generateCornerTable();
	static void generateTopEdgeTable();
	static void generateBottomEdgeTable();
private:
	static const int MAX_TABLE_DEPTH = 6;
	static bool loadedCornerTable;
	static std::unordered_set<std::string> lookupTable;
	static uint8_t cornerTable[88179840];
	static uint8_t topEdgeTable[510935040];
	static uint8_t bottomEdgeTable[510935040];
	static const int cornerPoses[8];
	static const int edgePoses[12];
	static const int indexByID[20];
	static int statesVisited;
	static int solutionDepth;
	static std::vector<int> getPathToTable(Cube cube);
	static std::vector<int> depthFirstForTable(Move current, int depth, int maxDepth);
	static void inflateStates(int newDepth);
	static void genStates(Move current, int depth, int maxDepth);
	static bool inTable(std::string key);
	static void generateCornerStates(Move move, int depth, int maxDepth);
	static void loadCornerTable();
	static bool inCornerTable(std::string key);
	static int getLowerBound(Cube cube);
	static int getIndexOfCorners(Cube cube);
	static int getIndexOfFirstSevenEdges(Cube cube);
	static int getIndexOfLastSevenEdges(Cube cube);
	static void generateTopEdgeStates(Move move, int depth, int maxDepth);
	static void generateBottomEdgeStates(Move move, int depth, int maxDepth);
	static void loadTableByDepth(int depth);
	
};

