#include "LightningGenerator.h"

LightningGenerator::LightningGenerator()
{

}

void LightningGenerator::regenLightning_multithread()
{

}

void LightningGenerator::regenLightning_other()
{


}

void LightningGenerator::regenLightning_optimised()
{
	grid_steps_made = 0;
	
	initialiseGrid();

	// need to set an initial lightning spot for the optimised version
	int starting_x = configuration->x_size / 2;
	int starting_z = configuration->z_size / 2;
	LightningCell intialCharge = LightningCell(starting_x, 0, starting_z, starting_x, 0, starting_z);

	lightning_points.push_back(intialCharge);
	
	while (!reached_edge)
	{
		performLightningStep_optimised();
	}

	std::cout << "grid steps" << grid_steps_made << std::endl;
}

void LightningGenerator::regenLightning_unoptimised()
{
	grid_steps_made = 0;

	initialiseGrid();

	while (!reached_edge)
	{
		performLightningStep();
	}

	std::cout << "grid steps" << grid_steps_made << std::endl;
}

void LightningGenerator::performLightningStep_optimised()
{
	bool is_within_tolerance = true;

	int loops = 0;

	while (is_within_tolerance)
	{
		is_within_tolerance = calculateGridStep();
		loops++;

		if (loops >= MAX_GRADIENT_LAPLACE_LOOPS)
		{
			is_within_tolerance = false;
		}
	}

	collectCandidates_optimised(); // todo: fix bug in here
	selectLightningCell();
}


