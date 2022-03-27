#include<iostream>
#include<map>
#include<windows.h>
#include"Cube.h"
#include <ctime>
#include "Move.h"
#include <stack>
#include "Solver.h"

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::map;
using::std::system;

bool Cube::solveWhenSolutionFound = false;
long Cube::statesVisited;
Cube::Cube() {
	for (int i = 0; i < 20; i++) {
		cube[i].identity = i;
		cube[i].orientation = 0;
	}
}

Cube::Cube(Cuboid* newCube) {
	for (int i = 0; i < 20; i++) {
		cube[i] = newCube[i];
	}
}
namespace Precomputed {
	//dimensions are positon on cube, id of cuboid, orientation, and then the cuboids of edges that it would be an adjacent piece

	const int cornerPoses[8] = { 0, 2, 5, 7, 12, 14, 17, 19 };
	const int edgesPoses[12] = { 1, 3, 4, 6, 8, 9, 10, 11, 13, 15, 16, 18 };
	const int indexByID[20] = { 0, 0, 1, 1, 2, 2, 3, 3, 4, 5, 6, 7, 4, 8, 5, 9, 10, 6, 11, 7 };
	const int edgesAdjecntToCornersBasedOnPosition[8][3] = {
		{3, 11, 1},
		{1, 10, 4},
		{6, 8, 3},
		{4, 9, 6},
		{15, 8, 13},
		{13, 9, 16},
		{18, 11, 15},
		{16, 10, 18}
	};
	const int cornerHashOrder[8][3][8] = {
		//pos 0
		{
			{0, 2, 5, 7, 12, 14, 17, 19},
			{0, 5, 17, 12, 19, 14, 2, 7},
			{0, 17, 2, 19, 7, 14, 5, 12}
		},
		//pos 2
		{
			{2, 7, 0, 5, 17, 12, 19, 14},
			{2, 0, 19, 17, 14, 12, 7, 5},
			{2, 19, 7, 14, 5, 12, 0, 17}
		},
		//pos 5
		{
			{5, 0, 7, 2, 14, 19, 12, 17},
			{5, 7, 12, 14, 17, 19, 0, 2},
			{5, 12, 0, 17, 2, 19, 7, 14}
		},
		//pos 7
		{
			{7, 5, 2, 0, 19, 17, 14, 12},
			{7, 2, 14, 19, 12, 17, 5, 0},
			{7, 14, 5, 12, 0, 17, 2, 19}
		},
		//pos 12
		{
			{12, 14, 17, 19, 0, 2, 5, 7},
			{12, 17, 5, 0, 7, 2, 14, 19},
			{12, 5, 14, 7, 19, 2, 17, 0}
		},
		//pos 14
		{
			{14, 19, 12, 17, 5, 0, 7, 2},
			{14, 12, 7, 5, 2, 0, 19, 17},
			{14, 7, 19, 2, 17, 0, 12, 5}
		},
		//pos 17
		{
			{17, 12, 19, 14, 2, 7, 0, 5},
			{17, 19, 0, 2, 5, 7, 12, 14},
			{17, 0, 12, 5, 14, 7, 19, 2}
		},
		//pos 19
		{
			{19, 17, 14, 12, 7, 5, 2, 0},
			{19, 14, 2, 7, 0, 5, 17, 12},
			{19, 2, 17, 0, 12, 5, 14, 7}
		},
	};
	const int cornerOrientaionChanges[3][8] = {
		{0, 0, 0, 0, 0, 0, 0, 0},
		{2, 1, 1, 2, 2, 1, 1, 2},
		{1, 2, 2, 1, 1, 2, 2, 1}
	};
}

void Cube::umove() {
	Cuboid temp = cube[0];
	cube[0] = cube[5];
	cube[5] = cube[7];
	cube[7] = cube[2];
	cube[2] = temp;
	temp = cube[1];
	cube[1] = cube[3];
	cube[3] = cube[6];
	cube[6] = cube[4];
	cube[4] = temp;
}
void Cube::u2move() {
	Cuboid temp = cube[0];
	cube[0] = cube[7];
	cube[7] = temp;
	temp = cube[2];
	cube[2] = cube[5];
	cube[5] = temp;
	temp = cube[1];
	cube[1] = cube[6];
	cube[6] = temp;
	temp = cube[3];
	cube[3] = cube[4];
	cube[4] = temp;
}
void Cube::uprime() {
	Cuboid temp = cube[0];
	cube[0] = cube[2];
	cube[2] = cube[7];
	cube[7] = cube[5];
	cube[5] = temp;
	temp = cube[1];
	cube[1] = cube[4];
	cube[4] = cube[6];
	cube[6] = cube[3];
	cube[3] = temp;
}

