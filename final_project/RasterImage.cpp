
#include "stdafx.h"

#include "common.h"
#include "RasterImage.h"
#include "RunLengthCoding.h"
#include "Point.h"
#include "math.h"
#include <algorithm>

using namespace std;
using namespace dnl;

void RasterImage::plot(int x, int y)
{
	//m_rasterData[y][x] = 1;
	m_boundary.push_back(std::pair<int, int>(x,y));
}

int sign(int value)
{
	if(value == 0)
	{
		return 0;
	}
	return (value < 0) ? -1 : 1;
}

int round(double value)
{
	if((value - floor(value)) < 0.5)
	{
		return floor(value);
	}
	return ceil(value);
}

void RasterImage::BresenhamsRasterizationAlgorithm(const dnl::Point & begin, const dnl::Point & end)
{
	const int x1 = round(begin.m_x);
	const int y1 = round(begin.m_y);
	const int x2 = round(end.m_x);
	const int y2 = round(end.m_y);

	int x = x1;
	int y = y1;
	
	int deltaX = abs(x2-x1);
	int deltaY = abs(y2-y1);

	int s1 = sign(x2 - x1);
	int s2 = sign(y2 - y1);

	bool interchange = false;

	if(deltaY > deltaX)
	{
		const int temp = deltaX;
		deltaX = deltaY;
		deltaY = temp;
		interchange = true;
	}
	else
	{
		interchange = false;
	}	

	int e = 2 * deltaY - deltaX;

	// Start loop
	for(double i = 1; i <= deltaX; i++)
	{
		plot(x, y);
		while(e >= 0)
		{
			if(interchange)
			{
				x = x + s1;
			}
			else
			{
				y = y + s2;
			}
			e = e - 2*deltaX;
		}
		if(interchange)
		{
			y = y + s2;
		}
		else
		{
			x = x + s1;
		}
		e = e + 2*deltaY;
	}
}

enum AugmentationState {
	LEVEL = 0,
	UP = 1,
	DOWN = 2
};

void RasterImage::AugmentBoundary()
{
	AugmentationState state, bstate;

	// trace backwords until a change in ordinate is found
	int nob = m_boundary.size() - 1;
	int x1 = m_boundary[0].first;
	int x2 = 0; //not used for a while
	int y1 = m_boundary[0].second;
	int y2 = y1;
	int i = nob + 1;
	bool even = false;

	while(y1 == y2 && i > 0)
	{
		i--;
		y2 = m_boundary[i].second;
	}
	int k = i;
	bstate = (y2 < y1) ? UP : DOWN;
	if(k < nob)
	{
		state == LEVEL;
		if(((nob - k) % 2) == 0)
		{
			even = false;
		}
		else
		{
			even = true;
		}
	}
	else
	{
		state = bstate;
	}

	// Copy these initial points
	int j = 0;
	for(i = k + 1; i <= nob; i++)
	{
		j++;
		m_augmentedBoundary.push_back(m_boundary[i]);
	}
	j++;
	m_augmentedBoundary.push_back(m_boundary[0]);
	
	// Check the rest of the boundary
	for(i = 1; i <= k; i++)
	{
		x2 = m_boundary[i].first;
		y2 = m_boundary[i].second;
		switch(state)
		{
		case LEVEL:
			{
				if(y2 == y1)
				{
					even = !even;
				}
				else if(y2 > y1)
				{
					if((even && bstate == UP) ||
					   (!even && bstate == DOWN))
					{
						j++;
						// Augment
						m_augmentedBoundary.push_back(m_boundary[i-1]);
						state = UP;
					}
				}
				else if(y2 < y1)
				{
					if((even && bstate == DOWN) || 
					   (!even && bstate == UP))
					{
						j++;
						// Augment
						m_augmentedBoundary.push_back(m_boundary[i-1]);
						state = DOWN;
					}
				}
				break;
			}
		case UP:
			{
				if(y2 < y1)
				{
					j++;
					// Augment
					m_augmentedBoundary.push_back(m_boundary[i-1]);
					state = DOWN;
				}
				else if(y2 == y1)
				{
					bstate = UP;
					even = true;
					state = LEVEL;
				}
				break;
			}
		case DOWN:
			{
				if(y2 > y1)
				{
					j++;
					// Augment
					m_augmentedBoundary.push_back(m_boundary[i-1]);
					state = UP;
				}
				else if(y2 == y1)
				{
					bstate = DOWN;
					even = true;
					state = LEVEL;
				}
				break;
			}
		default:
			assert(0);
		}

		j++;
		m_augmentedBoundary.push_back(m_boundary[i]);
		x1 = x2;
		y1 = y2;
	}
	
	// Check the last k'th position for augmentation
	if(k == nob)
	{
		y2 = m_boundary[0].second;
	}
	else
	{
		y2 = m_boundary[k+1].second;
	}
	switch(state)
	{
	case LEVEL:
		{
			if(y2 > y1 && 
			   ((even && bstate == UP) || 
			    (!even && bstate == DOWN)))
			{
				j++;
				m_augmentedBoundary.push_back(m_boundary[k]);
			}
			else if(y2 < y1 &&
				    ((even && bstate == DOWN) || 
			         (!even && bstate == UP)))
			{
				j++;
				m_augmentedBoundary.push_back(m_boundary[k]);
			}
			break;
		}
	case UP:
		{
			if(y2 < y1)
			{
				j++;
				m_augmentedBoundary.push_back(m_boundary[k]);
			}
			break;
		}
	case DOWN:
		{
			if(y2 > y1)
			{
				j++;
				m_augmentedBoundary.push_back(m_boundary[k]);
			}
			break;
		}
	default:
		assert(0);
	}
}

