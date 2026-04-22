#pragma once
#include "imgui.h"
#include "rlImGui.h"
#include "Config.h"

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
	void render(Config & configuration, std::vector<TestData> & tests);

private:
	Config config_defaults;


	const char* CandidateSelectionToString(bool candidate_selection);
};

