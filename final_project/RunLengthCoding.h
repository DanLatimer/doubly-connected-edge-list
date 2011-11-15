
#pragma once

#include "common.h"

class RunLengthCoding
{
public:
	RunLengthCoding(const std::vector< std::vector<int> > &inputVector);

	// Methods
	void output();

public:
	std::vector< std::pair<int, int> > m_rlcData;
	unsigned int m_rows;
	unsigned int m_columns;
};
