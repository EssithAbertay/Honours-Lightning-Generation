#pragma once
#include <vector>

enum CAMERA_METHOD
{
	rotating,
	control,
};

enum TEST_TYPE
{
	time_test,
	target_test,
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

	bool using_target;
	int target_x;
	int target_z;
	float target_weight;

	// todo: save all the config info!
	// todo: make sure tolerance saves correctly!
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

	bool include_border_cells = false; // are the zeroes from the borders used when calculating laplace

	// if true, select candidates from air cells, if false select from lightning cells, bad name, is unclear
	bool candidates_from_air = false; 
	int candidate_selection = 0; // 0 for air, 1 for lightning


	float gradient_tolerance = 0.005;

	bool use_loop_cap = true;
	int max_laplace_loops = 50;
	bool use_calculated_loops = true;
	float loop_multiplier = 1.5;

	int eta = 1;

	std::vector<int> starting_charges;
	
	bool is_regenerate_this_frame = false;

	bool is_multithread = false;

	bool is_perform_test = false;

	bool is_perform_target_test = false;

	TEST_TYPE test_type = TEST_TYPE::time_test;

	int dimension_increment = 5;
	int number_to_average = 5;
	int targets_to_test = 5;

	std::vector<float> times;
	float min_time = std::numeric_limits<float>::max();
	float max_time = 0;

	int grid_steps = 0;


	// graphing most recent

	std::vector<float> xs, ys, zs;
	std::vector<float> avg_times, min_times, max_times;


	// storing generation data

	std::vector<SavedGeneration> saved_info;

	// camera
	CAMERA_METHOD cam_method = rotating;
	int cam_angle = 0;
	
	bool is_bounding_box = false;
	bool is_initial_charge = true;

	// capsules
	bool line_mode = false;

	bool wireframe = false;

	float radius = 0.1;
	int slices = 3;
	int rings = 3;


	// distance wieghting
	bool use_target = false;
	float target_lambda = 0.05;
	int target_x = 0;
	int target_y = 0;
	int target_z = 0;
};

