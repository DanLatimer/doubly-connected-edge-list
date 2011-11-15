
#pragma once

#include "common.h"
#include "Polyline.h"
#include "RunLengthCoding.h"
#include "PrintManager.h"

class Run
{
public:
	int y;
	int xb;
	int xe;
};

class RasterImage
{
public:
	// Constructors
	RasterImage(
		const int width, 
		const int height, 
		const dnl::Polyline &polyline);
	RasterImage(const std::vector< std::vector<int> > &rasterData);

	// Methods
	RunLengthCoding getRunLengthCoding();
	void printRasterImage(PrintManager &printMan, int printWhat);

private:
	// Helper Methods
	void resizePolyline(
		const dnl::Polyline & input, 
		const int width, 
		const int height, 
		dnl::Polyline & output);
	void BresenhamsRasterizationAlgorithm(const dnl::Point & begin, const dnl::Point & end);
	void plot(int x, int y);
	void RasterizeData(const dnl::Polyline &polyline);
	void AugmentBoundary();
	void createRunLengthCoding(const std::vector< std::pair<int, int> > &augmentedBoundary);
	void rasterizeRunLengthCoding();

	// Private Data
	dnl::Polyline m_transformedPolyline;
	std::vector< std::vector<int> > m_rasterData;
	std::vector< std::pair<int, int> > m_boundary;
	std::vector< std::pair<int, int> > m_augmentedBoundary;
	std::vector< Run > m_runLengthCodingData;
	unsigned int m_rows;
	unsigned int m_columns;
};
