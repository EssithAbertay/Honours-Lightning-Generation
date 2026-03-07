#include "Lightning.h"

void Lightning::regenLightning()
{
	configuration->grid_steps = 0;

	generator.setVars();

	switch (configuration->method)
	{
	case unoptimised:
		std::cout << "Unoptimised" << std::endl;
		generator.regenLightning_unoptimised();
		break;
	case optimised:
		std::cout << "Optimised" << std::endl;
		generator.regenLightning_optimised();
		break;
	case multithread:
		std::cout << "Multithreaded" << std::endl;
		break;
	case other:
		std::cout << "Alt" << std::endl;
		break;
	default:
		generator.regenLightning_unoptimised();
		break;
	}

	configuration->grid_steps = generator.getGridSteps();

}

