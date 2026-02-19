#pragma once
#include "Config.h"
#include "LightningGenerator.h"
#include "LightningRenderer.h"

#include <vector>
#include <iostream>
#include <random>
#include <chrono>
#include <string>

class Lightning
{
public:
	Lightning(Config* config)
	{
		configuration = config;

		generator.setConfig(configuration);
	};

	~Lightning()
	{

	};


	void regenLightning();

	std::vector<LightningCell>* getLightningPointsPtr() {
		return generator.getLightningPointsPtr();
	}
private:
	Config * configuration;
	LightningGenerator generator;
};
