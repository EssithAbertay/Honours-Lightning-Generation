#pragma once
#include <vector>

enum GENERATION_METHOD
{
	unoptimised,
	optimised,
	multithread,
	other,
};

struct SavedGeneration
{
	int x_size, y_size, z_size;
	float time;
	int eta;

	GENERATION_METHOD method_used;
};

struct Config
{
public:
	int max_dimension = 100;

	int x_size = 5;
	int y_size = 5;
	int z_size = 5;

	bool force_ratio = false;

	bool reset_vol_between_steps = true;

	// if true, select candidates from air cells, if false select from lightning cells, bad name, is unclear
	bool candidates_from_air = true; 

	float gradient_tolerance = 0.005;

	int eta = 1;

	bool is_bounding_box = false;

	bool is_regenerate_this_frame = false;

	GENERATION_METHOD method = unoptimised;
	
	bool is_perform_test = false;

	int dimension_increment = 5;
	int number_to_average = 5;

	std::vector<float> times;
	float min_time = std::numeric_limits<float>::max();
	float max_time = 0;

	// graphing most recent

	std::vector<float> xs, ys, zs;
	std::vector<float> avg_times, min_times, max_times;


	// storing generation data

	std::vector<SavedGeneration> saved_info;
};

