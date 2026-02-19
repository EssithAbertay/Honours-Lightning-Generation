#pragma once
#include "Controller.h"
#include "Config.h"
#include "Lightning.h"
#include "LightningRenderer.h"
#include <fstream>
#include <iostream>
#include <iomanip>

class App
{
public:
	App()
	{

	};

	~App()
	{

	};

	void Run(); // public "game"loop call, it's not really a game is it?
private:
	void Update();
	void Render();

	Controller imgui_controller;
	Config lightning_config;

	Camera3D camera;

	Lightning lightning = Lightning(&lightning_config);

	LightningRenderer renderer = LightningRenderer(lightning.getLightningPointsPtr(), &lightning_config, &camera);

};

