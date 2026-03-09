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

#include <sycl/sycl.hpp>

class LightningGenerator
{
public:
	LightningGenerator();

	void setVars();

	void regenLightning_unoptimised();
	void regenLightning_optimised();

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
	void indivGridStep();

	bool calculateGridStep_multithread();
	void indivGridStep_multithread(int z, int y, int x, bool & is_tolerance);


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

	inline int index(int x, int y, int z)
	{
		return x + (y * configuration->x_size) + z * configuration->x_size * configuration->y_size;
	}

	std::vector<float> potentials_flat;
	std::vector<float> new_potentials_flat;
	std::vector<float> starting_flat;

	std::unordered_set<int> visited_lightning;

	std::mt19937 gen{ std::random_device{}() };

	std::vector<LightningCell> lightning_points;

	std::vector<candidate_cell> candidates;

	Config * configuration = nullptr;

	int max_laplace_loops = 50;

	bool reached_edge = false; // todo: shouldnt be global

	int grid_steps_made = 0;

	float tolerance = 0;
};

