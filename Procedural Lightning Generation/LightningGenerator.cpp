#include "LightningGenerator.h"

LightningGenerator::LightningGenerator()
{

}

void LightningGenerator::setVars()
{
	// at the start of each generation we have some stuff to set, like wghat laplace function to use, despite the option being deprecat

	tolerance = configuration->gradient_tolerance;

	//if (configuration->include_border_cells)
	//{
	//	laplace_func = &LightningGenerator::calculateLaplace;
	//}
	//else
	//{
		laplace_func = &LightningGenerator::calculateLaplaceNoBorders; // other option was deprecated
	//}

		// know how many gradient loops to use in advance

	if (configuration->use_calculated_loops)
	{
		max_laplace_loops = configuration->y_size * configuration->loop_multiplier; // user chooses a multiplier, defaults to 1.5 so that it goes ground to air every time

		configuration->max_laplace_loops = max_laplace_loops; // set this so that it saves the data correctly
	}
	else
	{
		max_laplace_loops = configuration->max_laplace_loops;
	}
}

void LightningGenerator::regenLightning_optimised()
{
	grid_steps_made = 0;

	initialiseGrid(); // setup grid

	// need to set an initial lightning point when checking candidates against lightning cells so that there isnt a null errror
	if(!configuration->candidates_from_air)
	{
		int starting_x = configuration->x_size / 2; //place the starting charge in the middle of the sky
		int starting_z = configuration->z_size / 2;
		LightningCell intialCharge = LightningCell(starting_x, 0, starting_z, starting_x, 0, starting_z);

		lightning_points.push_back(intialCharge);
	}

	while (!reached_edge) //keep stepping until on ground
	{
		performLightningStep_optimised();
	}


	//std::cout << "grid steps" << grid_steps_made << std::endl;
}

void LightningGenerator::regenLightning_unoptimised() // deprecated
{
	grid_steps_made = 0; // same as with the optimised version, just this one uses other functions and doesnt require an initial charge as checks against air not lightning

	initialiseGrid();

	while (!reached_edge)
	{
		performLightningStep();
	}

	//std::cout << "grid steps" << grid_steps_made << std::endl;
}

void LightningGenerator::performLightningStep_optimised()
{
	bool is_within_tolerance = true;

	int loops = 0;

	while (is_within_tolerance) // perform adjacent gradient until tolernace is reached
	{

		if (configuration->is_multithread)
		{
			is_within_tolerance = calculateGridStep_multithread();
		}
		else
		{
			is_within_tolerance = calculateGridStep();
		}
		loops++;

		if (loops >= max_laplace_loops && configuration->use_loop_cap) // if it's being used and the loop cap is reached just break out, no matter if within tolerance
		{
			is_within_tolerance = false;
		}

	}

	if (!configuration->candidates_from_air) // depending on user choice, collect candidates from air or lighnting
	{
		collectCandidates_optimised(); // collect from lightning
	}
	else
	{
		collectCandidates(); // collect from air
	}

	selectLightningCell(); // select cell from candidates collected

	if (configuration->reset_vol_between_steps) // if selected to reset the volume, the volume is reset
	{
		resetPotentialGrid();
	}
}

void LightningGenerator::performLightningStep() //deprecated, use optimised
{
	bool is_within_tolerance = true;

	int loops = 0;

	while (is_within_tolerance)
	{
		is_within_tolerance = calculateGridStep();
		loops++;

		if (loops >= max_laplace_loops)
		{
			is_within_tolerance = false;
		}
	}

	collectCandidates();
	selectLightningCell();

	resetPotentialGrid();
}

void LightningGenerator::createStartingGrid()
{
	for (int z = 0; z < configuration->z_size; z++)
	{
		for (int y = 0; y < configuration->y_size; y++)
		{
			for (int x = 0; x < configuration->x_size; x++)
			{
				int idx = index(x, y, z); // reduction of calls

				if (y == 0) //check for ceiling
				{
					starting_flat[idx] = 3; // ceiling/ same as walls
				}
				else if (y == configuration->y_size - 1) // check for ground
				{
					starting_flat[idx] = 1; // ground
				}
				else if (x == 0 || x == configuration->x_size - 1 || z == 0 || z == configuration->z_size - 1) // check for walls
				{
					starting_flat[idx] = 3; // walls
				}
				else
				{
					starting_flat[idx] = 0;  // air
				}
			}
		}
	}

	// place initial charge

	int starting_x = configuration->x_size / 2;
	int starting_z = configuration->z_size / 2;

	starting_flat[index(starting_x, 0, starting_z)] = 2;
}

