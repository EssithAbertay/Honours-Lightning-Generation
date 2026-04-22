#pragma once
#include <map>
#include <vector>


struct TestConditions // struct to store conditions of test
{
	int x_size, y_size, z_size;
	int eta;
	int grid_steps;
	bool candidates_from_air;
	bool multithreading_enabled;
	bool resetting_volume;
	float gradient_tolerance;
	bool loop_cap_enabled;
	int max_loops;
	bool using_target;
	int target_x;
	int target_z;
	float target_weight;
};

struct TestData
{
	TestConditions conditions;

	float time; // time in ms
	int grid_steps; // number of grid steps taken during generation
	int number_of_segments; // number of segments lightning is
};