void Cube::dmove() {
	Cuboid temp = cube[12];
	cube[12] = cube[17];
	cube[17] = cube[19];
	cube[19] = cube[14];
	cube[14] = temp;
	temp = cube[13];
	cube[13] = cube[15];
	cube[15] = cube[18];
	cube[18] = cube[16];
	cube[16] = temp;
}
void Cube::d2move() {
	Cuboid temp = cube[12];
	cube[12] = cube[19];
	cube[19] = temp;
	temp = cube[14];
	cube[14] = cube[17];
	cube[17] = temp;
	temp = cube[13];
	cube[13] = cube[18];
	cube[18] = temp;
	temp = cube[15];
	cube[15] = cube[16];
	cube[16] = temp;
}
void Cube::dprime() {
	Cuboid temp = cube[12];
	cube[12] = cube[14];
	cube[14] = cube[19];
	cube[19] = cube[17];
	cube[17] = temp;
	temp = cube[13];
	cube[13] = cube[16];
	cube[16] = cube[18];
	cube[18] = cube[15];
	cube[15] = temp;
}

void Cube::fmove() {
	cube[5].changeOrientation(1);
	cube[7].changeOrientation(2);
	cube[12].changeOrientation(2);
	cube[14].changeOrientation(1);

	Cuboid temp = cube[5];
	cube[5] = cube[12];
	cube[12] = cube[14];
	cube[14] = cube[7];
	cube[7] = temp;

	temp = cube[6];
	cube[6] = cube[8];
	cube[8] = cube[13];
	cube[13] = cube[9];
	cube[9] = temp;
}
void Cube::f2move() {
	Cuboid temp = cube[5];
	cube[5] = cube[14];
	cube[14] = temp;

	temp = cube[7];
	cube[7] = cube[12];
	cube[12] = temp;

	temp = cube[6];
	cube[6] = cube[13];
	cube[13] = temp;

	temp = cube[8];
	cube[8] = cube[9];
	cube[9] = temp;
}
void Cube::fprime() {
	cube[5].changeOrientation(1);
	cube[7].changeOrientation(2);
	cube[12].changeOrientation(2);
	cube[14].changeOrientation(1);

	Cuboid temp = cube[5];
	cube[5] = cube[7];
	cube[7] = cube[14];
	cube[14] = cube[12];
	cube[12] = temp;

	temp = cube[6];
	cube[6] = cube[9];
	cube[9] = cube[13];
	cube[13] = cube[8];
	cube[8] = temp;
}

void Cube::bmove() {
	cube[0].changeOrientation(2);
	cube[2].changeOrientation(1);
	cube[17].changeOrientation(1);
	cube[19].changeOrientation(2);

	Cuboid temp = cube[2];
	cube[2] = cube[19];
	cube[19] = cube[17];
	cube[17] = cube[0];
	cube[0] = temp;

	temp = cube[1];
	cube[1] = cube[10];
	cube[10] = cube[18];
	cube[18] = cube[11];
	cube[11] = temp;
}
void Cube::b2move() {
	Cuboid temp = cube[0];
	cube[0] = cube[19];
	cube[19] = temp;
	
	temp = cube[2];
	cube[2] = cube[17];
	cube[17] = temp;

	temp = cube[1];
	cube[1] = cube[18];
	cube[18] = temp;

	temp = cube[11];
	cube[11] = cube[10];
	cube[10] = temp;
}
void Cube::bprime() {
	cube[0].changeOrientation(2);
	cube[2].changeOrientation(1);
	cube[17].changeOrientation(1);
	cube[19].changeOrientation(2);

	Cuboid temp = cube[0];
	cube[0] = cube[17];
	cube[17] = cube[19];
	cube[19] = cube[2];
	cube[2] = temp;

	temp = cube[1];
	cube[1] = cube[11];
	cube[11] = cube[18];
	cube[18] = cube[10];
	cube[10] = temp;
}

