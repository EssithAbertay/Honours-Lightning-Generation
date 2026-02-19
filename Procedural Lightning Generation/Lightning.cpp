#include "Lightning.h"

void Lightning::regenLightning()
{
	switch (configuration->method)
	{
	case unoptimised:
		generator.regenLightning_unoptimised();
		break;
	case optimised:
		break;
	case multithread:
		break;
	case other:
		break;
	default:
		generator.regenLightning_unoptimised();
		break;
	}
}

