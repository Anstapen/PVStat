#pragma once
#include <vector>
#include <string>
#include "Row.h"
#include "Result.h"


class Result;
class Table
{
public:
	//Constructors and Destructors
	Table();
	Table(std::string FilePath);
	~Table();
	// operator functions
	Row& operator[](unsigned int index);
	operator bool() const;
	// Getters
	std::vector<std::string> spaceDesc;
	void print() const;
	size_t size() const;
	// Setters
	void insertRow(Row in);
	//Analyser function
	Result analyse();
	float getOptimum(Result &in, Table &optimum, float batteryCapacity = 7500, float batteryPower = 500) const;
	// Save to File
	int saveCSV(std::string FilePath) const;
private:
	unsigned int dateColumn = 0;
	std::vector<Row> rows;
	bool valid = false;
};