#pragma once

struct LightningCell
{
	LightningCell(int x, int y, int z, int px, int py, int pz) : x(x), y(y), z(z), parent_x(px), parent_y(py), parent_z(pz) {}

	int x, y, z, parent_x, parent_y, parent_z;
};