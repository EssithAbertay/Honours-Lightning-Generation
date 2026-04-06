#pragma once
#include <map>
#include <vector>
#include "Config.h"

struct TestConditions // struct to store conditions of test
{
	SavedGeneration setup;
};


struct TestResults_Time // struct to store result of test
{
	std::vector<float> times;
	float max_time;
	float min_time;
	float average_time;
};

struct TestResults_Target // struct to store result of test
{
	std::map<int, int> struck_cells; // first int sotres the index of the cell, second int stores the number of times it has been struck
};

struct TestResults_Other // struct to store  result of test
{
	int number_of_segments;
	int main_segment_length;
};

struct TestData
{
	TestConditions conditions;
	TestResults_Time time_results;
	TestResults_Target target_results;
	TestResults_Other other_results;
};