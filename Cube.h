#pragma once
#include "Cuboid.h"
#include <string>
#include <functional>
#include <vector>
using std::string;

class Cube {
public:
	static int random(int max);
	
	Cuboid cube[20];
	void print();
	void doMove();
	Cube();
	Cube(Cuboid* cube);

	void umove();
	void u2move();
	void uprime();
	
	void dmove();
	void d2move();
	void dprime();
	
	void fmove();
	void f2move();
	void fprime();
	
	void bmove();
	void b2move();
	void bprime();
	
	void rmove();
	void r2move();
	void rprime();
	
	void lmove();
	void l2move();
	void lprime();
	void makeAMoveBasedOnI(int i);
	static string getMoveByNumber(int move);
	void (Cube::* moveFunctions[18])() = {
		&Cube::umove,
		&Cube::u2move,
		&Cube::uprime,
		
		&Cube::dmove,
		&Cube::d2move,
		&Cube::dprime,

		& Cube::fmove,
		& Cube::f2move,
		& Cube::fprime,

		& Cube::bmove,
		& Cube::b2move,
		& Cube::bprime,

		& Cube::lmove,
		& Cube::l2move,
		& Cube::lprime,

		& Cube::rmove,
		& Cube::r2move,
		& Cube::rprime,
	};
	static long statesVisited;
	string convertToString();
	static string cornersAsString(Cube cube);
	static string cornersAsStringFirstFixed(Cube cube);
	Cube copy();
	bool isSolved();
	static int getIndexOfCorners(Cube cube);
private:
	//static const int totalNumOfStates = 88179840;
	static bool solveWhenSolutionFound;
	static void printChar(char c);
	static void sortCname(char* name);
	static void sortEname(char* name);
	void scramble(int secondLastMove, int lastMove, int amount);
};
