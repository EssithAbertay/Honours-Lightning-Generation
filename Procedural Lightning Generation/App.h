#pragma once
#include "Controller.h"
#include "Config.h"
#include "Lightning.h"
#include "LightningRenderer.h"
#include "TestData.h"

#include <fstream>
#include <iostream>
#include <iomanip>

#include <ctime>
#include <string>
#include <filesystem>

// nlohmann JSON library is used for JSON purposes
// https://github.com/nlohmann/json?tab=coc-ov-file
#include <json.hpp>

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


	// 2d vector flattener index function
	inline int index(int x, int z) // this function should really be in config or smthn, but crunch time
	{
		return x + (z * lightning_config.x_size);
	}

	inline std::string filename(std::string prefix, std::string folder, std::string extension) // helper function to create file names
	{
		std::filesystem::create_directory(folder);

		std::stringstream to_return;
		to_return << folder << "/" << prefix;

		auto t = std::time(nullptr);
		auto tm = *std::localtime(&t);

		to_return << "_";
		to_return << std::put_time(&tm, "%Y-%m-%d_%H-%M-%S");
		to_return << extension;
		return to_return.str();
	}
};

