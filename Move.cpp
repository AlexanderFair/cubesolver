#include "Move.h"


Move::Move(Cube cu, int l, int sl) {
	this->cube = cu;
	this->lastMove = l;
	this->secondLastMove = sl;
	this->path;
}

Move Move::copy() {
	Move newMove = Move(cube.copy(), lastMove, secondLastMove);
	newMove.path = path;
	return newMove;
}

void Move::makeMove(int moveID) {
	cube.makeAMoveBasedOnI(moveID);
	Cube::statesVisited++;
	path.push_back(moveID);
	secondLastMove = lastMove;
	lastMove = moveID;
}