void LightningGenerator::performLightningStep()
{
	bool is_within_tolerance = true;

	int loops = 0;

	while (is_within_tolerance)
	{
		is_within_tolerance = calculateGridStep();
		loops++;

		if (loops >= MAX_GRADIENT_LAPLACE_LOOPS)
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
	starting.clear();

	for (int z = 0; z < configuration->z_size; z++)
	{
		std::vector<std::vector<int>> face;

		for (int y = 0; y < configuration->y_size; y++)
		{
			std::vector<int> row;

			for (int x = 0; x < configuration->x_size; x++)
			{
				if (y == 0) //check for ceiling
				{
					row.push_back(3);

				}
				else if (y == configuration->y_size - 1) // check for ground
				{
					row.push_back(1);
				}
				else if (x == 0 || x == configuration->x_size - 1 || z == 0 || z == configuration->z_size - 1) // check for walls
				{
					row.push_back(3);
				}
				else // air
				{
					row.push_back(0);
				}
			}

			face.push_back(row);
		}

		starting.push_back(face);
	}

	int starting_x = configuration->x_size / 2;
	int starting_z = configuration->z_size / 2;

	starting[starting_z][0][starting_x] = 2;
}

void LightningGenerator::initialiseGrid()
{
	lightning_points.clear();
	potentials.clear();
	new_potentials.clear();

	MAX_GRADIENT_LAPLACE_LOOPS = std::max(configuration->x_size * 1.5 + 20, double(30));
	reached_edge = false;


	createStartingGrid();

	for (int z = 0; z < configuration->z_size; z++)
	{
		std::vector<std::vector<float>> face;

		for (int y = 0; y < configuration->y_size; y++)
		{
			std::vector<float> row;

			for (int x = 0; x < configuration->x_size; x++)
			{
				if (y == 0) //check for ceiling
				{
					row.push_back(0);
				}
				else if (y == configuration->y_size - 1) // check for ground
				{
					row.push_back(1);
				}
				else if (x == 0 || x == configuration->x_size - 1 || z == 0 || z == configuration->z_size - 1) // check for walls
				{
					row.push_back(0);
				}
				else // air
				{
					row.push_back(0.5);
				}
			}

			face.push_back(row);
		}

		potentials.push_back(face);
		new_potentials.push_back(face);
	}

}

void LightningGenerator::checkCandidacy(int x_pos, int y_pos, int z_pos)
{
	// check if cell has lightning on border
	bool is_ground_candidate_found = false;


	for (int x = -1; x <= 1; x++)
	{
		for (int y = -1; y <= 1; y++)
		{
			for (int z = -1; z <= 1; z++)
			{
				if (x_pos + x >= configuration->x_size || y_pos + y >= configuration->y_size || z_pos + z >= configuration->z_size) continue;  // if checking cells that are out of bounds skip

				if (x == 0 && y == 0 && z == 0) continue;// skip middle cell

				if (y_pos + y < 0) continue; //temp fix // why is this here?????
				if (x_pos + x < 0) continue; //temp fix // why is this here?????
				if (z_pos + z < 0) continue; //temp fix // why is this here?????


				if (starting[z_pos + z][y_pos + y][x_pos + x] == 3) continue; //skip boundaries 

				if (potentials[z_pos + z][y_pos + y][x_pos + x] == 0) // if a surrounding cell is lightning then this is a candidate
				{

					if (starting[z_pos + z][y_pos + y][x_pos + x] == 1) // check if candidate ground is next to lightning
					{
						is_ground_candidate_found = true;
						reached_edge = true;
					}

					candidate_cell temp;

					temp.potential = potentials[z_pos][y_pos][x_pos];
					temp.x = x_pos;
					temp.y = y_pos;
					temp.z = z_pos;

					temp.parent_x = x_pos + x;
					temp.parent_y = y_pos + y;
					temp.parent_z = z_pos + z;

					if (is_ground_candidate_found)
					{
						candidates.clear();
						candidates.push_back(temp);
						break;
					}

					candidates.push_back(temp);

					x = 2; y = 2; z = 2; // skip out the check cause we knw it's a candidate // why not just use a break? or similar?



			


				}
			}
		}
	}
}

void LightningGenerator::collectCandidates()
{
	candidates.clear();

	for (int z = 0; z < configuration->z_size; z++)
	{
		for (int y = 0; y < configuration->y_size; y++)
		{
			for (int x = 0; x < configuration->x_size; x++)
			{
				if (potentials[z][y][x] == 0) //skip current lightning cells or boundaries
				{
					continue;
				}
				checkCandidacy(x, y, z);
			}
		}
	}
}

void LightningGenerator::collectCandidates_optimised()
{
	//instead of going through every air cell, we will go through each lightning cell and check them instead
	// todo: currently can get duplicate candidates, create a second check candidacy function

	candidates.clear();
	bool is_ground_candidate_found = false;

	for (auto & point : lightning_points) // for each lightning cell
	{
		for (int x = -1; x <= 1; x++) // check neighbours
		{
			for (int y = -1; y <= 1; y++)
			{
				for (int z = -1; z <= 1; z++)
				{

					if (x == 0 && y == 0 && z == 0) continue;// skip self
					if (point.x + x >= configuration->x_size || point.y + y >= configuration->y_size || point.z + z >= configuration->z_size || point.x + x < 0 || point.y + y < 0 || point.z + z < 0) continue;  // if checking cells that are out of bounds skip
					if (potentials[point.z + z][point.y + y][point.x + x] == 0) continue;//skip other lightning cells or boundaries

					// as we've skipped everything else, this cell must be air or ground

					// if it's ground we want to only select it

					if (starting[point.z + z][point.y + y][point.x + x] == 1) // check if candidate ground is next to lightning
					{
						is_ground_candidate_found = true;
						reached_edge = true;
					}

					//  define this as a candidate

					candidate_cell temp;

					temp.potential = potentials[point.z + z][point.y + y][point.x + x];
					temp.x = point.x + x;
					temp.y = point.y + y;
					temp.z = point.z + z;

					// check if this candidate already exists, so that we dont duplicate candidates

					bool duplicate = false;
					for (auto x : candidates) // dont like doing it like this, should use a map/set instead
					{
						if (temp == x)
						{
							duplicate = true;
							break;
						}
					}

					if (duplicate)
					{
						continue;
					}

					temp.parent_x = point.x;
					temp.parent_y = point.y;
					temp.parent_z = point.z;

					// if it was a ground we get rid of the rest

					if (is_ground_candidate_found)
					{
						candidates.clear();
						candidates.push_back(temp);
						break;
					}

					candidates.push_back(temp);
				}
			}
		}
	}
}

void LightningGenerator::selectLightningCell()
{


	// formula for probability
	// https://onlinelibrary.wiley.com/cms/asset/6dcd580c-06ae-421c-acf7-f5449d06a5e4/cav1760-math-0002.png

	float total_potential = 0;

	for (int i = 0; i < candidates.size(); i++)
	{
		total_potential += pow(candidates[i].potential, configuration->eta);
	}

	for (auto& x : candidates)
	{
		x.probability = (pow(x.potential, configuration->eta)) / total_potential;
	}

	// taken from cpp reference  https://en.cppreference.com/w/cpp/numeric/random/generate_canonical.html
	float rnd = std::generate_canonical<float, 10>(gen);

	int chosen_candidate = 0;

	for (int i = 0; i < candidates.size(); i++) {
		if (rnd < candidates[i].probability)
		{
			chosen_candidate = i;
			break;
		}
		rnd -= candidates[i].probability;
	}

	candidate_cell chosen = candidates[chosen_candidate];

	lightning_points.push_back(LightningCell(chosen.x, chosen.y, chosen.z, chosen.parent_x, chosen.parent_y, chosen.parent_z));

	potentials[chosen.z][chosen.y][chosen.x] = 0;
	new_potentials[chosen.z][chosen.y][chosen.x] = 0;
}

void LightningGenerator::resetPotentialGrid()
{
	for (int z = 0; z < configuration->z_size; z++)
	{
		for (int y = 0; y < configuration->y_size; y++)
		{
			for (int x = 0; x < configuration->x_size; x++)
			{
				if (potentials[z][y][x] == 0 || potentials[z][y][x] == 1)
				{
					continue;
				}

				potentials[z][y][x] = 0.5;
			}

		}
	}
}

float LightningGenerator::calculateLaplace(int x_pos, int y_pos, int z_pos)
{
	float left = potentials[z_pos][y_pos - 1][x_pos];
	float right = potentials[z_pos][y_pos + 1][x_pos];
	float forward = potentials[z_pos + 1][y_pos][x_pos];
	float backward = potentials[z_pos - 1][y_pos][x_pos];
	float up = potentials[z_pos][y_pos][x_pos - 1];
	float down = potentials[z_pos][y_pos][x_pos + 1];

	float average = left + right + forward + backward + up + down;
	average /= 6;

	return average;
}

bool LightningGenerator::calculateGridStep()
{
	grid_steps_made++;


	bool is_within_tolerance = false;

	float tolerance = configuration->gradient_tolerance;

	for (int z = 1; z < configuration->z_size-1; z++)
	{
		for (int y = 1; y < configuration->y_size-1; y++)
		{
			for (int x = 1; x < configuration->x_size-1; x++)
			{
				if (potentials[z][y][x] == 0 || potentials[z][y][x] == 1) //skip anything with 0 or 1
				{
					continue;
				}

				float new_value = calculateLaplace(x, y, z);

				float old_value = potentials[z][y][x];

				new_potentials[z][y][x] = new_value;

				if (abs(old_value - new_value) >= tolerance)
				{
					is_within_tolerance = true;
				}
			}
		}
	}

	std::swap(potentials, new_potentials);

	return is_within_tolerance;
}

