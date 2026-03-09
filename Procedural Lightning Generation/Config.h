#pragma once
#include <vector>

enum CAMERA_METHOD
{
	rotating,
	control,
};

struct SavedGeneration
{
	int x_size, y_size, z_size;
	float time;
	int eta;
	int grid_steps;
	bool candidates_from_air;
	bool multithreading_enabled;
	bool resetting_volume;
	float gradient_tolerance;
	bool loop_cap_enabled;
	int max_loops;

	// todo: save all the config info!
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
	bool candidates_from_air = false; 
	int candidate_selection = 0; // 0 for air, 1 for lightning


	float gradient_tolerance = 0.005;

	bool use_loop_cap = true;
	int max_laplace_loops = 50;
	bool use_calculated_loops = true;
	float loop_multiplier = 1.5;

	int eta = 1;

	bool is_bounding_box = false;

	bool is_regenerate_this_frame = false;

	bool is_multithread = false;

	bool is_perform_test = false;

	int dimension_increment = 5;
	int number_to_average = 5;

	std::vector<float> times;
	float min_time = std::numeric_limits<float>::max();
	float max_time = 0;

	int grid_steps = 0;


	// graphing most recent

	std::vector<float> xs, ys, zs;
	std::vector<float> avg_times, min_times, max_times;


	// storing generation data

	std::vector<SavedGeneration> saved_info;

	// display method
	CAMERA_METHOD cam_method = rotating;
	int cam_angle = 0;
};

