#include "LightningRenderer.h"

#include <iostream>

void LightningRenderer::render()
{	
	segment_size = (GetScreenHeight()/9) / std::max(y_size, std::max(x_size, z_size));

	int y_offset = segment_size;

	Vector3 position = { float(configuration->x_size * segment_size), (configuration->y_size + (configuration->y_size / 2)) * segment_size / 2,  -float(std::max(y_size, std::max(x_size, z_size)) * segment_size - (segment_size * 3)) };

	if (configuration->x_size != x_size || configuration->y_size != y_size || configuration->z_size != z_size)
	{
		camera->position = position;
	}

	x_size = configuration->x_size;
	y_size = configuration->y_size;
	z_size = configuration->z_size;	
	


	// keep the camera always looking at the centre of the structure

	Vector3 centre = { float(x_size * segment_size / 2), float(y_size * segment_size / 2),  float(z_size * segment_size / 2) };
	camera->target = centre;

	float radius;
	float angle;

	switch (configuration->cam_method)
	{
	case rotating:
		UpdateCamera(camera, CAMERA_ORBITAL);
		break;
	case control:
		radius = Vector3Distance(position, centre);
		angle = configuration->cam_angle * DEG2RAD;

		camera->position.x = centre.x + cosf(angle) * radius;
		camera->position.z = centre.z + sinf(angle) * radius;
		camera->position.y = position.y;

		camera->target = centre;

		UpdateCamera(camera, CAMERA_CUSTOM);
		break;
	default:
		break;
	}





	BeginMode3D(*camera);



	if (configuration->is_bounding_box)
	{
		DrawCubeWires(centre, x_size*segment_size, y_size * segment_size, z_size * segment_size, RAYWHITE);
	}

	float y_start = segment_size * (y_size -1.5); // -1.5 to align with volume

	if (configuration->is_initial_charge) // todo: fix this
	{
		int starting_x = (x_size/2);
		int starting_z = (z_size/2);

		Vector3 initial_charge_position = {
		(starting_x) * segment_size,
		y_start + segment_size,
		(starting_z) * segment_size
		};

		DrawCubeWires(initial_charge_position, segment_size, segment_size, segment_size, GREEN);
	}

	if (configuration->use_target) // todo: fix this
	{

		Vector3 target_position = {
		configuration->target_x * segment_size,
		y_start - ((y_size - 2) * segment_size),
		configuration->target_z * segment_size
		};

		DrawCubeWires(target_position, segment_size, segment_size, segment_size, RED);
	}


	//
	for (int i = 0; i < lightning_points->size(); i++)
	{
		Vector3 start_pos = { lightning_points->at(i).parent_x * segment_size, y_start - (lightning_points->at(i).parent_y * segment_size) + y_offset, lightning_points->at(i).parent_z * segment_size };
		Vector3 end_pos = { lightning_points->at(i).x * segment_size, y_start - (lightning_points->at(i).y * segment_size) + y_offset, lightning_points->at(i).z * segment_size };

		if (configuration->line_mode)
		{
			DrawLine3D(end_pos, start_pos, YELLOW);
		}
		else
		{
			if (configuration->wireframe)
			{
				DrawCapsuleWires(start_pos, end_pos, configuration->radius, configuration->slices, configuration->rings, YELLOW);
			}
			else
			{
				DrawCapsule(start_pos, end_pos, configuration->radius, configuration->slices, configuration->rings, YELLOW);
			}
		}
	}

	EndMode3D();
}
