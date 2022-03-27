#include "Cuboid.h"


Cuboid::Cuboid(int id, int o) {
	identity = id;
	orientation = o;
}
Cuboid::Cuboid() {
	identity = 0;
	orientation = 0;
}


void Cuboid::changeOrientation(int amount) {
	orientation += amount;
	if (Cuboid::areEdges[identity] && orientation == 2) {
		orientation = 0;
	}
	else if (orientation > 2) {
		orientation -= 3;
	}
}

Cuboid Cuboid::copy() {
	return Cuboid(identity, orientation);
}

bool Cuboid::areEqual(Cuboid x, Cuboid y) {
	if (x.identity == y.identity && x.orientation == y.orientation) {
		return true;
	}
	return false;
}