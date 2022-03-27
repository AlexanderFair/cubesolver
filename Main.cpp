#include "Cube.h"
#include "Move.h"
#include "Solver.h"
#include <iostream>
#include "Cuboid.h"


int main() {
	
	
	//Solver::generateBottomEdgeTable();
	//int x[2] = { 1, 2 };
	//std::cout << sizeof(x) << std::endl;
	//Cuboid c = Cuboid();
	//std::cout << sizeof(c) << std::endl;

	Cube cube = Cube();
	//Solver::solve(cube);
	////std::cout << Cube::cornersAsStringFirstFixed(cube) << std::endl;
	////std::cout << sizeof(cube.moveFunctions) << std::endl;
	////Move move = Move(cube, 100, 1000);
	////std::cout << sizeof(move);
	cube.print();

	while (true) {
		cube.doMove();
	}
	int x;
	std::cin >> x;
	
	return 0;
}