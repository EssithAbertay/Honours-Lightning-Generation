#pragma once
#include "LightningCell.h"
#include "Config.h"
#include "LightningVolume.h"

#include <vector>
#include <iostream>
#include <random>
#include <chrono>
#include <string>

#include <unordered_set>

class LightningGenerator
{
public:
	LightningGenerator();

	void regenLightning_unoptimised();
	void regenLightning_optimised();
	void regenLightning_multithread();
	void regenLightning_other();

	void setConfig(Config* config)
	{
		configuration = config;
	}

	std::vector<LightningCell>* getLightningPointsPtr() {
		return &lightning_points;
	}

	int getGridSteps() { return grid_steps_made; }
private:

	void createStartingGrid();
	void initialiseGrid();

	void performLightningStep();
	void performLightningStep_optimised();

	void collectCandidates();
	void collectCandidates_optimised();
	
	void checkCandidacy(int x_pos, int y_pos, int z_pos);

	void selectLightningCell();
	
	void resetPotentialGrid();
	
	float calculateLaplace(int x_pos, int y_pos, int z_pos);
	bool calculateGridStep();

	struct candidate_cell
	{
		int x, y, z, parent_x, parent_y, parent_z;
		float potential, probability;

		bool operator==(const candidate_cell& other) const {
			return x == other.x &&
				y == other.y &&
				z == other.z;
		}
	};



	std::mt19937 gen{ std::random_device{}() };

	std::vector<LightningCell> lightning_points;

	std::vector<std::vector<std::vector<float>>> potentials;

	std::vector<std::vector<std::vector<float>>> new_potentials;

	std::vector<std::vector<std::vector<int>>> starting;

	std::vector<candidate_cell> candidates;

	Config * configuration = nullptr;

	const int LIGHTNING_STEPS = 30;
	int MAX_GRADIENT_LAPLACE_LOOPS = 50; // todo: shouldnt be all caps

	bool reached_edge = false; // todo: shouldnt be global

	int grid_steps_made = 0;
};

