#pragma once
#include "Cube.h"
#include <vector>

class Move
{
public:
	Cube cube;
	int lastMove : 8;
	int secondLastMove : 8;
	std::vector<int> path;
	Move(Cube cu, int l, int sl);
	Move copy();
	void makeMove(int moveID);
};