#pragma once
#include "imgui.h"
#include "rlImGui.h"
#include "Config.h"
#include "implot3d.h"
#include "implot3d_internal.h"

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
};

