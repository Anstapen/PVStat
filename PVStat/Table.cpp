#include "Table.h"
#include "Row.h"
#include "Result.h"
#include <iostream>
#include <fstream>
#include <istream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>


Table::Table()
{
}

Table::Table(std::string FilePath)
{
	//std::cout << "Given FilePath for CSV: " << FilePath << std::endl;
	// Create the necessary variables
	std::ifstream inFile;
	std::string line;
	std::string word;
	// Try to open the file, if unsuccessful, mark object as bad and return
	inFile.open(FilePath);
	if (!inFile) {
		//std::cout << "File open failed!\n";
		this->valid = false;
		return;
	}
	// get Space Descriptions
	getline(inFile, line);
	std::stringstream sLine(line);
	while (getline(sLine, word, ';')) {
		this->spaceDesc.push_back(word);
	}
	/*Should be exactly 9 column descriptions*/
	if (this->spaceDesc.size() != 9) {
		this->valid = false;
		return;
	}
	// Determine which Column is "Date"
	for (int i = 0; i < this->spaceDesc.size(); i++) {
		if (this->spaceDesc[i] == "Date") {
			//std::cout << "Found Date!" << i << std::endl;
			this->dateColumn = (unsigned int)i;
		}
	}
	// Create Line Objects
	std::string prnt;
	for (int i = 0; std::getline(inFile, line); i++) {
		sLine.str(line);
		prnt = sLine.str();
		Row *tmp = new Row(sLine);
		if (*tmp) {
			this->rows.push_back(*tmp);
		}
		else {
			delete tmp;
		}
	}
	// Set object to valid if there is more than one row
	if (this->size() > 1) {
		this->valid = true;
	}

}


Table::~Table()
{
}

Row & Table::operator[](unsigned int index)
{
	return this->rows[index];
}

Table::operator bool() const
{
	return this->valid;
}

void Table::print() const
{
	std::cout << "Contents of this Table:" << std::endl;

	//Print column descriptions
	std::cout << "|";
	for (int i = 0; i < this->spaceDesc.size(); i++) {
		std::cout << this->spaceDesc[i] << "|";
	}
	std::cout << std::endl;
	// print rows
	for (int i = 0; i < this->size(); i++) {
		this->rows[i].print();
	}
}

size_t Table::size() const
{
	return rows.size();
}

void Table::insertRow(Row in)
{
	// Space descirptions have to exist!
	if (this->spaceDesc.size() == 0) {
		return;
	}
	// the columns have to match!
	if (in.fields.size() != this->spaceDesc.size() - 1) {
		return;
	}
	this->rows.push_back(in);
}



/*
This is the analyser function.
*/
Result Table::analyse()
{
	Result ret;
	float tmp = 0.0f;
	float val = 0.0f;
	// Create vector entries for min, max and space Descriptions
	for (int i = 0; i < this->spaceDesc.size() - 1; i++) {
		ret.max.push_back(0.0f);
		ret.min.push_back(0.0f);
	}
	for (int i = 0; i < this->spaceDesc.size(); i++) {
		ret.spaceDesc.push_back(this->spaceDesc[i]);
	}
	// i = column, j = row
	for (int i = 0; i < this->spaceDesc.size() - 1; i++) {
		//add all values of 1 column
		for (int j = 0; j < this->size(); j++) {
			val = this->rows[j][i];
			tmp += val;
			//check min and max
			if (val > ret.max[i]) {
				ret.max[i] = val;
			}
			if (val < ret.min[i]) {
				ret.min[i] = val;
			}
		}
		ret.total.push_back(tmp);
		//divide by size to get the mean value
		tmp = tmp / (float)this->size();
		ret.mean.push_back(tmp);
		ret.total.push_back(tmp);
		tmp = 0.0f;
	}
	// Stromzukauf
	float eBuy = 0.0f;
	float eProfit = 0.0f;
	float eSell = 0.0f;
	for (int i = 0; i < this->rows.size(); i++) {
		eProfit += this->rows[i][6] * 0.00011f - this->rows[i][1] * 0.0003f;
		eBuy += this->rows[i][1] * 0.0003f;
		eSell += this->rows[i][6] * 0.00011f;
	}
	//Fill the Result Object
	ret.buyValue = eBuy;
	ret.profitValue = eProfit;
	ret.sellValue = eSell;
	ret.valid = true;
	return ret;
}

