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

	float time;
	int grid_steps;
	int number_of_segments;
};