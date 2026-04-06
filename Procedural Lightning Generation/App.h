#pragma once
#include "Controller.h"
#include "Config.h"
#include "Lightning.h"
#include "LightningRenderer.h"
#include "TestData.h"

#include <fstream>
#include <iostream>
#include <iomanip>

class App
{
public:
	App()
	{
		lightning.setConfig(&lightning_config);
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

	Lightning lightning;

	LightningRenderer renderer = LightningRenderer(lightning.getLightningPointsPtr(), &lightning_config, &camera);

	std::vector<TestData> tests;

	inline int index(int x, int z) // this function should really be in config or smthn, but crunch time
	{
		return x + (z * lightning_config.x_size);
	}
};