void Cube::lmove() {
	cube[3].changeOrientation(1);
	cube[8].changeOrientation(1);
	cube[11].changeOrientation(1);
	cube[15].changeOrientation(1);

	cube[5].changeOrientation(2);
	cube[0].changeOrientation(1);
	cube[17].changeOrientation(2);
	cube[12].changeOrientation(1);

	Cuboid temp = cube[0];
	cube[0] = cube[17];
	cube[17] = cube[12];
	cube[12] = cube[5];
	cube[5] = temp;

	temp = cube[3];
	cube[3] = cube[11];
	cube[11] = cube[15];
	cube[15] = cube[8];
	cube[8] = temp;
}
void Cube::l2move() {
	Cuboid temp = cube[0];
	cube[0] = cube[12];
	cube[12] = temp;

	temp = cube[5];
	cube[5] = cube[17];
	cube[17] = temp;

	temp = cube[8];
	cube[8] = cube[11];
	cube[11] = temp;
		
	temp = cube[3];
	cube[3] = cube[15];
	cube[15] = temp;
}
void Cube::lprime() {
	cube[3].changeOrientation(1);
	cube[8].changeOrientation(1);
	cube[11].changeOrientation(1);
	cube[15].changeOrientation(1);

	cube[5].changeOrientation(2);
	cube[0].changeOrientation(1);
	cube[17].changeOrientation(2);
	cube[12].changeOrientation(1);

	Cuboid temp = cube[0];
	cube[0] = cube[5];
	cube[5] = cube[12];
	cube[12] = cube[17];
	cube[17] = temp;

	temp = cube[3];
	cube[3] = cube[8];
	cube[8] = cube[15];
	cube[15] = cube[11];
	cube[11] = temp;
}

void Cube::rmove() {
	cube[4].changeOrientation(1);
	cube[9].changeOrientation(1);
	cube[10].changeOrientation(1);
	cube[16].changeOrientation(1);

	cube[2].changeOrientation(2);
	cube[7].changeOrientation(1);
	cube[14].changeOrientation(2);
	cube[19].changeOrientation(1);

	Cuboid temp = cube[2];
	cube[2] = cube[7];
	cube[7] = cube[14];
	cube[14] = cube[19];
	cube[19] = temp;

	temp = cube[4];
	cube[4] = cube[9];
	cube[9] = cube[16];
	cube[16] = cube[10];
	cube[10] = temp;
}
void Cube::r2move() {
	Cuboid temp = cube[2];
	cube[2] = cube[14];
	cube[14] = temp;

	temp = cube[7];
	cube[7] = cube[19];
	cube[19] = temp;

	temp = cube[4];
	cube[4] = cube[16];
	cube[16] = temp;

	temp = cube[9];
	cube[9] = cube[10];
	cube[10] = temp;

}
void Cube::rprime() {
	cube[4].changeOrientation(1);
	cube[9].changeOrientation(1);
	cube[10].changeOrientation(1);
	cube[16].changeOrientation(1);

	cube[2].changeOrientation(2);
	cube[7].changeOrientation(1);
	cube[14].changeOrientation(2);
	cube[19].changeOrientation(1);

	Cuboid temp = cube[2];
	cube[2] = cube[19];
	cube[19] = cube[14];
	cube[14] = cube[7];
	cube[7] = temp;
		
	temp = cube[4];
	cube[4] = cube[10];
	cube[10] = cube[16];
	cube[16] = cube[9];
	cube[9] = temp;
}


