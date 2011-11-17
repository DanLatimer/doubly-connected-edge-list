
#pragma once
/*
#include <cstddef>


#include <vector>


#define MAX 1000
*/
#include <string>
#include <iostream>
#include <vector>
#include <utility>
#include <memory>

#include "assert.h"
static void ReportError(std::string errorMsg)
{
	std::cout << "Epic Fail: " << errorMsg << std::endl;
	#ifdef _DEBUG
		assert(0);
	#endif
}