typedef std::pair<int, int> my_pair;

bool sortByFirst(const my_pair& left, const my_pair& right)
{
	return left.first < right.first;
}

bool sortByYthenX(const my_pair& left, const my_pair& right)
{
	if(left.second == right.second)
	{
		return left.first < right.first;
	}

	return left.second < right.second;
}

void assertNoDuplicates(const std::vector< std::pair<int, int> > &boundary)
{
	if(boundary.size() < 2)
	{
		return;
	}

	std::vector< std::pair<int, int> > boundaryCopy = boundary;
	
	// It's okay that it's a closed polygon.
	if(boundaryCopy[0] == boundaryCopy[boundaryCopy.size() - 1])
	{
		boundaryCopy.pop_back();
	}
	
	// Sort by X
	std::sort(boundaryCopy.begin(), boundaryCopy.end(), sortByFirst);

	// Do a pass over the data to ensure there are no duplicates
	my_pair last = boundaryCopy[0];
	for(unsigned int i = 1; i < boundaryCopy.size(); i++)
	{
		my_pair current = boundaryCopy[i];
		const bool sameAsLast = current.first == last.first && 
									current.second == last.second;
		//assert(!sameAsLast);

		last = current;
	}
}

void RasterImage::RasterizeData(const dnl::Polyline &polyline)
{
	const std::vector<dnl::Point> &points(polyline.m_points);
	for(unsigned int i = 0; i < points.size() - 1; i++)
	{
		BresenhamsRasterizationAlgorithm(points[i], points[i+1]);
	}
	if(!polyline.isClosed())
	{
		BresenhamsRasterizationAlgorithm(points[points.size() - 1], points[0]);
	}
	assertNoDuplicates(m_boundary);
	AugmentBoundary();

}

void RasterImage::resizePolyline(
	const dnl::Polyline & input, 
	const int width, 
	const int height, 
	dnl::Polyline & output)
{
	assert(width == height);
	int size = width;

	output.m_points.clear();
	output.m_name = input.m_name;

	// Transform all points in polyline
	for(unsigned int i = 0; i < input.m_points.size(); i++)
	{
		const Point &current(input.m_points[i]);
		
		const double xVal = current.m_x * size / 135;
		const double yVal = size - (current.m_y * size / 135);
		
		output.addPoint(Point(xVal, yVal));
	}
}

void RasterImage::rasterizeRunLengthCoding()
{
	
	for(unsigned int i = 0; i < m_runLengthCodingData.size(); i++)
	{
		int y = m_runLengthCodingData[i].y;
		int xb = m_runLengthCodingData[i].xb;
		int xe = m_runLengthCodingData[i].xe;

		int temp = (xb > xe) ? xe : xb;
		xe = (xb > xe) ? xb : xe;
		xb = temp;


		for(int j = xb; j <= xe; j++)
		{
			m_rasterData[y][j] = 1;
		}
	}
}

RasterImage::RasterImage(const int width, const int height, const dnl::Polyline &inputPolyline) 
	: m_rows(width), m_columns(height), m_transformedPolyline("")
{
	resizePolyline(inputPolyline, width, height, m_transformedPolyline);

	// Initialize m_rasterData to be all zeros.
	for(unsigned int i = 0; i < m_rows; i++)
	{
		std::vector<int> oneDimensionVector;
		for(unsigned int j = 0; j < m_columns; j++)
		{
			oneDimensionVector.push_back(0);
		}
		m_rasterData.push_back(oneDimensionVector);
	}

	RasterizeData(m_transformedPolyline);

	createRunLengthCoding(m_augmentedBoundary);
	rasterizeRunLengthCoding();
}