void LightningGenerator::initialiseGrid()
{
	int total = configuration->x_size * configuration->y_size * configuration->z_size;

	potentials_flat.clear();
	new_potentials_flat.clear();
	starting_flat.clear();

	potentials_flat.resize(total);
	new_potentials_flat.resize(total);
	starting_flat.resize(total);

	lightning_points.clear();

	reached_edge = false;

	//max_laplace_loops = std::max(configuration->y_size * 1.5 + 20, double(50)); // moved to set vars, makes more sense there, this formula also deprecated
	
	createStartingGrid();

	for (int z = 0; z < configuration->z_size; z++)
	{
		for (int y = 0; y < configuration->y_size; y++)
		{
			for (int x = 0; x < configuration->x_size; x++)
			{
				int idx = index(x, y, z); // reduction of calls

				if (y == 0) //check for ceiling
				{
					potentials_flat[idx] = 0;
					new_potentials_flat[idx] = 0;
				}
				else if (y == configuration->y_size - 1) // check for ground
				{
					potentials_flat[idx] = 1;
					new_potentials_flat[idx] = 1;
				}
				else if (x == 0 || x == configuration->x_size - 1 || z == 0 || z == configuration->z_size - 1) // check for walls
				{
					potentials_flat[idx] = 0;
					new_potentials_flat[idx] = 0;
				}
				else // air
				{
					potentials_flat[idx] = 0.5;
					new_potentials_flat[idx] = 0.5;
				}
			}
		}
	}
}

void LightningGenerator::checkCandidacy(int x_pos, int y_pos, int z_pos)
{
	// check every cell surrounding air cell
	for (int x = -1; x <= 1; x++)
	{
		for (int y = -1; y <= 1; y++)
		{
			for (int z = -1; z <= 1; z++)
			{
				if (x_pos + x >= configuration->x_size || y_pos + y >= configuration->y_size || z_pos + z >= configuration->z_size || x_pos + x <  0 || y_pos + y < 0 || z_pos + z < 0) continue;  // if checking cells that are out of bounds skip

				if (x == 0 && y == 0 && z == 0) continue;// skip middle cell

				if (starting_flat[index(x_pos + x, y_pos + y, z_pos + z)] == 3) continue; //skip boundaries 

				if (potentials_flat[index(x_pos + x,y_pos + y,z_pos + z)] == 0 ) // if a surrounding cell is lightning then this is a candidate
				{
					if (starting_flat[index(x_pos, y_pos, z_pos)] == 1) // check if this candidate is ground
					{
						//is_ground_candidate_found = true;
						reached_edge = true;
					}

					// create a temporary candidate cell and fill with info

					candidate_cell temp;

					temp.potential = potentials_flat[index(x_pos,y_pos,z_pos)];
					temp.x = x_pos;
					temp.y = y_pos;
					temp.z = z_pos;

					temp.parent_x = x_pos + x;
					temp.parent_y = y_pos + y;
					temp.parent_z = z_pos + z;

					if (reached_edge) // if the candidate cell is a ground cell, it's the only one to check
					{
						candidates.clear();
						candidates.push_back(temp);			
						return;
					}

					candidates.push_back(temp); // make new candidate

					x = 2; y = 2; z = 2; // skip out the check cause we know it's a candidate // why not just use a return? or similar?
				}
			}
		}
	}
}

void LightningGenerator::collectCandidates()
{
	candidates.clear();

	// check every air cell, for if its a candidate, if we reach the ground then return, otherwise continue checking, we dont always reach the ground, in fact it's rare
	for (int z = 0; z < configuration->z_size; z++)
	{
		for (int y = 0; y < configuration->y_size; y++)
		{
			for (int x = 0; x < configuration->x_size; x++)
			{
				if (potentials_flat[index(x,y,z)] == 0) //skip current lightning cells or boundaries
				{
					continue;  
				}

				checkCandidacy(x, y, z);

				if (reached_edge) // if reached the edge return
				{
					return;
				}
			}
		}
	}
}

