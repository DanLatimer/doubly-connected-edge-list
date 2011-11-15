
#include "stdafx.h"

#include "common.h"
#include "Algorithms.h"

using namespace std;

void Algorithms::VectorToRLC(
	vector< vector<int> > &inputVector, 
	vector< pair<int, int> > &rlc)
{
	assert(inputVector.size() > 0);

	const unsigned int vectorHeight = inputVector.size();
	const unsigned int vectorWidth = inputVector[0].size();

	int lastValue = 0;
	for(unsigned int i = 0; i < vectorHeight; i++)
	{
		for(unsigned int j = 0; j < vectorWidth; j++)
		{
			if(inputVector[i][j] != lastValue)
			{
				rlc.push_back(pair<int, int>(i,j));
				lastValue = inputVector[i][j];
			}
		}
	}
}