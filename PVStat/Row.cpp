#include "Row.h"
#include <sstream>
#include <algorithm>



Row::Row()
{
}

Row::Row(std::stringstream & instream)
{
	/*
	Quick fix  for the stringstream problem:
	*/
	std::stringstream getstream(instream.str());
	// Input is one Row
	std::string tmp;
	//Get the date (first column of every row)
	std::getline(getstream, tmp, ';');
	this->dateAndTime = tmp;
	while (std::getline(getstream, tmp, ';')) {
		//check if the line is valid
		if (tmp == "\"---\"") {
			this->valid = false;
			return;
		}
		else {
			//replace the komma for float conversion
			//std::replace(tmp.begin(), tmp.end(), ',', '.');
			this->fields.push_back(atof(tmp.c_str()));
			this->valid = true;
		}
	}
}


Row::operator bool() const
{
	return this->valid;
}

float& Row::operator[](unsigned int index)
{
	return this->fields[index];
}

std::string Row::getDate() const
{
	std::string ret = this->dateAndTime;
	return ret;
}

void Row::print() const
{
	std::cout << this->dateAndTime << "|";
	for (int i = 0; i < this->fields.size(); i++) {
		std::cout << "\t" << this->fields[i] << "\t" << "|";
	}
	std::cout << std::endl;
}

Row::~Row()
{
}
