#include "LightningRenderer.h"

#include <iostream>

void LightningRenderer::render()
{	
	
	//if (segment_size * std::max(y_size, std::max(x_size,z_size)) > 50)
	//{
		segment_size = (GetScreenHeight()/9) / std::max(y_size, std::max(x_size, z_size));
	//}
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

	UpdateCamera(camera, CAMERA_ORBITAL);

	



	BeginMode3D(*camera);



	if (configuration->is_bounding_box)
	{
		DrawCubeWires(centre, x_size*segment_size, y_size * segment_size, z_size * segment_size, RAYWHITE);
	}


	float y_start = segment_size * (y_size - 1.5);

	for (int i = 0; i < lightning_points->size(); i++)
	{
		Vector3 start_pos = { lightning_points->at(i).parent_x * segment_size, y_start - (lightning_points->at(i).parent_y * segment_size) + y_offset, lightning_points->at(i).parent_z * segment_size };
		Vector3 end_pos = { lightning_points->at(i).x * segment_size, y_start - (lightning_points->at(i).y * segment_size) + y_offset, lightning_points->at(i).z * segment_size };

		DrawLine3D(end_pos, start_pos, YELLOW);
	}


	EndMode3D();
}
