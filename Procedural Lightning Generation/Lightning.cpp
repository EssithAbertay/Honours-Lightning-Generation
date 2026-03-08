#include "Lightning.h"

void Lightning::regenLightning()
{
	configuration->grid_steps = 0;

	generator.setVars();

	generator.regenLightning_optimised();

	configuration->grid_steps = generator.getGridSteps();
}

