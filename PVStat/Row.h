#pragma once
#include <iostream>
#include <string>
#include <vector>


class Row
{
	friend class Table;
public:
	Row();
	Row(std::stringstream& instream);
	operator bool() const;
	float& operator[](unsigned int index);
	std::string getDate() const;
	void print() const;
	~Row();
private:
	std::string dateAndTime;
	std::vector<float> fields;
	bool valid = false;
};
