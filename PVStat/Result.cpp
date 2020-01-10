#include "Result.h"
#include "Table.h"
#include <iostream>



Result::Result()
{
}


Result::~Result()
{
}

void Result::print(Table &table) const
{
	for (int i = 0; i < this->mean.size(); i++) {
		std::cout << "Column " << table.spaceDesc[i + 1] << ": " << std::endl;
		std::cout << "Mean(Given): " << this->mean[i] << " Wh " << "Mean(Analysis): " << this->mean[i] << " Wh" << std::endl;
		std::cout << "Maximum(Given):" << this->max[i] << " Wh " << "Maximum(Analysis):" << this->max[i] << " Wh" << std::endl;
		std::cout << std::endl;
	}
	std::cout << "Buy/Sell Values: " << std::endl;
	std::cout << "Sold Energy: " << this->sellValue << " Euro " << std::endl;
	std::cout << "Bought Energy: " << this->buyValue << " Euro " << std::endl;
	std::cout << "Profit: " << this->profitValue << " Euro " << std::endl;
}