void Cube::print() {
	//convert cube into a char* and print it out
	char carr[6][9];
	//this array is if it is orientated correctly
	string colourByID[20] = {
		"wob",
		"wb",
		"wbr",	
		"wo",
		"wr",
		"wgo",
		"wg",
		"wrg",
		"og",
		"rg",
		"rb",
		"ob",
		"yog",
		"yg",
		"ygr",
		"yo",
		"yr",
		"ybo",
		"yb",
		"yrb"
	};

	/*
	for (int i = 0; i <= 19; i++) {
		cout << i << ": " << int(cube[i].identity) << " " << int(cube[i].orientation) << endl;
	}
	*/
	

	int cubeIndicesOfCorners[8] = {0, 2, 5, 7, 12, 14, 17, 19};
	int cubeCornerSections[8][6] = {
		{0, 0, 4, 0, 3, 2},
		{0, 2, 3, 0, 2, 2},
		{0, 6, 1, 0, 4, 2},
		{0, 8, 2, 0, 1, 2},
		{5, 0, 4, 8, 1, 6},
		{5, 2, 1, 8, 2, 6},
		{5, 6, 3, 8, 4, 6},
		{5, 8, 2, 8, 3, 6},
	};
	for (int cornerNum = 0; cornerNum < 8; cornerNum++) {
		Cuboid thisCuboid = cube[cubeIndicesOfCorners[cornerNum]];
		const char* colours = colourByID[thisCuboid.identity].c_str();
		if (thisCuboid.orientation == 1) {
			//NEXT ERROR PROBABLY IN VALUES HERE
			char newcolours[3];
			newcolours[0] = colours[2];
			newcolours[1] = colours[0];
			newcolours[2] = colours[1];
			colours = newcolours;
		}
		else if (thisCuboid.orientation == 2) {
			//NEXT ERROR PROBABLY IN VALUES HERE
			char newcolours[3];
			newcolours[0] = colours[1];
			newcolours[1] = colours[2];
			newcolours[2] = colours[0];
			colours = newcolours;
		}
		//put colours into char*
		carr[cubeCornerSections[cornerNum][0]][cubeCornerSections[cornerNum][1]] = colours[0];
		carr[cubeCornerSections[cornerNum][2]][cubeCornerSections[cornerNum][3]] = colours[1];
		carr[cubeCornerSections[cornerNum][4]][cubeCornerSections[cornerNum][5]] = colours[2];
	}


	//edges now

	int cubeIndicesOfEdges[12] = { 1, 3, 4, 6, 8, 9, 10, 11, 13, 15, 16, 18 };
	int cubeEdgeSections[12][4] = {
		{0, 1, 3, 1},
		{0, 3, 4, 1},
		{0, 5, 2, 1},
		{0, 7, 1, 1},
		{4, 5, 1, 3},
		{2, 3, 1, 5},
		{2, 5, 3, 3},
		{4, 3, 3, 5},
		{5, 1, 1, 7},
		{5, 3, 4, 7},
		{5, 5, 2, 7},
		{5, 7, 3, 7},
	};

	for (int edgeNum = 0; edgeNum < 12; edgeNum++) {
		Cuboid thisCuboid = cube[cubeIndicesOfEdges[edgeNum]];
		const char* colours = colourByID[thisCuboid.identity].c_str();
			
		if (thisCuboid.orientation) {
			char newcolours[2];
			newcolours[0] = colours[1];
			newcolours[1] = colours[0];
			colours = newcolours;
		}

		//put colours into char*
		carr[cubeEdgeSections[edgeNum][0]][cubeEdgeSections[edgeNum][1]] = colours[0];
		carr[cubeEdgeSections[edgeNum][2]][cubeEdgeSections[edgeNum][3]] = colours[1];
	}


	//put in the centers
	carr[0][4] = 'w';
	carr[1][4] = 'g';
	carr[2][4] = 'r';
	carr[3][4] = 'b';
	carr[4][4] = 'o';
	carr[5][4] = 'y';


	//white face
	cout << "        ";
	Cube::printChar(carr[0][0]);
	Cube::printChar(carr[0][1]);
	Cube::printChar(carr[0][2]);
	cout << endl;

	cout << "        ";
	Cube::printChar(carr[0][3]);
	Cube::printChar(carr[0][4]);
	Cube::printChar(carr[0][5]);
	cout << endl;
	
	cout << "        ";
	Cube::printChar(carr[0][6]);
	Cube::printChar(carr[0][7]);
	Cube::printChar(carr[0][8]);
	cout << endl << endl;
	
	//orange to blue face
	Cube::printChar(carr[4][0]);
	Cube::printChar(carr[4][1]);
	Cube::printChar(carr[4][2]);
	cout << "  ";
	Cube::printChar(carr[1][0]);
	Cube::printChar(carr[1][1]);
	Cube::printChar(carr[1][2]);
	cout << "  ";
	Cube::printChar(carr[2][0]);
	Cube::printChar(carr[2][1]);
	Cube::printChar(carr[2][2]);
	cout << "  ";
	Cube::printChar(carr[3][0]);
	Cube::printChar(carr[3][1]);
	Cube::printChar(carr[3][2]);
	cout << endl;
	Cube:://second line of o-b
	Cube::printChar(carr[4][3]);
	Cube::printChar(carr[4][4]);
	Cube::printChar(carr[4][5]);
	cout << "  ";
	Cube::printChar(carr[1][3]);
	Cube::printChar(carr[1][4]);
	Cube::printChar(carr[1][5]);
	cout << "  ";
	Cube::printChar(carr[2][3]);
	Cube::printChar(carr[2][4]);
	Cube::printChar(carr[2][5]);
	cout << "  ";
	Cube::printChar(carr[3][3]);
	Cube::printChar(carr[3][4]);
	Cube::printChar(carr[3][5]);
	cout << endl;
	//third line
	Cube::printChar(carr[4][6]);
	Cube::printChar(carr[4][7]);
	Cube::printChar(carr[4][8]);
	cout << "  ";
	Cube::printChar(carr[1][6]);
	Cube::printChar(carr[1][7]);
	Cube::printChar(carr[1][8]);
	cout << "  ";
	Cube::printChar(carr[2][6]);
	Cube::printChar(carr[2][7]);
	Cube::printChar(carr[2][8]);
	cout << "  ";
	Cube::printChar(carr[3][6]);
	Cube::printChar(carr[3][7]);
	Cube::printChar(carr[3][8]);
	cout << endl << endl;
	
	//yellow face
	cout << "        ";
	Cube::printChar(carr[5][0]);
	Cube::printChar(carr[5][1]);
	Cube::printChar(carr[5][2]);
	cout << endl;
	
	cout << "        ";
	Cube::printChar(carr[5][3]);
	Cube::printChar(carr[5][4]);
	Cube::printChar(carr[5][5]);
	cout << endl;
	
	cout << "        ";
	Cube::printChar(carr[5][6]);
	Cube::printChar(carr[5][7]);
	Cube::printChar(carr[5][8]);
	cout << endl << endl;
	//cout << "strd corners: " << endl << cornersAsStringFirstFixed(*this) << endl;
	//cout << "index in corner table " << getIndexOfCorners(*this) << endl;
}

