#pragma once

#include <string>

class Extractor
{
public:
	Extractor(const std::string & videoFilePath, double from = -1, double to = -1);
	~Extractor();
};

