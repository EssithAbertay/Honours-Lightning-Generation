#pragma once
#include <vector>
#include "Config.h"

class LightningVolume // helper class for lightning volumes, deprecated
{
public:
	LightningVolume();
	~LightningVolume();

	void setConfig(Config* config)
	{
		configuration = config;
	}

	void buildVolume();
	void setCellValue(int x, int  y, int z, float value);
	float getCellValue(int x, int  y, int z);
private:
	std::vector<float> volume;
	Config* configuration = nullptr;
};