void Cube::printChar(char c) {
	HANDLE hConsoleColor = GetStdHandle(STD_OUTPUT_HANDLE);

	switch (c) {
	case 'w':
		SetConsoleTextAttribute(hConsoleColor, 255);
		break;
	case 'g':
		SetConsoleTextAttribute(hConsoleColor, 170);
		break;
	case 'r':
		SetConsoleTextAttribute(hConsoleColor, 68);
		break;
	case 'b':
		SetConsoleTextAttribute(hConsoleColor, 153);
		break;
	case 'o':
		SetConsoleTextAttribute(hConsoleColor, 204);
		break;
	case 'y':
		SetConsoleTextAttribute(hConsoleColor, 102);
		break;
	default:
		break;
	}
	cout << c << ' ';
	SetConsoleTextAttribute(hConsoleColor, 15);
}

void Cube::sortCname(char* name) {
	if (name[0] > name[1]) {
		char temp = name[0];
		name[0] = name[1];
		name[1] = temp;
	}

	if (name[1] > name[2]) {
		char temp = name[1];
		name[1] = name[2];
		name[2] = temp;
	}

	if (name[0] > name[1]) {
		char temp = name[0];
		name[0] = name[1];
		name[1] = temp;
	}
}
void Cube::sortEname(char* name) {
	if (name[0] > name[1]) {
		char temp = name[0];
		name[0] = name[1];
		name[1] = temp;
	}
}


