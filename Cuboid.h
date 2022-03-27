#pragma once
#include <stdint.h>

class Cuboid
{
public:
	static constexpr const bool areEdges[] = { false, true, false, true, true, false, true, false, true, true, true, true, false, true, false, true, true, false, true, false };
	uint8_t identity : 5;
	uint8_t orientation : 3;
	void changeOrientation(int amount);
	Cuboid(int id, int orientation);
	Cuboid();
	Cuboid copy();
	static bool areEqual(Cuboid x, Cuboid y);
};