void LightningGenerator::collectCandidates_optimised()
{
	//instead of going through every air cell, we will go through each lightning cell and check them instead
	visited_lightning.clear();

	candidates.clear();

	for (auto& point : lightning_points) // for each lightning cell
	{
		for (int x = -1; x <= 1; x++) // check neighbours
		{
			for (int y = -1; y <= 1; y++)
			{
				for (int z = -1; z <= 1; z++)
				{

					if (x == 0 && y == 0 && z == 0) continue;// skip self
					if (point.x + x >= configuration->x_size || point.y + y >= configuration->y_size || point.z + z >= configuration->z_size || point.x + x < 0 || point.y + y < 0 || point.z + z < 0) continue;  // if checking cells that are out of bounds skip
					//if (potentials[point.z + z][point.y + y][point.x + x] == 0) continue;//skip other lightning cells or boundaries
					if (potentials_flat[index(point.x + x,point.y + y,point.z + z)] == 0) continue;//skip other lightning cells or boundaries

					// as we've skipped everything else, this cell must be air or ground

					// if it's ground we want to only select it

					//if (starting[point.z + z][point.y + y][point.x + x] == 1) // check if candidate ground is next to lightning
					if (starting_flat[index(point.x + x, point.y + y, point.z + z)] == 1) // check if candidate ground is next to lightning, as we check every cell around a lightning it shouldnt matter the order we do it in
					{
						//is_ground_candidate_found = true;
						reached_edge = true;

						//std::cout << "found ground" << std::endl;
					}

					//  define this as a candidate

					candidate_cell temp;

					//temp.potential = potentials[point.z + z][point.y + y][point.x + x];
					temp.potential = potentials_flat[index(point.x + x,point.y + y,point.z + z)];
					temp.x = point.x + x;
					temp.y = point.y + y;
					temp.z = point.z + z;

					// check if this candidate already exists, so that we dont duplicate candidates

					bool duplicate = false;

					duplicate = visited_lightning.contains(index(temp.x, temp.y, temp.z));
					
					if (duplicate)
					{
						continue;
					}

					temp.parent_x = point.x;
					temp.parent_y = point.y;
					temp.parent_z = point.z;

					// if it was a ground we get rid of the rest

					if (reached_edge)
					{
						candidates.clear();
						candidates.push_back(temp);
						return; // using a return so we leave properly
					}

					visited_lightning.insert(index(temp.x, temp.y, temp.z));
					candidates.push_back(temp);
				}
			}
		}
	}
}

void LightningGenerator::selectLightningCell()
{
	// formula for probability
	// https://ieeexplore.ieee.org/document/1348357
	
	// Formula for DBM proability is from
	// Kim, T. and Lin, M.C. (2004).Physically based animation and rendering of lightning. 12th Pacific Conference on Computer Graphics and Applications, 2004. PG 2004. Proceedings. (pp.267–275).Available at : https://doi.org/10.1109/PCCGA.2004.1348357

	float total_potential = 0;

	if (configuration->use_target) // todo: fix large weighting bug
	{
		int targetx = configuration->target_x;
		int targety = configuration->target_y;
		int targetz = configuration->target_z;

		for (int i = 0; i < candidates.size(); i++)
		{
			// find distance to the target
			int dx = targetx - candidates[i].x;
			int dy = targety - candidates[i].y;
			int dz = targetz - candidates[i].z;

			float distance = sqrtf((dx * dx) + (dy * dy) + (dz * dz));

			// the closer to target the greater the weighting on the potential

			float distance_weight = expf(-configuration->target_lambda * distance);
			
			//distance_weight = std::max((float)0.001, distance_weight); // if 0 or below strange behaviour occurs

			/*candidates[i].distance_weight = distance_weight;

			float weighted = pow(candidates[i].potential, configuration->eta) * distance_weight;
			total_potential += weighted;*/


			candidates[i].distance_weight = distance_weight;
			candidates[i].potential *= distance_weight;

			total_potential += pow(candidates[i].potential, configuration->eta); // total the potentials
		}
	}
	else
	{
		for (int i = 0; i < candidates.size(); i++)
		{
			total_potential += pow(candidates[i].potential, configuration->eta); // total the potentials with no weighting
		}
	}

	//for (auto& x : candidates)
	//{
	//	float weighted = pow(x.potential, configuration->eta);

	//	if (configuration->use_target) // apply the distance weight again
	//	{
	//		weighted *= x.distance_weight;
	//	}

	//	x.probability = weighted / total_potential;
	//}

	for (auto& x : candidates)
	{
		x.probability = (pow(x.potential, configuration->eta)) / total_potential; // find the probaility based on the potentials of each
	}

	// function for generating a random number from, function generates a float between 0 and 1
	// https://en.cppreference.com/w/cpp/numeric/random/generate_canonical.html
	float rnd = std::generate_canonical<float, 16>(gen);

	int chosen_candidate = 0;

	//subtracts from rnd if rnd is larger than the probabilty, otherwise selects as the chosen cell
	for (int i = 0; i < candidates.size(); i++) { 
		if (rnd < candidates[i].probability)
		{
			chosen_candidate = i;
			break;
		}
		rnd -= candidates[i].probability;
	}

	candidate_cell chosen = candidates[chosen_candidate];

	// adds the chosed cell to the points
	lightning_points.push_back(LightningCell(chosen.x, chosen.y, chosen.z, chosen.parent_x, chosen.parent_y, chosen.parent_z));

	// updates points
	potentials_flat[index(chosen.x, chosen.y, chosen.z)] = 0;
	new_potentials_flat[index(chosen.x, chosen.y, chosen.z)] = 0;

	//potentials[chosen.z][chosen.y][chosen.x] = 0;
	//new_potentials[chosen.z][chosen.y][chosen.x] = 0;
}

