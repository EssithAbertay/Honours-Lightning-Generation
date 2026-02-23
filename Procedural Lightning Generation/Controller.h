#pragma once
#include "imgui.h"
#include "rlImGui.h"
#include "Config.h"
#include "implot3d.h"
#include "implot3d_internal.h"

#include "string"
#include <fstream>
#include <iostream>
#include <iomanip>

class Controller
{
public:
	Controller()
	{
		
	};

	~Controller()
	{

	};

	void init(bool dark_mode = false);
	void render(Config & configuration);

private:
	const char* MethodToString(GENERATION_METHOD m);
};