RasterImage::RasterImage(const std::vector< std::vector<int> > &rasterData) : m_transformedPolyline("")
{
	assert(rasterData.size() > 0);

	m_rows = rasterData.size();
	m_columns = rasterData[0].size();

	m_rasterData = rasterData;
}

RunLengthCoding RasterImage::getRunLengthCoding()
{
	return RunLengthCoding(m_rasterData);
}

Point pairToPoint(std::pair<int, int> value)
{
	return Point(value.first, value.second);
}

void RasterImage::printRasterImage(PrintManager &printMan, int printWhat)
{
	if(printWhat == 1)
	{
		char buffer[100];
		unsigned int boundarySize = m_augmentedBoundary.size();
		Point previous(pairToPoint(m_augmentedBoundary[boundarySize - 1]));
		for(unsigned int i = 0; i < boundarySize - 1; i++)
		{
			Point current(pairToPoint(m_augmentedBoundary[i]));
			Point next(pairToPoint(m_augmentedBoundary[i+1]));

			Colour *pointColour = NULL;
			if(current.m_x == previous.m_x &&
				current.m_y == previous.m_y)
			{
				pointColour = &printMan.m_solidRed;
			}

			printMan.PrintLine(current,	next, &printMan.m_solidBlack, 5);
			printMan.PrintPoint(current, 1, pointColour);
			//printMan.PrintText(current, itoa(i, buffer, 10), 1);

			previous = current;
		}
		
		printMan.PrintLine(pairToPoint(m_augmentedBoundary[boundarySize-1]),
			pairToPoint(m_augmentedBoundary[0]),	&printMan.m_solidBlack);
		printMan.PrintPoint(pairToPoint(m_augmentedBoundary[boundarySize-1]));
		//printMan.PrintText(pairToPoint(m_augmentedBoundary[boundarySize-1]), 
		//	itoa(m_augmentedBoundary.size()-1, buffer, 10), 1);
		

		// Print points from polyline in seethrough blue
		/*for(unsigned int i = 0; i < m_transformedPolyline.m_points.size(); i++)
		{
			printMan.FillPoint(m_transformedPolyline.m_points[i], &printMan.m_seeThroughBlue);
		}*/
	}

	if(printWhat == 2)
	{
		printMan.PrintRasterGrid(0, 0, m_columns, m_rows, m_rasterData);
	}

	if(printWhat == 3)
	{
		m_transformedPolyline.printPolygon(printMan);
	}
}

void RasterImage::createRunLengthCoding(
	const std::vector< std::pair<int, int> > &augmentedBoundary)
{
	std::vector< std::pair<int, int> > boundaryCopy = augmentedBoundary;

	// Sort by Y
	std::sort(boundaryCopy.begin(), boundaryCopy.end(), sortByYthenX);

	//i := 1; nrlc := 0; x := ab[1].x; y := ab[1].y;
	int nrlc = 0;
	int x = boundaryCopy[0].first;
	int y = boundaryCopy[0].second;
	int row = 0;
	int col1;
	int col2;
	bool odd;

	//while i £ nab do begin
	for(unsigned int i = 0; i < boundaryCopy.size() - 1; )
	{
        //row := y; col1 := x; odd := true;
		row = y;
		col1 = x;
		odd = true;

		bool rightRow;
		bool whatever;
		bool moreInput;
		do
		{
			//col2 := x; i := i + 1; odd := not odd;
			col2 = x;
			i++;
			odd = !odd;
            //x := ab[i].x; y := ab[i].y;
			x = boundaryCopy[i].first;
			y = boundaryCopy[i].second;
			
			rightRow = (y != row);
			whatever = (x - col2 > 1 && odd);
			moreInput = (i >= boundaryCopy.size() - 1);
		}
		//until (y != row) or (x - col2 > 1 and odd) or (i > nab);
		while(!(rightRow || whatever || moreInput));
		//nrlc := nrlc + 1; rlc[nrlc].y := row;
        //rlc[nrlc].xb := col1; rlc[nrlc].xe := col2;
		Run currentRun;
		currentRun.y = row;
		currentRun.xb = col1;
		currentRun.xe = col2;
		m_runLengthCodingData.push_back(currentRun);
	}
}
