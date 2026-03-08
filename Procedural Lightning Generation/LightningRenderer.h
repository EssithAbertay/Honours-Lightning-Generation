#pragma once
#include <vector>

#include "raylib.h"
#include "raymath.h"

#include "LightningCell.h"
#include "Config.h"


class LightningRenderer
{
public:
	LightningRenderer(std::vector<LightningCell>* lightning_points, Config* config, Camera3D* camera) : lightning_points(lightning_points), configuration(config), camera(camera) {};
	void render();
private:
	std::vector<LightningCell> * lightning_points;
	Config* configuration;
	Camera3D* camera;

	int segment_size = 4; 

	int x_size = 5, y_size = 5, z_size = 5;
};