int Table::saveCSV(std::string FilePath) const
{
	std::ofstream outFile;
	std::ostringstream ss;
	std::string s;
	outFile.open(FilePath, std::ios_base::trunc);
	if (outFile.fail()) {
		return -1;
	}
	// Writing space descriptions
	for (int i = 0; i < this->spaceDesc.size(); i++) {
		outFile << this->spaceDesc[i] << ';';
	}
	outFile << '\n';
	// Writing all the rows
	// i = rows, j = columns
	for (int i = 0; i < this->rows.size(); i++) {
		outFile << this->rows[i].getDate() << ';';
		for (int j = 0; j < this->spaceDesc.size() - 1; j++) {
			ss << this->rows[i].fields[j];
			s.assign(ss.str());
			//std::replace(s.begin(), s.end(), '.', ',');
			outFile << s;
			//Clear the stringstream
			ss.str("");
			ss.clear();
			// last one does not have a semicolon
			if (j == this->spaceDesc.size() - 2) {
				outFile << std::endl;
			}
			else {
				outFile << ';';
			}
		}
	}
	outFile.close();
	//std::cout << "Saved CSV File to: " << FilePath << std::endl;
	return 0;
}

float Table::getOptimum(Result &in, Table &optimum, float cap, float power) const
{

	
	/*Table *optimump = new Table;
	Table &optimum = *optimump;*/
	float cap_1p = cap / 100;
	Row tmpRow;
	std::cerr << "Given Battery Capacity:" << cap << std::endl;
	std::cerr << "Given Battery Power:" << power << std::endl;
	std::cerr << "1% of Battery Capacity" << cap_1p << std::endl;
	//Create the colums for the row
	for (int i = 0; i < this->spaceDesc.size()-1; i++) {
		tmpRow.fields.push_back(0.0f);
	}

	std::cerr << "Filled fields Array of tmp Row: " << tmpRow.fields.size() << std::endl;

	//Copy space descriptions
	for (int i = 0; i < this->spaceDesc.size(); i++) {
		optimum.spaceDesc.push_back(this->spaceDesc[i]);
	}

	std::cerr << "Copied Space Descriptions" << optimum.spaceDesc.size() << std::endl;

	//Create the rows and copy the values needed
	for (int i = 0; i < this->rows.size(); i++) {
		// copy first battery percentage
		if (i == 0) {
			tmpRow.fields[4] = this->rows[i].fields[4];
		}
		if (i == 1) {
			tmpRow.fields[4] = 0.0f;
		}

		//Date and Time
		tmpRow.dateAndTime = this->rows[i].dateAndTime;

		//Gesamt-Stromerzeugung
		tmpRow.fields[7] = this->rows[i].fields[7];

		// Gesamt-Stromverbrauch
		tmpRow.fields[0] = this->rows[i].fields[0];

		optimum.insertRow(tmpRow);
	}

	std::cerr << "Created Rows:" << this->rows.size() << std::endl;

	//We have to check different scenarios
		/*
			Scenario number 1: Gesamt-Stromverbrauch > Gesamt-Stromerzeugung
			What has to be done?
			If the Battery is not empty, get the most amount of energy from it (max 2 kW of Power)
			If the Battery is empty, buy the rest...
		*/

		/*
			Scenario number 2: Gesamt-Stromverbrauch < Gesamt-Stromerzeugung
			What has to be done?
			If the Battery is full, sell the power.
			If the Battery is not full, charge the battery and sell the remaining power if p > 2kW
		*/
		//variables 
	float eNeeded = 0.0f;
	float eAvailable = 0.0f;
	float eStoreable = 0.0f;
	float youCanTake = 0.0f;
	for (int i = 1; i < optimum.rows.size(); i++) {
		//Scenario 1
		if (optimum[i][0] >= optimum[i][7]) {
			eNeeded = optimum[i][0] - optimum[i][7];
			// if eNeeded < 487,5 , all the power can be provided by the battery(theoretically)
			if (eNeeded < power) {
				//the battery has enough power stored
				if ((optimum[i - 1][4] * cap_1p) - eNeeded >= cap_1p) {
					// calculate Batterieversorgung
					optimum[i][2] = eNeeded;
					//calculate new battery percentage
					optimum[i][4] = ((optimum[i - 1][4] * cap_1p) - eNeeded) / cap_1p;
					// no energy has to be bought
					optimum[i][1] = 0.0f;
				}
				//the battery does not have enough capacity left to provide all the energy needed
				else {
					// take the rest out of the battery
					optimum[i][2] = ((optimum[i - 1][4] * cap_1p) - cap_1p);
					eNeeded = eNeeded - ((optimum[i - 1][4] * cap_1p) - cap_1p);
					// new battery percentage is at min
					optimum[i][4] = 1.0f;
					// the rest of the energy has to be bought
					optimum[i][1] = eNeeded;
				}
			}
			// more than 487,5 Wh are needed
			else {
				//take what you can from the battery the rest has to be bought
				youCanTake = (optimum[i - 1][4] * cap_1p) - cap_1p;
				// the battery can provide 487.5 KWh
				if (youCanTake - power >= 0.0f) {
					eNeeded = eNeeded - power;
					optimum[i][4] = optimum[i - 1][4] -(power / cap_1p);
				}
				else {
					eNeeded = eNeeded - youCanTake;
					optimum[i][4] = 1.0f;
				}
				// the rest has to be bought
				optimum[i][1] = eNeeded;
			}
		}
		//Scenario 2
		else if (optimum[i][0] < optimum[i][7]) {
			eAvailable = optimum[i][7] - optimum[i][0];
			//Check if the battery is full
			if (optimum[i - 1][4] >= 99.0f) {
				optimum[i][5] = eAvailable;
				optimum[i][3] = 0.0f;
				optimum[i][4] = 99.0f;
			}
			// if the battery is not full, store the maximum amount possible in it and sell the rest
			else {
				if (eAvailable >=power) {
					eStoreable = cap - (optimum[i - 1][4] * cap_1p) - cap_1p;
					if (eStoreable >= power) {
						eAvailable = eAvailable - power;
						optimum[i][4] = optimum[i - 1][4] + (power / cap_1p);
						optimum[i][5] = eAvailable;
						optimum[i][3] = power;
					}
					else {
						eAvailable = eAvailable - eStoreable;
						optimum[i][4] = 99.0f;
						optimum[i][5] = eAvailable;
						optimum[i][3] = eStoreable;
					}
				}
				// battery is not full and there are not more than 487.5 Wh to store
				else {
					eStoreable = cap - (optimum[i - 1][4] * cap_1p) - cap_1p;
					// the battery can store all the energy
					if (eStoreable >= eAvailable) {
						optimum[i][4] = optimum[i - 1][4] + (eAvailable / cap_1p);
						optimum[i][5] = 0.0f;
						optimum[i][3] = eAvailable;
					}
					else {
						eAvailable = eAvailable - eStoreable;
						optimum[i][4] = 99.0f;
						optimum[i][3] = eStoreable;
						optimum[i][5] = eAvailable;
					}
				}
			}
		}
		// the rest of the fields is just subtractio and addition

		// PV Selbstversorgung
		optimum[i][6] = optimum[i][7] - optimum[i][5] - optimum[i][3] + optimum[i][2];
	}

	in = optimum.analyse();
	return 0.0f;
}