void LightningGenerator::resetPotentialGrid()
{
	for (int z = 0; z < configuration->z_size; z++)
	{
		for (int y = 0; y < configuration->y_size; y++)
		{
			for (int x = 0; x < configuration->x_size; x++)
			{
				//if (potentials[z][y][x] == 0 || potentials[z][y][x] == 1)
				if (potentials_flat[index(x,y,z)] == 0 || potentials_flat[index(x, y, z)] == 1)
				{
					continue;
				}

				potentials_flat[index(x, y, z)] = 0.5;
			}

		}
	}
}

float LightningGenerator::calculateLaplace(int x_pos, int y_pos, int z_pos)
{
		float left = potentials_flat[index(x_pos, y_pos - 1, z_pos)];
		float right = potentials_flat[index(x_pos, y_pos + 1, z_pos)];
		float forward = potentials_flat[index(x_pos, y_pos, z_pos + 1)];
		float backward = potentials_flat[index(x_pos, y_pos, z_pos - 1)];
		float up = potentials_flat[index(x_pos - 1, y_pos, z_pos)];
		float down = potentials_flat[index(x_pos + 1, y_pos, z_pos)];

		float average = left + right + forward + backward + up + down;
		average /= 6;

		//return average;
}

float LightningGenerator::calculateLaplaceNoBorders(int x_pos, int y_pos, int z_pos)
{

	// use comparisons to avoid boundaries

	float sum = 0.0f;
	int count = 0;

	// left
	if (starting_flat[index(x_pos, y_pos - 1, z_pos)] != 3) 
	{
		sum += potentials_flat[index(x_pos, y_pos - 1, z_pos)];
		count++;
	}

	// right
	if (starting_flat[index(x_pos, y_pos + 1, z_pos)] != 3) 
	{
		sum += potentials_flat[index(x_pos, y_pos + 1, z_pos)];
		count++;
	}

	// forward
	if (starting_flat[index(x_pos, y_pos, z_pos + 1)] != 3) 
	{
		sum += potentials_flat[index(x_pos, y_pos, z_pos + 1)];
		count++;
	}

	// backward
	if (starting_flat[index(x_pos, y_pos, z_pos - 1)] != 3) 
	{
		sum += potentials_flat[index(x_pos, y_pos, z_pos - 1)];
		count++;
	}

	// up
	if (starting_flat[index(x_pos - 1, y_pos, z_pos)] != 3) 
	{
		sum += potentials_flat[index(x_pos - 1, y_pos, z_pos)];
		count++;
	}

	// down
	if (starting_flat[index(x_pos + 1, y_pos, z_pos)] != 3) 
	{
		sum += potentials_flat[index(x_pos + 1, y_pos, z_pos)];
		count++;
	}

	return sum / count;
}

bool LightningGenerator::calculateGridStep()
{
	grid_steps_made++; // todo: do this with config instead!

	bool is_within_tolerance = false;

	// takes each cell, and finds potential, the swaps to new vector

	for (int z = 1; z < configuration->z_size - 1; z++)
	{
		for (int y = 1; y < configuration->y_size - 1; y++)
		{
			for (int x = 1; x < configuration->x_size - 1; x++)
			{
				if (potentials_flat[index(x,y,z)] == 0 || potentials_flat[index(x, y, z)] == 1) //skip anything with 0 or 1 (ground, boundary, or already lightning)
				{
					continue;
				}

				float new_value = (this->*laplace_func)(x, y, z);
				float old_value = potentials_flat[index(x, y, z)];

				new_potentials_flat[index(x, y, z)] = new_value;

				if (abs(old_value - new_value) >= tolerance)
				{
					is_within_tolerance = true;
				}
			}
		}
	}

	std::swap(potentials_flat, new_potentials_flat);

	return is_within_tolerance; //return if the largest change was within tolerance or not
}


// Multithread uses the IntelOne Api, which can be found at the link below
// https://www.intel.com/content/www/us/en/developer/tools/oneapi/base-toolkit.html