void Cube::doMove() {
	cout << "0   :  R	10  :  L	20  :  U	30  :  D	40  :  F	50  :  B	70  :  Scramble" << endl;
	cout << "1   :  R2	11  :  L2	21  :  U2	31  :  D2	41  :  F2	51  :  B2	80  :  Reset" << endl;
	cout << "2   :  R'	12  :  L'	22  :  U'	32  :  D'	42  :  F'	52  :  B'	90  :  Solve	" << endl << "100  : Toggle Solve When Solution Found" << endl;
	string solution = "";
	vector<int> sol;
	int in = 0;
	std::cin >> in;
	switch (in) {
	case 0:
		this->rmove();
		break;
	case 1:
		this->r2move();
		break;
	case 2:
		this->rprime();
		break;
	case 10:
		this->lmove();
		break;
	case 11:
		this->l2move();
		break;
	case 12:
		this->lprime();
		break;
	case 20:
		this->umove();
		break;
	case 21:
		this->u2move();
		break;
	case 22:
		this->uprime();
		break;
	case 30:
		this->dmove();
		break;
	case 31:
		this->d2move();
		break;
	case 32:
		this->dprime();
		break;
	case 40:
		this->fmove();
		break;
	case 41:
		this->f2move();
		break;
	case 42:
		this->fprime();
		break;
	case 50:
		this->bmove();
		break;
	case 51:
		this->b2move();
		break;
	case 52:
		this->bprime();
		break;
	case 55:
		int whichToChange;
		std::cin >> whichToChange;
		cube[whichToChange].changeOrientation(1);
		break;
	case 70:
		cout << "Enter the depth to scramble" << endl;
		int amountToScramble;
		std::cin >> amountToScramble;
		this->scramble(100, 1000, amountToScramble);
		cout << endl;
		break;
	case 80:
		for (int i = 0; i < 20; i++) {
			cube[i].identity = i;
			cube[i].orientation = 0;
		}
		break;
	case 90:		

		sol = Solver::solve(*this);
		cout << endl;
		if (sol.empty()) {
			cout << "Already Solved!" << endl;
		}
		else if (sol.at(0) == -1) {
			cout << "Couldn't find the solution!" << endl;
		}
		else {
			cout << "Soltuion:";
			for (int i : sol) {
				cout << ' ' << getMoveByNumber(i);
			}
			cout << endl;

			if (Cube::solveWhenSolutionFound) {
				for (int i : sol) {
					this->print();
					cout << Cube::getMoveByNumber(i) << endl;
					this->makeAMoveBasedOnI(i);
				}
			}

			cout << "Soltuion:";
			for (int i : sol) {
				cout << ' ' << getMoveByNumber(i);
			}
			cout << endl;
		}
		break;
	case 100:
		solveWhenSolutionFound ^= true;
		break;
	default:
		cout << "invalid input" << endl;
	}
	this->print();
}

Cube Cube::copy() {
	Cuboid newCubeArr[20];
	for (int i = 0; i <= 19; i++) {
		newCubeArr[i] = cube[i].copy();
	}
	Cube newCube = Cube(newCubeArr);
	return newCube;
}

bool Cube::isSolved() {
	//skip last two as placement and orientaion are both forced
	for (int i = 0; i < 18; i++) {
		if (cube[i].identity != i || cube[i].orientation != 0) {
			return false;
		}
	}
	return true;
}

void Cube::makeAMoveBasedOnI(int i) {
	(this->*moveFunctions[i])();
}

string Cube::getMoveByNumber(int move) {
	switch (move) {
	case 0:
		return "U";
	case 1:
		return "U2";
	case 2:
		return "U'";
	case 3:
		return "D";
	case 4:
		return "D2";
	case 5:
		return "D'";
	case 6:
		return "F";
	case 7:
		return "F2";
	case 8:
		return "F'";
	case 9:
		return "B";
	case 10:
		return "B2";
	case 11:
		return "B'";
	case 12:
		return "L";
	case 13:
		return "L2";
	case 14:
		return "L'";
	case 15:
		return "R";
	case 16:
		return "R2";
	case 17:
		return "R'";
	case 100:
		"Already was solved!";
	default:
		return "that aint a move!";
	}
	return "that aint a move!";
}

