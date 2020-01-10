#pragma once
#include <vector>
#include "Table.h"
class Table;
class Result
{
public:
	Result();
	~Result();
	//data members for the given csv
	bool valid = false;
	std::vector<std::string> spaceDesc;
	std::vector<float> mean;
	std::vector<float> max;
	std::vector<float> min;
	std::vector<float> total;
	float sellValue = 0.0f;
	float buyValue = 0.0f;
	float profitValue = 0.0f;
	float savedEnergy = 0.0f;
	// functions
	void print(Table &table) const;
};