bool LightningGenerator::calculateGridStep_multithread()
{
	//performs the grid step on the GPU, innefficiencies caused by poor implementation and too many overheads

	bool is_within_tolerance = false;

	grid_steps_made++; // todo: do this with config instead!

	// buffers that will store data
	sycl::buffer<float, 1> potentials_buffer(potentials_flat.data(), sycl::range<1>(configuration->x_size * configuration->y_size * configuration->z_size));
	sycl::buffer<float, 1> starting_buffer(starting_flat.data(), sycl::range<1>(configuration->x_size * configuration->y_size * configuration->z_size));
	sycl::buffer<float, 1> new_potentials_buffer(new_potentials_flat.data(), sycl::range<1>(configuration->x_size * configuration->y_size * configuration->z_size));

	int x_size = configuration->x_size;
	int y_size = configuration->y_size;
	int z_size = configuration->z_size;

	float local_tolerance = tolerance;


	//try {
		q.submit([&](sycl::handler& h) {

			// accessors to read from buffers
			sycl::accessor pot_a(potentials_buffer, h, sycl::read_only);
			sycl::accessor start_a(starting_buffer, h, sycl::read_only);
			sycl::accessor new_pot_a(new_potentials_buffer, h, sycl::write_only, sycl::no_init);


			// a reduction accessor so that we don't use an atomic variable when checking tolerance
			auto reduction_acc = sycl::reduction(&is_within_tolerance, std::logical_or<bool>());

			// for each cell we use a thread to find the new potential
			// -2/+1 means we avoid the edges and only deal with internals
			h.parallel_for<kernels::gridstepKernel>(sycl::range<3>(x_size-2,y_size-2,z_size-2),reduction_acc, [=](sycl::item<3> idx, auto& is_tol) {
				int x = idx[0] + 1;
				int y = idx[1] + 1;
				int z = idx[2] + 1;

				int flat_index = x + (y * x_size) + (z * x_size * y_size);

				if (pot_a[flat_index] != 0 && pot_a[flat_index] != 1) //skip anything with 0 or 1
				{
					// todo: this doesn't have the boundary option

					//float left = pot_a[x + (y - 1) * x_size + z * x_size * y_size];
					//float right = pot_a[x + (y + 1) * x_size + z * x_size * y_size];
					//float forward = pot_a[x + y * x_size + (z + 1) * x_size * y_size];
					//float backward = pot_a[x + y * x_size + (z - 1) * x_size * y_size];
					//float up = pot_a[(x - 1) + y * x_size + z * x_size * y_size];
					//float down = pot_a[(x + 1) + y * x_size + z * x_size * y_size];

					//float average = (left + right + forward + backward + up + down) / 6.0f;
					float average = 0.0f;
					float sum = 0.0f;
					int count = 0;

					// left
					if (start_a[x + (y - 1) * x_size + z * x_size * y_size] != 3)
					{
						sum += pot_a[x + (y - 1) * x_size + z * x_size * y_size];
						count++;
					}

					// right
					if (start_a[x + (y + 1) * x_size + z * x_size * y_size] != 3)
					{
						sum += pot_a[x + (y + 1) * x_size + z * x_size * y_size];
						count++;
					}

					// forward
					if (start_a[x + y * x_size + (z + 1) * x_size * y_size] != 3)
					{
						sum += pot_a[x + y * x_size + (z + 1) * x_size * y_size];
						count++;
					}

					// backward
					if (start_a[x + y * x_size + (z - 1) * x_size * y_size] != 3)
					{
						sum += pot_a[x + y * x_size + (z - 1) * x_size * y_size];
						count++;
					}

					// up
					if (start_a[(x - 1) + y * x_size + z * x_size * y_size] != 3)
					{
						sum += pot_a[(x - 1) + y * x_size + z * x_size * y_size];
						count++;
					}

					// down
					if (start_a[(x + 1) + y * x_size + z * x_size * y_size] != 3)
					{
						sum += pot_a[(x + 1) + y * x_size + z * x_size * y_size];
						count++;
					}

					average = sum / count;

					new_pot_a[flat_index] = average;

					if (sycl::fabs(pot_a[flat_index] - average) >= local_tolerance) {
						is_tol.combine(true);  // mark that tolerance exceeded
					}
				}
			});
		});

		q.wait();

		std::swap(potentials_flat, new_potentials_flat); // swap the vectors
	//}
	//catch (sycl::exception const& e) {
	//	std::cout << "SYCL exception: " << e.what() << " category: " << e.category().name() << " code:" << e.code() << std::endl;
	//}


	return is_within_tolerance;
}