void Cube::scramble(int secondLastMove, int lastMove, int amount) {
	if (amount == 0) {
		return;
	}
	
	vector<int> possibleMoves;

	bool doOppositeMove = lastMove / 6 != secondLastMove / 6;
	for (int i = 0; i < 18; i++) {
		bool isSameTypeAsLastMove = lastMove / 3 == i / 3;
		bool isOppositeLayerOfLastMove = lastMove / 6 == i / 6;
		if (!isSameTypeAsLastMove && (!isOppositeLayerOfLastMove || doOppositeMove)) {
			possibleMoves.push_back(i);
		}
	}
	/*
	for (int i = 0; i < possibleMoves.size(); i++) {
		cout << possibleMoves.at(i) << endl;
	}
	*/


	int choice = random(int(possibleMoves.size()));
	int moveMade = possibleMoves.at(choice);
	makeAMoveBasedOnI(moveMade);
	cout << getMoveByNumber(moveMade) << " ";
	scramble(lastMove, moveMade, amount - 1);
}

int Cube::random(int max) {
	static bool first = true;
	if (first)
	{
		srand(int(time(NULL))); //seeding for the first time only!
		first = false;
	}
	return rand() % max;
}

string Cube::convertToString() {
	string out = "";
	//omit last two cuboies as both placement and orienation are known due to 9other pieces known
	for (int i = 0; i < 18; i++) {
		//cout << (*(int*)&cube[i]) << " i: " << i << endl;
		out += *(char*)&cube[i]; // fast inverse square root "evil floating bit hack"
	}
	//cout << endl << endl;
	return out;
}

string Cube::cornersAsString(Cube cube) {
	string out = "";
	for (int i = 0; i < 7; i++) {
		out += *(char*)&cube.cube[Precomputed::cornerPoses[i]];
	}
	return out;
}

string Cube::cornersAsStringFirstFixed(Cube cube) {
	//find pos of white orange blue corner (id 0 corner)
	int pos = 0;
	while (cube.cube[pos].identity != 0) {
		pos++;
	}

	string out;
	//get the return string
	int index = Precomputed::indexByID[pos];
	//if the orientaion isnt 0, the orietaion of the other corners all have to change too
	//if it is 0, then just add the cuboid into without change orietaion - might have speed increase because it only checks now and doesnt run the change orietaion fucntion? idk
	if (cube.cube[pos].orientation) {
		for (int i = 1; i < 7; i++) {
			Cuboid thisCorner = cube.cube[Precomputed::cornerHashOrder[index][cube.cube[pos].orientation][i]].copy();
			thisCorner.changeOrientation(Precomputed::cornerOrientaionChanges[cube.cube[pos].orientation][i]);
			out += *(char*)&thisCorner; // fast inverse square root "evil floating bit hack"
		}
	}
	else {
		for (int i = 1; i < 7; i++) {
			//i love unreadable code
			//is the same as other loop in the if but wihtout changing orienation
			Cuboid thisCuboid = cube.cube[Precomputed::cornerHashOrder[index][cube.cube[pos].orientation][i]].copy();
			out += *(char*)&thisCuboid;
		}
	}

	return out;
}

int Cube::getIndexOfCorners(Cube cube) {
	int totalNumOfStates = 88179840;
	int index = 0;
	int cornersLeft = 8;
	vector<int> cornerNums;
	cornerNums.insert(cornerNums.begin(), { 0, 2, 5, 7, 12, 14, 17, 19 });

	for (int i = 0; i < 7; i++) {
		//cout << "i " << i;
		Cuboid corner = cube.cube[Precomputed::cornerPoses[i]];
		//get the element it is in the sorted list of the corners left
		int cornerNum = 0;
		while (cornerNums.at(cornerNum) != corner.identity) {
			cornerNum++;
		}
		cornerNums.erase(cornerNums.begin() + cornerNum);
		//cout << " corner num " << cornerNum << endl;

		totalNumOfStates /= cornersLeft;
		index += cornerNum * totalNumOfStates;
		//cout << "index now " << index << endl;
		totalNumOfStates /= 3;
		index += corner.orientation * totalNumOfStates;
		//cout << "index now " << index << endl;
		cornersLeft--;
	}

	return index;
}