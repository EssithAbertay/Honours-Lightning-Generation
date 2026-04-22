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
#include <algorithm>

#include <sycl/sycl.hpp>

namespace kernels
{
	struct gridstepKernel;
}

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
	float calculateLaplaceNoBorders(int x_pos, int y_pos, int z_pos);

	bool calculateGridStep();

	bool calculateGridStep_multithread();

	struct candidate_cell // struct of a candidate cell and comparator for them
	{
		int x, y, z, parent_x, parent_y, parent_z;
		float potential, probability;
		float distance_weight;

		bool operator==(const candidate_cell& other) const {
			return x == other.x &&
				y == other.y &&
				z == other.z;
		}
	};

	inline int index(int x, int y, int z) // flattening function for 3d vector to turn into 1d
	{
		return x + (y * configuration->x_size) + z * configuration->x_size * configuration->y_size;
	}

	std::vector<float> potentials_flat;
	std::vector<float> new_potentials_flat;
	std::vector<float> starting_flat; // todo: why float? int no?

	std::unordered_set<int> visited_lightning;

	std::mt19937 gen{ std::random_device{}() };

	std::vector<LightningCell> lightning_points;

	std::vector<candidate_cell> candidates;

	Config * configuration = nullptr;

	int max_laplace_loops = 50;

	bool reached_edge = false; // todo: shouldnt be global

	int grid_steps_made = 0;

	float tolerance = 0;

	// multithreading

	// pointer to what laplace function is being used, cause it's always the same one now, pretty much deprecated
	float (LightningGenerator::*laplace_func)(int, int, int);

	
	sycl::queue q;
};