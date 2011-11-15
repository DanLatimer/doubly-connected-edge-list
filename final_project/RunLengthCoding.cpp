
#include "stdafx.h"

#include "common.h"
#include "RunLengthCoding.h"

using namespace std;

RunLengthCoding::RunLengthCoding(const vector< vector<int> > &inputVector)
{
	assert(inputVector.size() > 0);

	m_rows = inputVector.size();
	m_columns = inputVector[0].size();

	int lastValue = 0;
	for(unsigned int i = 0; i < m_rows; i++)
	{
		for(unsigned int j = 0; j < m_columns; j++)
		{
			if(inputVector[i][j] != lastValue)
			{
				m_rlcData.push_back(pair<int, int>(i,j));
				lastValue = inputVector[i][j];
			}
		}
	}
}

void RunLengthCoding::output()
{
	if(m_rlcData.size() == 0)
	{
		return;
	}

	cout << "Run Length Coding switches colours at these points:" << endl;

	int previousFirst = m_rlcData[0].first;
	for(unsigned int i = 0; i < m_rlcData.size(); i++)
	{
		if(m_rlcData[i].first != previousFirst)
		{
			previousFirst = m_rlcData[i].first;
			cout << endl;
		}
		cout << "(" << m_rlcData[i].first << ", " << m_rlcData[i].second << ") ";
	}
}