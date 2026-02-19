#include "LightningVolume.h"

LightningVolume::LightningVolume()
{
}

LightningVolume::~LightningVolume()
{
}

void LightningVolume::buildVolume()
{
	volume.clear();
	volume.resize(configuration->z_size * configuration->y_size * configuration->x_size);
}

void LightningVolume::setCellValue(int x, int y, int z, float value)
{
	int position = (z * (configuration->y_size * configuration->x_size)) + (y * configuration->x_size) + x;
	volume[position] = value;
}

float LightningVolume::getCellValue(int x, int y, int z)
{
	int position = (z * (configuration->y_size * configuration->x_size)) + (y * configuration->x_size) + x;
	return volume[position];
}
